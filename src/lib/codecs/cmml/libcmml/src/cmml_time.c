/* Copyright (C) 2003 CSIRO Australia

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the CSIRO nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* cmml_time.c */

#include <assert.h>
#include <ctype.h> /* isdigit () */
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

/* include files for cmml only */
#include "cmml_private.h"
#include "cmml.h"

/* for debugging functions in this file
#define DEBUG
*/


/* parse_npt: INLINE function to parse a npt-string */
static double
parse_npt(const char *str)
{
  int h=0,m=0,n;
  float s;
  double result;

#ifdef DEBUG
  fprintf(stderr,"parse_npt: %s\n", str);
#endif

  n = sscanf (str, "%d:%d:%f",  &h, &m, &s);
  if (n == 3) {
    goto done;
  }
  
  n = sscanf (str, "%d:%f",  &m, &s);
  if (n == 2) {
    h = 0;
    goto done;
  }

  n = sscanf (str, "%f", &s);
  if (n == 1) {
    h = 0; m = 0;
    goto sec_only;
  }
  
  return -1.0;

 done:
  /* check valid time specs */
  if (h<0) return -1;
  if (m>59 || m<0) return -1;
  if (s>=60.0 || s<0.0) return -1;

 sec_only:
  result = ((h * 3600.0) + (m * 60.0) + s);
  
  return result;
}

/* parse_smpte: INLINE function to parse a smpte-string */
static double
parse_smpte(const char *str, double framerate)
{
  int h = 0, m = 0, s = 0, n;
  float frames;
  double result;

  n = sscanf (str, "%d:%d:%d:%f", &h, &m, &s, &frames);
  if (n == 4) {
    goto done;
  }

  n = sscanf (str, "%d:%d:%f", &m, &s, &frames);
  if (n == 3) {
    h = 0;
    goto done;
  }

  return -1.0;

 done:
  /* check valid time specs */
  if (h < 0) return -1;
  if (m > 59 || m < 0) return -1;
  if (s > 59 || s < 0) return -1;
#ifdef DEBUG
  fprintf(stderr,"frames  = %f\n", frames);
#endif
  if (frames > (float)ceil(framerate) || frames < 0) return -1;
  result = ((h * 3600.0) + (m * 60.0) + s) + (frames/framerate);

  return result;
}


/* creates a time struct from any name-value time spec pairs */
CMML_Time *
cmml_time_new (const char * s)
{
  /* parse utc spec "clock:" */
  if (strncmp(s, "clock:", 6) == 0) {
    return cmml_utc_new(s+6);
  }
  return cmml_sec_new(s);
}

/* creates a time struct only from name-value sec specs in npt or smpte */
CMML_Time *
cmml_sec_new (const char * s)
{
  CMML_Time * t;
  t = (CMML_Time *) cmml_malloc (sizeof (*t));

#ifdef DEBUG
  fprintf(stderr, "creating new play time: %s\n", s);
#endif

  t->tstr = cmml_strdup(s);
  t->type = CMML_SEC_TIME;
  t->t.sec = cmml_sec_parse(s);

  return t;
}

/* creates a time struct only from name-value sec specs in utc */
CMML_Time *
cmml_utc_new (const char * s)
{
  CMML_Time * t;
  t = (CMML_Time *) cmml_malloc (sizeof (*t));

#ifdef DEBUG
  fprintf(stderr, "creating new utc time: %s\n", s);
#endif

  t->tstr = cmml_strdup(s);
  t->type = CMML_UTC_TIME;
  t->t.utc = cmml_utc_parse(s);

  return t;
}

/* handles start and end times for clip tags, where we will only store sec offsets */
CMML_Time *
cmml_time_new_in_sec (const char * s, CMML_Time * ref, double base)
{
  CMML_Time * t;

  t = cmml_time_new (s);
  return cmml_time_utc_to_sec (t, ref, base);
}

