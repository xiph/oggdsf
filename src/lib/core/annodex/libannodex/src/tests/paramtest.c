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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <annodex/anx_params.h>

#include "anx_tests.h"

static char * h =
"Fish-Type: haddock\r\n"
"Cheese-Age: mouldy\r\n"
"Moose-Flavor: red\r\n"
"Cheese-Age: green\r\n";

static char * q =
"Fish-Type=haddock&Cheese-Age=mouldy&Moose-Flavor=red&Cheese-Age=green";

static char * m =
"Fish-Type=bream";

static char * h_header =
"Fish-Type: haddock\r\n"
"Cheese-Age: mouldy,green\r\n"
"Moose-Flavor: red\r\n";

static char * h_query =
"Fish-Type=haddock&Cheese-Age=mouldy,green&Moose-Flavor=red";

static char * h_meta =
"<meta name=\"Fish-Type\" content=\"haddock\"/>\n"
"<meta name=\"Cheese-Age\" content=\"mouldy,green\"/>\n"
"<meta name=\"Moose-Flavor\" content=\"red\"/>\n";

static char * h_param =
"<param name=\"Fish-Type\" content=\"haddock\"/>\n"
"<param name=\"Cheese-Age\" content=\"mouldy,green\"/>\n"
"<param name=\"Moose-Flavor\" content=\"red\"/>\n";

static void
test_print (AnxParams * params, AnxParamStyle style, int correct_len,
	    char * correct_output)
{
  char buf[1024];
  int len;

  len = anx_params_snprint (NULL, 0, params, style);
  if (len <= 0) {
    WARN ("system has non-C99 snprintf");
  } else if (len != correct_len) {
    FAIL ("incorrect print length");
  }

  memset (buf, '\0', 1024);
  len = anx_params_snprint (buf, 1024, params, style);
  if (len != correct_len)
    FAIL ("incorrect print length");
  if (strcmp (buf, correct_output))
    FAIL ("incorrect printing");
}

static void
test_params (AnxParams * params)
{
  AnxParams * m_params = NULL;
  char * v;

  INFO ("  get value");
  v = anx_params_get (params, "Fish-Type");
  if (strcmp (v, "haddock"))
    FAIL ("error storing param values");

  INFO ("  get appended value");
  v = anx_params_get (params, "Cheese-Age");
  if (strcmp (v, "mouldy,green"))
    FAIL ("error appending param values");

  INFO ("  print query");
  test_print (params, ANX_PARAMS_QUERY, 58, h_query);

  INFO ("  print headers");
  test_print (params, ANX_PARAMS_HEADERS, 65, h_header);

  INFO ("  print metatags");
  test_print (params, ANX_PARAMS_METATAGS, 134, h_meta);

  INFO ("  print paramtags");
  test_print (params, ANX_PARAMS_PARAMTAGS, 137, h_param);

  INFO ("  remove value");
  params = anx_params_remove (params, "Cheese-Age");
  v = anx_params_get (params, "Cheese-Age");
  if (v)
    FAIL ("error removing param value");

  INFO ("  replace value");
  params = anx_params_replace (params, "Moose-Flavor", "curry");
  v = anx_params_get (params, "Moose-Flavor");
  if (strcmp (v, "curry"))
    FAIL ("error replacing param values");

  INFO ("  merge params");
  m_params = anx_params_new_parse (m, ANX_PARAMS_QUERY);
  params = anx_params_merge (params, m_params);
  v = anx_params_get (params, "Fish-Type");
  if (strcmp (v, "haddock,bream"))
    FAIL ("error merging params");

  INFO ("  free params");
  anx_params_free (m_params);
  anx_params_free (params);
}

int
main (int argc, char * argv[])
{
  AnxParams * params, * cparams;

  INFO ("Testing query parsing");
  params = anx_params_new_parse (q, ANX_PARAMS_QUERY);
  test_params (params);

  INFO ("Testing header parsing");
  params = anx_params_new_parse (h, ANX_PARAMS_HEADERS);
  test_params (params);

  INFO ("Testing param cloning");
  params = anx_params_new_parse (q, ANX_PARAMS_QUERY);
  cparams = anx_params_clone (params);
  if (!cparams)
    FAIL ("error cloning params");

  INFO ("  freeing original params");
  anx_params_free (params);
  test_params (cparams);

  exit (EXIT_SUCCESS);
}
