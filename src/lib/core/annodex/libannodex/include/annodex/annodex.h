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

#ifndef __ANNODEX_H__
#define __ANNODEX_H__

/** \file
 *
 * This header is provided for convenience and includes all files required
 * for reading or writing Annodex media.
 */

#include <stdlib.h>

#include <annodex/anx_constants.h>
#include <annodex/anx_types.h>
#include <annodex/anx_core.h>
#include <annodex/anx_list.h>
#include <annodex/anx_params.h>
#include <annodex/anx_general.h>
#include <annodex/anx_track.h>
#include <annodex/anx_read.h>
#include <annodex/anx_write.h>

/** \mainpage
 *
 * \section intro Introduction
 *
 * This is the documentation for the libannodex C API. libannodex provides
 * a complete programming interface for reading and writing Annodex media.
 * Annodex is a free, open and unpatented format for annotating and
 * indexing media and other forms of continuous data, and can be used
 * on the World Wide Web for "video surfing". For more information about
 * Annodex, see <a href="http://www.annodex.net/">Annodex.net</a>.
 *
 * \subsection Contents
 *
 * - \link basics Basics \endlink:
 * Information about Annodex format required to understand libannodex
 *
 * - \link reading Reading \endlink:
 * Reading from Annodex media
 *
 * - \link writing Writing \endlink:
 * Writing Annodex media
 *
 * - \link configuration Configuration \endlink:
 * Customizing libannodex to only read or write.
 *
 * - \link building Building \endlink:
 * Information related to building software that uses libannodex.
 *
 * \section Licensing
 *
 * libannodex is provided under the following BSD-style open source license:
 *
 * \include COPYING
 *
 */

/** \defgroup basics Annodex format basics
 *
 *
 * \section Terminology
 *
 * libannodex introduces terminology directly related to Annodex format,
 * and also borrows some terminology from the underlying Ogg framework.
 * Ogg is a non-heirarchical container format developed by Monty at Xiph.Org,
 * originally for the Ogg Vorbis audio codec.
 * 
 * \subsection multitrack Managing multitrack data
 *
 * Annodex format allows time-synchronous interleaving of multiple data
 * tracks such as audio, video and annotations. Each track is uniquely
 * identified by a serial number or \a serialno in Ogg terminology.
 *
 * - \a serialno: an integer identifying a track
 *
 * A random serial number will be assigned to each track automatically.
 *
 * \subsection time Time: video frames, sampling rates etc.
 *
 * All data contained within Annodex tracks must be timed.
 * Annodex format introduces the generic concept of \a granulerate to
 * describe such things as framerates and sampling rates.
 *
 * - \a granule: a generic unit of time, specified per track
 * - \a granulerate: granules per second
 *
 */

/** \defgroup configuration Configuration
 * \section ./configure ./configure
 *
 * It is possible to customize the functionality of libannodex
 * by using various ./configure flags when
 * building it from source. You can build a smaller
 * version of libannodex to only read or write.
 * By default, both reading and writing support is built.
 *
 * For general information about using ./configure, see the file
 * \link install INSTALL \endlink
 *
 * \subsection no_encode Removing writing support
 *
 * Configuring with \a --disable-write will remove all support for writing:
 * - All internal write related functions will not be built
 * - Any attempt to call anx_new(), anx_open() or anx_open_stdio()
 *   with \a flags == ANX_WRITE will fail, returning NULL
 * - Any attempt to call anx_write(), anx_write_output(), or
 *   anx_write_get_next_page_size() will return ANX_ERR_DISABLED
 *
 * \subsection no_decode Removing reading support
 *
 * Configuring with \a --disable-read will remove all support for reading:
 * - All internal reading related functions will not be built
 * - Any attempt to call anx_new(), anx_open() or anx_open_stdio()
 *    with \a flags == ANX_READ will fail, returning NULL
 * - Any attempt to call anx_read(), anx_read_input(),
 *   anx_set_read_callback(), anx_seek(), or anx_seek_units() will return 
 *   ANX_ERR_DISABLED
 *
 */

/** \defgroup install Installation
 * \section install INSTALL
 *
 * \include INSTALL
 */

/** \defgroup building Building against libannodex
 *
 *
 * \section autoconf Using GNU autoconf
 *
 * If you are using GNU autoconf, you do not need to call pkg-config
 * directly. Use the following macro to determine if libannodex is
 * available:
 *
 <pre>
 PKG_CHECK_MODULES(ANNODEX, annodex >= 0.6.0,
                   HAVE_ANNODEX="yes", HAVE_ANNODEX="no")
 if test "x$HAVE_ANNODEX" = "xyes" ; then
   AC_SUBST(ANNODEX_CFLAGS)
   AC_SUBST(ANNODEX_LIBS)
 fi
 </pre>
 *
 * If libannodex is found, HAVE_ANNODEX will be set to "yes", and
 * the autoconf variables ANNODEX_CFLAGS and ANNODEX_LIBS will
 * be set appropriately.
 *
 * \section pkg-config Determining compiler options with pkg-config
 *
 * If you are not using GNU autoconf in your project, you can use the
 * pkg-config tool directly to determine the correct compiler options.
 *
 <pre>
 ANNODEX_CFLAGS=`pkg-config --cflags annodex`

 ANNODEX_LIBS=`pkg-config --libs annodex`
 </pre>
 *
 */


#endif /* __ANNODEX_H__ */