/* convert a time from UTC to secs with ref to a time base */
CMML_Time *
cmml_time_utc_to_sec (CMML_Time * t, CMML_Time * ref, double base)
{
  double timediff;
  char txtsec[100];

  if (t->type == CMML_UTC_TIME && ref != NULL) {
    /* convert to seconds representation */
    timediff = cmml_utc_diff (t->t.utc, ref->t.utc);
#ifdef DEBUG
    fprintf(stderr, "comparing %s with %s\n", t->tstr, ref->tstr);
    fprintf(stderr, "UTC time parsed in stream tag\n");
    fprintf(stderr, "Year=%d\tMonth=%d\tDay=%d\n", 
	    t->t.utc->tm_year, t->t.utc->tm_mon, t->t.utc->tm_mday);
    fprintf(stderr, "Hour=%d\tMinutes=%d\tSec=%d\tHSec=%d\n",
	    t->t.utc->tm_hour, t->t.utc->tm_min, 
	    t->t.utc->tm_sec, t->t.utc->tm_hsec);
    fprintf(stderr, "Year=%d\tMonth=%d\tDay=%d\n", 
	    ref->t.utc->tm_year, ref->t.utc->tm_mon, ref->t.utc->tm_mday);
    fprintf(stderr, "Hour=%d\tMinutes=%d\tSec=%d\tHSec=%d\n",
	    ref->t.utc->tm_hour, ref->t.utc->tm_min, 
	    ref->t.utc->tm_sec, ref->t.utc->tm_hsec);
    fprintf(stderr, "creating new time with %f sec\n", timediff);
#endif
    cmml_time_free(t);
    timediff += base;
    cmml_snprintf (txtsec, 100, "%f", timediff);
    t = cmml_time_new (txtsec);
  }
  return t;
}


/* creates two time constructs if the string was a time range, otherwise
   just a t_start time, and returns the number of times created;
   examples: npt:40,79 or smpte-25:00:20:20,00:21:30 */
int 
cmml_time_interval_new (const char * s, 
			CMML_Time ** t_start, CMML_Time ** t_end)
{
  char * after_scheme = NULL;
  char * pos = NULL;
  char * start_time = NULL;
  int len_start_time = 0;
  double seconds = -1.0;

  if (s == NULL) return -1;
  after_scheme = strchr(s, ':');
  if (after_scheme == NULL) after_scheme = (char *) s;
  if ((pos=strchr(after_scheme, ',')) == NULL) {
    /* it's just a time point */
    *t_end = NULL;
    *t_start = cmml_time_new(s);
#ifdef DEBUG
    fprintf(stderr, "start time %f\n", (*t_start)->t.sec);
#endif
    return 1;
  } else {
    /* it's a time range */
    len_start_time = (int)(strlen(s) - strlen(pos));
    start_time = (char *) cmml_malloc (len_start_time+1);
    start_time = strncpy (start_time, s, len_start_time);
    start_time[len_start_time]='\0';
    *t_start = cmml_time_new(start_time);
    if ((*t_start)->type == CMML_UTC_TIME) {
      *t_end = cmml_utc_new(pos+1);
    } else {
      /* Find out what type the timestamp is and parse appropriately */
      if (strncmp(s, "smpte-24", 8) == 0) {
	seconds = parse_smpte(pos+1, 24.0);
      } else if (strncmp(s, "smpte-24-drop", 13) == 0) {
	seconds = parse_smpte(pos+1, 23.976);
      } else if (strncmp(s, "smpte-25", 8) == 0) {
	seconds = parse_smpte(pos+1, 25.0);
      } else if (strncmp(s, "smpte-30", 8) == 0) {
	seconds = parse_smpte(pos+1, 30.0);
      } else if (strncmp(s, "smpte-30-drop", 13) == 0) {
	seconds = parse_smpte(pos+1, 29.97);
      } else if (strncmp(s, "smpte-50", 8) == 0) {
	seconds = parse_smpte(pos+1, 50.0);
      } else if (strncmp(s, "smpte-60", 8) == 0) {
	seconds = parse_smpte(pos+1, 60.0);
      } else if (strncmp(s, "smpte-60-drop", 13) == 0) {
	seconds = parse_smpte(pos+1, 59.94);
      } else {
	/* default is npt */
	*t_end = cmml_time_new(pos+1);
      } 
      /* create t_end for the smpte timecodes */
      if (seconds > 0) {
	*t_end = (CMML_Time *) cmml_malloc (sizeof (CMML_Time));
	(*t_end)->tstr = cmml_strdup(pos+1);
	(*t_end)->type = CMML_SEC_TIME;
	(*t_end)->t.sec = seconds;
      }
    }
    return 2;
  }
  return -1;
}


