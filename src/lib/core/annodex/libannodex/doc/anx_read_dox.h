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

/** \defgroup reading Reading from Annodex media
 *
 * libannodex provides a convenient callback based framework for reading
 * Annodex media. After opening an annodex for reading, you can attach
 * various callbacks relevant to the parts of the file you are interested
 * in, including the stream header, track headers, head element, anchors
 * and media data. Then, as bytes are read, libannodex will call your
 * callbacks as appropriate.
 *
 * - General functions related to opening, closing and seeking are located
 *   in \link anx_general.h anx_general.h \endlink.
 * - Functions directly related to reading are located in
 *   \link anx_read.h anx_read.h \endlink
 *
 * The following modules contain descriptions and example code for various
 * methods of reading Annodex media.
 */

/** \defgroup reading_files Reading from files and file descriptors
 * \ingroup reading
 *
 * If the Annodex media you wish to access is directly available as a
 * local file or via a file descriptor (such as a network socket), it
 * can be directly opened as follows:
 *
 * - open an annodex using anx_open() or anx_openfd()
 * - attach read callbacks using anx_set_read_*_callback()
 * - call anx_read() repeatedly until it returns 0 or -1
 * - close the annodex with anx_close()
 *
 * This procedure is illustrated in src/examples/print-title-file.c:
 *
 * \include print-title-file.c
 */

/** \defgroup reading_memory Reading from memory buffers
 * \ingroup reading
 *
 * Sometimes it is not possible to provide a file descriptor for a data
 * source; for example, if the data is extracted from a high level library.
 * In this case, you must directly input the data to libannodex using
 * memory buffers as follows:
 *
 * - open an annodex using anx_new()
 * - attach read callbacks using anx_set_read_*_callback()
 * - call anx_reader_input() repeatedly until it returns 0 or -1
 * - close the annodex with anx_close()
 *
 * This procedure is illustrated in src/examples/print-title-memory.c:
 *
 * \include print-title-memory.c
 */

/** \defgroup reading_callbacks Advanced management of AnxRead* callbacks
 * \ingroup reading
 *
 * You retain control of the number of bytes read or input, and the
 * callbacks you provide can instruct libannodex to immediately return
 * control back to your application.
 *
 * \section rcb_list Callbacks
 *
 * It is not required to implement all callbacks.
 *
 * \section rcb_retvals Return values
 *
 * - ANX_CONTINUE on success, and to inform anx_read*() functions to
 *   continue on to the next packet
 * - ANX_STOP_OK on success, to inform anx_read*() functions to return
 *   without further processing
 * - ANX_STOP_ERR on error, to inform anx_read*() functions to return
 *   without further processing
 *
 * These mechanisms are illustrated in src/examples/print-lots.c:
 *
 * \include print-lots.c
 *
 * which produces output like:
 <pre>
Welcome to /tmp/alien_song.anx! The timebase is 20.000000
Our first track has mime-type video/x-theora and granule rate 1536/1.
We will remember it by its serial number 1810353996 and mark it with crosses.
+...+++++++++++++++++...................................++++++++++++++..........
......................++++++++++++..............................++++++++++++++++
++++..............................+++++++++++++.................................
...++++++++++++++++++++++................................++++++++++++++.........
........................++++++++++++++++....................................++++
++++++++++.......................................++++++++++++++.................
......+++++++++++++.......................+++++++++++++.........................
...+++++++++++++..................................+++++++++++++.................
......+++++++++++++...............................++++++++++++++................
..........+++++++++++..........................++++++++++++++...................
.....++++++++++++.......................++++++++++++........................++++
++++++++........................+++++++++++++........................+++++++++++
+++++.......................+++++++++++.......................+++++++++++++++++.
......................+++++++++++++........................++++++++++...........
.............+++++++++++.......................++++++++++++++++++...............
........++++++++........................+++++++++++++++++.......................
++++++++.......................+++++++++++++++++........................++++++++
++++++...........................+++++++++++........................++++++++++++
+++........................+++++++++++.......................++++++++++++++.....
.................++++++++++++++..............................++++++++++.........
.........................++++++++++++................................+++++++++++
+++...................................++++++
The first anchor links to http://www.mars.int/
+
The second anchor links to http://www.pluto.int/
+++......................+++++++++++.............................+++++++++++....
...........................+++++++++++..........................................
.......+++++++++++++...........................+++++++++++......................
.......+

And the third anchor links to http://www.venus.int/
This completes our whirlwind tour of the first three anchors!

639 packets from the first track (serialno 1810353996) were received
before the third anchor.

 </pre>
 */
