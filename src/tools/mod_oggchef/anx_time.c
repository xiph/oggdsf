/*
   Copyright (C) 2003 Commonwealth Scientific and Industrial Research
   Organisation (CSIRO) Australia

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of CSIRO Australia nor the names of its
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

#include "anx_time.h"

#include <stdio.h>
#include <math.h>

static double
parse_npt (const char *str)
{
  int h=0,m=0, n;
  float s;
  double result;

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
  if (h < 0) return -1;
  if (m > 59 || m < 0) return -1;
  if (s >= 60.0 || s < 0) return -1;

 sec_only:

  result = ((h * 3600.0) + (m * 60.0) + s);

  return result;
}

/* parse_smpte: parse a smpte-string */
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
  if (frames > (float)ceil(framerate) || frames < 0) return -1;

  result = ((h * 3600.0) + (m * 60.0) + s) + (frames/framerate);

  return result;

#if 0
  char *p;
  int maxframes;
  maxframes = (int) ceil(framerate) - 1;

  hours = strtol(s, &p, 10);
  if (*p != ':' || !isdigit(*(p+1))) {
    secs = hours;
    hours = 0;
    goto return_time;
  }
  p++;

  mins = strtol(p, &p, 10);
  if (*p != ':' || !isdigit(*(p+1))) {
    secs = mins;
    mins = hours;
    hours = 0;
    goto return_time;
  }
  p++;

  secs = strtol(p, &p, 10);
  if (*p != ':' || !isdigit(*(p+1))) {
    goto return_time;
  }

  if (*p == ':') { 
    frames = strtol(++p, &p, 10);
  }

 return_time:
  if (*p == '.') {
    subframes = strtol(++p, &p, 10);
  }

  /*
  fprintf(stderr, "hours=%d min=%d sec=%d frames=%d subframes=%d\n", hours, mins
, secs, frames, subframes);
  */

  /* check valid time specs */
  if (hours<0) return -1;
  if (mins>59 || mins<0) return -1;
  if (secs>59 || secs<0) return -1;
  if (frames>maxframes || frames<0) return -1;
  if (subframes>99 || subframes<0) return -1;

  return ( 1.0*hms_to_secs(hours, mins, secs)
           + (1.0*frames/framerate)
           + (1.0*subframes/(100.0*framerate)) );
#endif
}

double
anx_parse_time (const char * str)
{
  char timespec[16];

  if (str == NULL) return -1.0;

  if (sscanf (str, "npt:%16s", timespec) == 1) {
    return parse_npt (timespec);
  }

  if (sscanf (str, "smpte-24:%16s", timespec) == 1) {
    return parse_smpte (timespec, 24.0);
  }
  
  if (sscanf (str, "smpte-24-drop:%16s", timespec) == 1) {
    return parse_smpte (timespec, 23.976);
  }
  
  if (sscanf (str, "smpte-25:%16s", timespec) == 1) {
    return parse_smpte (timespec, 25.0);
  }
  
  if (sscanf (str, "smpte-30:%16s", timespec) == 1) {
    return parse_smpte (timespec, 30.0);
  }
  
  if (sscanf (str, "smpte-30-drop:%16s", timespec) == 1) {
    return parse_smpte (timespec, 29.97);
  }
  
  if (sscanf (str, "smpte-50:%16s", timespec) == 1) {
    return parse_smpte (timespec, 50.0);
  }
  
  if (sscanf (str, "smpte-60:%16s", timespec) == 1) {
    return parse_smpte (timespec, 60);
  }

  if (sscanf (str, "smpte-60-drop:%16s", timespec) == 1) {
    return parse_smpte (timespec, 59.94);
  }

  return parse_npt(str);
}