/* frees memory of a time struct */
void cmml_time_free (CMML_Time *t)
{
  if (t == NULL) return;
  cmml_free(t->tstr);
  if (t->type == CMML_UTC_TIME) {
    cmml_free(t->t.utc);
  }
  cmml_free(t);
  return;
}

/* copies time structs */
CMML_Time *
cmml_time_clone (CMML_Time * t1)
{
  CMML_Time * t = NULL;
  if (t1 == NULL) return NULL;
  t = (CMML_Time*) cmml_malloc (sizeof (CMML_Time));
  t->tstr = cmml_strdup(t1->tstr);
  t->type = t1->type;
  if (t->type == CMML_UTC_TIME) {
    t->t.utc = cmml_utc_clone (t1->t.utc);
  } else if (t->type == CMML_SEC_TIME) {
    t->t.sec = t1->t.sec;
  }

  return t;
}

/* parse_sec: parse a sec-string of stream, head or clip tag */
/* this parses a name-value pair of npt:xxx, smpte:xxx etc. */
double
cmml_sec_parse (const char * str)
{
  char timespec[16];

  if (str == NULL) return -1.0;

  if (sscanf (str, "npt:%16s", timespec) == 1) {
    return parse_npt (str+4); /* could be longer than 16 :) */
  }

  if (sscanf (str, "smpte-24:%16s", timespec) == 1) {
    return parse_smpte (str+9, 24.0);
  }
  
  if (sscanf (str, "smpte-24-drop:%16s", timespec) == 1) {
    return parse_smpte (str+14, 23.976);
  }
  
  if (sscanf (str, "smpte-25:%16s", timespec) == 1) {
    return parse_smpte (str+9, 25.0);
  }
  
  if (sscanf (str, "smpte-30:%16s", timespec) == 1) {
    return parse_smpte (str+9, 30.0);
  }
  
  if (sscanf (str, "smpte-30-drop:%16s", timespec) == 1) {
    return parse_smpte (str+14, 29.97);
  }
  
  if (sscanf (str, "smpte-50:%16s", timespec) == 1) {
    return parse_smpte (str+9, 50.0);
  }
  
  if (sscanf (str, "smpte-60:%16s", timespec) == 1) {
    return parse_smpte (str+9, 60);
  }

  if (sscanf (str, "smpte-60-drop:%16s", timespec) == 1) {
    return parse_smpte (str+14, 59.94);
  }

  return parse_npt(str);

}

/* parse_utc: parse a utc-string of utc-tag in head or of clips */
/* this is just parsing the plain utc string YYYYMMDDTHHmmss.hhZ */
CMML_UTC *
cmml_utc_parse (const char * s)
{
  CMML_UTC * t;
  int n;
  char tm_year[5], tm_month[3], tm_day[3], tm_hour[3], tm_min[3], tm_sec[3];

  t = (CMML_UTC *) cmml_malloc (sizeof (*t));

  /* try parsing and return NULL in case of failure */
  n = sscanf (s, "%4s%2s%2s%*c%2s%2s%2s.%hi", tm_year, tm_month, tm_day, 
              tm_hour, tm_min, tm_sec, &(t->tm_hsec));
#if 0
  if (n==6 || n==7) fprintf(stderr, "Successfully ");
  fprintf(stderr, "Parsed UTC time: %s\n", s);
  fprintf(stderr, "Year=%4s\tMonth=%2s\tDay=%2s\n", 
	  tm_year, tm_month, tm_day);
  fprintf(stderr, "Hour=%2s\tMinutes=%2s\tSec=%2s\tHSec=%d\n",
	  tm_hour, tm_min, tm_sec, t->tm_hsec);
#endif

  if (n==6 || n==7) {
    /* successful parsing: assign values to fields */
    t->tm_year = (short) atoi(tm_year);
    t->tm_mon  = (short) atoi(tm_month);
    t->tm_mday = (short) atoi(tm_day);
    t->tm_hour = (short) atoi(tm_hour);
    t->tm_min  = (short) atoi(tm_min);
    t->tm_sec  = (short) atoi(tm_sec);

#ifdef DEBUG
    fprintf(stderr, "Year=%d\tMonth=%d\tDay=%d\n", 
	    t->tm_year, t->tm_mon, t->tm_mday);
    fprintf(stderr, "Hour=%d\tMinutes=%d\tSec=%d\tHSec=%d\n",
	    t->tm_hour, t->tm_min, t->tm_sec, t->tm_hsec);
#endif
    
    /* check valid time specs */
    if ((t->tm_mon < 1 || t->tm_mon > 12) ||
        (t->tm_mday < 1 || t->tm_mday > 31) ||
        (t->tm_hour < 0 || t->tm_hour > 23) ||
        (t->tm_min < 0 || t->tm_min > 59) ||
        (t->tm_sec < 0 || t->tm_sec > 59) ||
        (t->tm_hsec < 0 || t->tm_hsec > 99)) {
      cmml_free(t);
      return NULL;
    } else {
      return t;
    }
  } else {
    cmml_free (t);
    return NULL;
  }
}

/* copy a utc time */
CMML_UTC *
cmml_utc_clone (CMML_UTC * t1)
{
  CMML_UTC * t;
  if (t1 == NULL) return NULL;
  t = (CMML_UTC *) cmml_malloc (sizeof (*t));
  t->tm_hsec = t1->tm_hsec;
  t->tm_sec  = t1->tm_sec;
  t->tm_min  = t1->tm_min;
  t->tm_hour = t1->tm_hour;
  t->tm_mday = t1->tm_mday;
  t->tm_mon  = t1->tm_mon;
  t->tm_year = t1->tm_year;
  return t;
}

/* difference of two utc times in seconds */
double
cmml_utc_diff (CMML_UTC *t2, CMML_UTC *t1)
{
  double diffsec;

  diffsec = (t2->tm_hsec - t1->tm_hsec)/100;
  diffsec += (t2->tm_sec - t1->tm_sec);
  diffsec += 60.0*(t2->tm_min - t1->tm_min);
  diffsec += 3600.0*(t2->tm_hour - t1->tm_hour);
  diffsec += 24.0*3600*(t2->tm_mday - t1->tm_mday);
  /* presuming there are 30.4375 days in each month (=365.25/12)*/
  diffsec += 30.4375*24*3600*(t2->tm_mon - t1->tm_mon);
  diffsec += 12.0*30*24*3600*(t2->tm_year - t1->tm_year);

#ifdef DEBUG
  fprintf(stderr, "t1: Year=%d\tMonth=%d\tDay=%d\n", 
	  t1->tm_year, t1->tm_mon, t1->tm_mday);
  fprintf(stderr, "Hour=%d\tMinutes=%d\tSec=%d\tHSec=%d\n",
	  t1->tm_hour, t1->tm_min, t1->tm_sec, t1->tm_hsec);
  fprintf(stderr, "t2: Year=%d\tMonth=%d\tDay=%d\n", 
	  t2->tm_year, t2->tm_mon, t2->tm_mday);
  fprintf(stderr, "Hour=%d\tMinutes=%d\tSec=%d\tHSec=%d\n",
	  t2->tm_hour, t2->tm_min, t2->tm_sec, t2->tm_hsec);
  fprintf(stderr, "diffsec = %f\n", diffsec);
#endif

  return diffsec;

  /* seems like the system time functions don't work */
#if 0
  time_t time1, time2;
  struct tm tmptm1;
  struct tm tmptm2;
  double diffreturn;

  if (t1 == NULL || t2 == NULL) {
    return -1;
  }

  /* transform t1 to struct tm and seconds from EPOCH */
  tmptm1.tm_sec  = t1->tm_sec;
  tmptm1.tm_min  = t1->tm_min;
  tmptm1.tm_hour = t1->tm_hour;
  tmptm1.tm_mday = t1->tm_mday;
  tmptm1.tm_mon  = t1->tm_mon-1;
  tmptm1.tm_year = t1->tm_year-1900;
  time1 = mktime(&tmptm1);
#ifdef DEBUG
  fprintf(stderr,"time1 %ld\n", time1);
  fprintf(stderr, "Year=%d(%d)\tMonth=%d(%d)\tDay=%d(%d)\n", 
	  tmptm1.tm_year, t1->tm_year, tmptm1.tm_mon, t1->tm_mon, tmptm1.tm_mday, t1->tm_mday);
  fprintf(stderr, "Hour=%d(%d)\tMinutes=%d(%d)\tSec=%d(%d)\tHSec=0(%d)\n",
	  tmptm1.tm_hour, t1->tm_hour, tmptm1.tm_min, t1->tm_min, tmptm1.tm_sec, t1->tm_sec, 
	  t1->tm_hsec);
#endif

  /* convert t2 to struct tm and seconds from EPOCH */
  tmptm2.tm_sec  = t2->tm_sec;
  tmptm2.tm_min  = t2->tm_min;
  tmptm2.tm_hour = t2->tm_hour;
  tmptm2.tm_mday = t2->tm_mday;
  tmptm2.tm_mon  = t2->tm_mon-1;
  tmptm2.tm_year = t2->tm_year-1900;
  time2 = mktime(&tmptm2);
#ifdef DEBUG
  fprintf(stderr,"time2 %ld\n", time2);
  fprintf(stderr, "Year=%d(%d)\tMonth=%d(%d)\tDay=%d(%d)\n", 
	  tmptm2.tm_year, t2->tm_year, tmptm2.tm_mon, t2->tm_mon, tmptm2.tm_mday, t2->tm_mday);
  fprintf(stderr, "Hour=%d(%d)\tMinutes=%d(%d)\tSec=%d(%d)\tHSec=0(%d)\n",
	  tmptm2.tm_hour, t2->tm_hour, tmptm2.tm_min, t2->tm_min, tmptm2.tm_sec, t2->tm_sec, 
	  t2->tm_hsec);
#endif

  /* use system's time difference functions for calculation */
  diffreturn = (double) difftime (time2, time1);
  fprintf(stderr, "diffreturn=%f\n", diffreturn);
  diffreturn += (t2->tm_hsec - t1->tm_hsec)/100;

  return diffreturn;
#endif

}


/*
 * cmml_npt_snprint: prints npt time to a string in the
 * format Hmmss.hh
 */
int cmml_npt_snprint (char * buf, int n, double seconds)
{
  int hrs, min;
  double sec;
  char * sign;

  sign = (seconds < 0.0) ? "-" : "";

  if (seconds < 0.0) seconds = -seconds;

  hrs = (int) (seconds/3600.0);
  min = (int) ((seconds - ((double)hrs * 3600.0)) / 60.0);
  sec = seconds - ((double)hrs * 3600.0)- ((double)min * 60.0);

  /* XXX: %02.3f workaround */
  if (sec < 10.0) {
    return sprintf (buf, "%s%2d:%02d:0%2.3f", sign, hrs, min, sec);
  } else {
    return sprintf (buf, "%s%2d:%02d:%02.3f", sign, hrs, min, sec);
  }
}

/*
 * cmml_utc_snprint: print utc time to a string in the
 * format YYYYMMDDTHHmmss.hhZ
 */
int cmml_utc_snprint (char *buf, int n, CMML_UTC *t) 
{
  if (t == NULL) return 0;
  if (n < 20) return 0;
  
  return sprintf (buf, "%4d%2d%2dT%2d%2d%2d.%3d", 
		  t->tm_year,
		  t->tm_mon,
		  t->tm_mday,
		  t->tm_hour,
		  t->tm_min,
		  t->tm_sec,
		  t->tm_hsec);
}

/*
 * cmml_utc_pretty_snprint: print utc time to a string in the
 * format "DATE: MM/DD/YYYY TIME: HH:mm:ss.hh"
 */
int cmml_utc_pretty_snprint (char *buf, int n, CMML_UTC *t)
{
  int n_written;
  if (t == NULL) return 0;
  n_written = sprintf (buf, "DATE: %02d/%02d/%04d TIME: %02d:%02d:%02d.%03d", 
		       t->tm_mon,
		       t->tm_mday,
		       t->tm_year,

		       t->tm_hour,
		       t->tm_min,
		       t->tm_sec,
		       t->tm_hsec);
  
  return n_written;
}
