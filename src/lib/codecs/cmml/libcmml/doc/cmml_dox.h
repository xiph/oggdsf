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

/** 
 * \mainpage
 *
 * \section intro Introduction
 *
 * <b>libcmml</b> is a library that provides a complete programming
 * interface including functions, data structures, and sloppy or
 * strict error handling to parse a XML file in CMML. CMML is the
 * Continuous Media Markup Language defined as part of the Continuous
 * Media Web project (see <a
 * href="http://www.annodex.net/software/libcmml">
 * http://www.annodex.net/software/libcmml/</a>).
 *
 * libcmml also includes the following command-line tools:
 * - <a href="cmml-validate_8c.html">cmml-validate</a>, which takes as
 *   input a CMML file and tests it against the <a
 *   href="cmml_8dtd-example.html">cmml.dtd</a>
 * - <a href="cmml-fix_8c.html">cmml-fix</a>, which fixes a sloppily
 *   written input CMML and creates a valid one if possible
 * - <a href="cmml-fortune_8c.html">cmml-fortune</a>, which creates a
 *   valid CMML file with random content
 *
 *
 * \section features Features of CMML
 *
 * The version of CMML that this version of libcmml supports is CMML
 * 2.0 . It has the following features:
 *
 * - html-like markup language for audio, video, and other
 *   time-continuous data files (call them "media files")
 * - provides markup to structure an input media file into clips by
 *   identification of time intervals
 * - URI hyperlinking to clips is possible
 * - provides structured annotations (meta tags) and unstructed
 *   annotations (free text) both for the complete media file and each
 *   clips
 * - URI hyperlinks from clips to other Web resources possible
 * - URI hyperlinks from clips to representative images (keyframes)
 *   possible
 * - internationalisation (i18n) support for markup
 * - multi-track composition directions for media files from several
 *   input media files possible
 * - several tracks of annotations (multi-track annotations) possible
 * - arbitrarily high temporal resolution for annotation and media
 *   tracks
 * - non-zero timbase association with media files possible
 * - wall-clock time association with media files possible
 *
 * To learn more about CMML and how to write a CMML file, check out
 * the <a href="modules.html">Modules</a> documentation.
 *
 *
 * \section install Compiling and Installing libcmml
 *
 * libcmml is developed in C under Linux and OS X but is easily
 * portable. A Visual C++ MS Windows port is included in the tarball.
 *
 * libcmml uses expat for parsing XML. Get the appropriate version for
 * your OS from <a
 * href="http://expat.sourceforge.net/">http://expat.sourceforge.net/</a>.
 *
 * libcmml uses <a href="http://www.doxygen.org/">doxygen</a> to
 * create documentation and <a
 * href="http://www.docgook.org/">docbook</a> to create manpages.
 *
 * To install on Linux, OS X, and other Unix-like OSs run the usual
 * configure, make, make install sequence. Full details in the INSTALL
 * file.
 *
 * To install on MS Windows you have the choice between a MS Visual
 * Studio version 6 workspace file, a MS Visual Studio version 7
 * solution, and using nmake with the Win32/Makefile file. Full
 * details in the README.win32 file.
 *
 *
 * \section begin Programming with libcmml
 *
 * The libcmml <b>API documentation</b> can be found <a
 * href="globals_func.html">here</a>. The API of libcmml is based on a
 * convenient callback based framework for parsing CMML files. This
 * enables activation of actions for the stream, head and clip tags
 * while reading a CMML file. After opening a CMML file for reading,
 * you attach callbacks to each of these types of tags. Then, as bytes
 * are read, libcmml will call your callbacks as appropriate. Check
 * out the <a href="examples.html">code examples</a> to gain a better
 * understanding.
 *
 *
 * \section dtd The CMML DTD
 *
 * The DTD for CMML can be found here: <a href="cmml_8dtd-example.html">here</a>.
 *
 *
 * \section license Licensing
 * libcmml is provided under the following BSD-style open source license:
 *
 * \include COPYING
 *
 * \section ack Acknowledgements
 *
 * \include AUTHORS
 */

/*------------------------- Examples ----------------------------*/

/**
 * \example cmml.dtd
 *
 * The DTD for CMML is quite short and produces flat hierarchies. It
 * follows an intention to create the minimal necessary markup and
 * although tags were copied from HTML, attributes and elements were
 * selected carefully. The inclusion of further tags is possible, but
 * only when there is a proven need for it. Incompatible changes will
 * result in a change of the major version number of CMML, while
 * compatible changes, such as optional extensions, only change the
 * minor version number.
 *
 * To get a better understanding of the components in libcmml it is
 * recommended to read the DTD:
 *
 */

/**
 * \example cmml-parse.c
 * <b>Reading from a CMML file</b>
 *
 * libcmml provides a convenient callback based framework for reading
 * CMML files. After opening a CMML file for reading, you can attach
 * various callbacks relevant to the parts of the file you are
 * interested in, including the stream element, head element, and clip
 * elements. Then, as bytes are read, libcmml will call your callbacks
 * as appropriate.
 *
 * The functions that need to be called and the respective data
 * structures are defined in cmml.h.
 *
 * The general sequence of API calls is the following:
 *
 * - open a CMML object using cmml_open() on a local file or
 * cmml_new() with a FILE pointer to an already opened file.
 * - attach read callbacks using cmml_set_read_callbacks()
 * - call cmml_read() repeatedly until it returns 0 or -1.
 * - close the CMML object with cmml_destroy() returning the FILE 
 * pointer or use cmml_close() to close both the object and the open 
 * file.
 *
 * This procedure is illustrated in cmml-parse.c, which prints the
 * title attribute of a CMML tag :
 *
 */

/**
 * \example cmml-write.c
 * <b>Writing a CMML file</b>
 *
 * Although libcmml was created mainly for the purpose of parsing
 * existing CMML files, it also provides rudimentary support for
 * creating CMML files. This basically consists in providing the
 * data structures for the different tags and API functions to
 * print those tags. There is no validation or sequence checking
 * available for the writing side.
 *
 * The general sequence of API calls is the following:
 *
 * - create a CMML_Preamble with cmml_preamble_new() and print it
 * with cmml_preamble_snprint()
 * - create a CMML_Head with cmml_head_new(), change the attribute and
 * element contents in the CMML_Head structure, and print it with
 * cmml_head_snprint() or cmml_head_pretty_snprint()
 * - create one or more CMML_Clip with cmml_clip_new(), change the attribute and
 * element contents in the CMML_Clip structures, and print them with
 * cmml_clip_snprint() or cmml_clip_pretty_snprint()
 * - print the final "</cmml>" tag
 *
 * This procedure is illustrated in cmml-write.c:
 *
 */

/**
 * \example cmml-seek-clip.c
 * <b>Seeking to an clip in a CMML file</b>
 *
 * Sometimes you don't want to access all the clips available in a
 * CMML file, but only a specific one. libcmml provides an API for
 * this functionality through the cmml_skip_to_id() function.
 *
 * The procedure is illustrated in cmml-seek-clip.c, which seeks to
 * a clip of a given name and if found prints out the descriptions
 * of this and all the following clips:
 *
 */

/**
 * \example cmml-seek-offset.c 
 * <b>Seeking to a temporal offset into a CMML file</b>
 *
 * Sometimes you'll need to seek to a temporal offset in seconds into
 * a CMML file. Note: The seconds offset is calculated with respect to
 * the timebase attribute of the stream tag. libcmml provides an API
 * for this functionality through the cmml_skip_to_secs() function.
 *
 * The procedure is illustrated in cmml-seek-offset.c, which seeks to
 * an offset given in seconds and prints out the descriptions of all
 * the following clips:
 *
 */

/**
 * \example cmml-seek-utc.c 
 * <b>Seeking to a utc time offset into a CMML file</b>
 *
 * Sometimes you'll need to seek to a temporal offset in utc into a
 * CMML file. Note: The utc offset is calculated with respect to the
 * utc and timebase attributes of the stream tag. libcmml provides an
 * API for this functionality through the cmml_skip_to_utc()
 * function.
 *
 * The procedure is illustrated in cmml-seek-utc.c, which seeks to an
 * offset given in utc and prints out the descriptions of all the
 * following clips:
 *
 */

/**
 * \example cmml-uri-file.c 
 * <b>Parse a CMML file given through a file uri (optionally with a
 * fragment offset)</b>
 *
 * This example program demonstrates how a CMML file that is given
 * through a file uri and optionally contains a fragment offset can be
 * interpreted. The example can be extended to other schemes such as
 * http and to cover uri queries, too.
 *
 * The procedure is illustrated in cmml-uri-file.c, which opens a
 * file given through a file uri, and optionally seeks to an offset
 * given the uri fragment specifier. It then prints out the
 * descriptions of all the following clips:
 *
 */

/**
 * \example cmml-validate.c
 *
 * This is the full source code of the cmml-validate program, which
 * parses a CMML instance document and validates it against the
 * cmml.dtd returning true/false. In case of an error
 * the faulty tag including line and col number is reported.
 * It also spits out warnings for strange stuff.
 */


/*------------------------- CMML intro ----------------------------*/

/** 
 * \defgroup intro Writing CMML files
 *
 * The following simple example CMML file demonstrates the main
 * features of CMML:
 *
 * \include simple.cmml
 *
 * \section preamble The preamble
 *
 * After the usual preamble including the xml processing instruction
 * and the doctype declaration, the root <b>cmml tag</b> houses the
 * markup.
 *
 * \section stream The stream tag
 *
 * The first markup will usually be a <b>stream</b> tag which names
 * the source media file(s) that the markup of the CMML file relates
 * to. The stream tag is optional as you may want to prepare a CMML
 * file for a not yet existing media file, for a live media stream, or
 * just as a template.
 *
 * \section head The head tag
 *
 * The <b>head</b> tag is mandatory as with html. It will at least
 * contain either a title or a base tag. The <b>title</b> tag contains
 * a short description of the complete annotated work. More structured
 * information for the work go into the <b>meta</b> tags. It is
 * recommended to use existing meta schemes such as the <a
 * href="http://www.dublincore.org/">Dublin Core</a> for the
 * structured annotations.
 *
 * \section clip The clip tag
 *
 * The following <b>clip</b> tags structure the work into temporal
 * sections by specification of start and end times for each
 * section. The end time is optional as a clip will be implicitly
 * ended with the start of the next clip or the end of the file. You
 * may attach an <b>a</b> tag to a clip to specify a related Web
 * resource through its href attribute. You may attach an <b>img</b>
 * tag to a clip to specify a URL to an image that represents the
 * content of the clip visually. You may attach a <b>desc</b> tag to a
 * clip to provide a textual description. More structured information
 * for the clip go into the <b>meta</b> tags.
 *
 * \section end Finishing the CMML file
 *
 * The closing of the cmml tag ends the CMML file.
 *
 */

/**
 * \defgroup time Concepts of time in CMML
 *
 * Using time specifications in CMML is powerful but somewhat
 * confusing. So, we describe it here in more detail. CMML has
 * specifications of time in the stream and the clip tag.  There are
 * two different concepts of time specifications possible in CMML: utc
 * time and playback time.
 *
 * \section stream The stream tag
 *
 * The stream tag contains specifications of the INPUT media
 * document(s) in the media tags. Conversely, the attributes of the
 * stream tag refer to the OUTPUT annodex media document for which
 * this CMML file provides the markup.
 *
 * \subsection tb Timebase attribute 
 *
 * The timebase attribute maps the first frame of the OUTPUT annodex
 * document to a playback time to be displayed with the first
 * frame. This playback time can be given as npt or some smpte
 * specification given as a name-value pair, and it defaults to 0.
 *
 * \subsection utc Utc attribute
 *
 * The optional utc attribute maps the timebase to a real-world clock
 * time. It is given as a utc value.
 *
 * \subsection start Start attribute of source tags 
 *
 * The start attribute specifies the insertion time of the specified
 * INPUT document into the Annodex media file, i.e. the absolute time
 * at which this media document (offset by seekoffset time) will start
 * within the OUTPUT Annodexed file. It will usually be given as a npt
 * or smpte time, which has to be greater than the timebase given in
 * the stream tag (or just greater than 0 if there is not timebase
 * given).  The value is given as a name-value pair separated by a
 * colon ":" though the name can be omitted if using npt. The start
 * attribute can also be given as a plain utc time, which has to be
 * some time after the utc time given in the stream tag. utc
 * specification is not possible when the utc attribute in the stream
 * tag is not given. The default value for the start attribute is
 * "npt:0".
 *
 * \subsection end End attribute of source tags
 *
 * The end attribute specifies when the inserted INPUT media document
 * will end. The time specification is analogous to the start
 * attribute. This attribute is optional as the default end of the
 * INPUT media document is when it finishes.
 *
 * \subsection loc Location attribute of source tags
 *
 * As the location is specified as a URI, it may contain temporal
 * fragment offsets. These specify a temporal subpart of the INPUT
 * media document which will be inserted into the Annodexed media
 * stream.
 *
 * \section clip The clip tag
 *
 * The clip tag contains specifications of insertion time of the clip
 * into the Annodexed media file.
 *
 * \subsection clipstart Start and end attributes
 *
 * Insertion start and end times are given in the start and end
 * attributes and they are specified in the same way as the start
 * and end attributes of the source tag.
 *
 */

/** 
 * \defgroup i18n Internationalisation support in CMML
 *
 * CMML is designed to provide full internationalisation (i18n)
 * support, covering different character sets as well as languages and
 * their differing directionality. Any tag or attribute that could end
 * up containing text in a different language to the other tags may
 * specify their own language.
 *
 * As a CMML file is an XML file by definition, the xml processing
 * instruction provides for a file-specific specification of the
 * encoding format in its "encoding" tag. A potentially differing
 * character set for an INPUT media file will be specified in the
 * "contenttype" attribute of the <b>source</b> tag as a parameter to
 * the MIME type.
 *
 * Different languages and their directionality for display purposes
 * are given for every tag that contains human-readable text as a
 * value of either an attribute or a tag. They are specified in the
 * <b>lang</b> and <b>dir</b> attributes.

 * The root <b>cmml</b> tag's "lang" and "dir" attributes provide the
 * default language for the whole CMML file.
 *
 * The <b>source</b> tag in the stream tag has i18n support as its
 * "title" attribute may contain human readable text. 
 *
 * The <b>head</b> tag has i18n support to provide a default language
 * to its contained tags such as the <b>title</b> and <b>meta</b>
 * tags. Both of them may incidentally overrun the defaults by having
 * a language specification of their own.
 *
 * The <b>clip</b> tag again contains a default language for the clip
 * itself. The <b>a</b>, <b>img</b>, <b>desc</b>, and <b>meta</b> tags
 * may each override the default language.
 *
 * Here is an example of a multi-lingual CMML file:
 *
 * \include i18n.cmml
 *
 * The document is generally in Polish (see lang attribute of cmml
 * tag). The lang atttribute of the head tag makes all head tags and
 * attributes be English. The lang attribute of the first clip tag
 * makes all of them be German, though this is overrun by the lang
 * attribute of the desc tag, making that one English. The last clip
 * tag is in English, but it has a meta tag in French.
 *
 */

/** 
 * \defgroup multitracks Multitrack input media in CMML
 *
 * With CMML it is possible to create markup for several, potentially
 * interleaved, media files. This is specified in the <b>stream</b>
 * tag by giving several <b>source</b> tags which each point to a
 * media file. The "start" tag specifies at what time the media file
 * should be inserted. In case of temporally overlapping media files
 * they are regarded as interleaved files.
 *
 * This is an example of such a specification:
\verbatim
<stream timebase="npt:0">
  <source contenttype="video/x-theora" location="fish.ogg" start="npt:0"/>
  <source contenttype="audio/x-vorbis" location="fishsound.ogg" start="npt:0"/>
  <source contenttype="audio/x-speex" location="comments.spx" start="npt:7"/>
</stream>
\endverbatim
 *
 */

/** 
 * \defgroup multicliptrack Multitrack annotations in CMML
 *
 * The default case for CMML is to have one annotation track with
 * temporally non-overlapping clips of annotations. This is the case
 * when the "track" attribute of the <b>clip</b> tag is not used.
 *
 * It is however possible to specify several tracks of annotations
 * within one CMML file. To that end, you need to give each track that
 * you're using a name. This name needs to be written into the "track"
 * attribute of the <b>clip</b> tags which belong onto that track. Not
 * providing a track attribute attaches the clip to the default
 * track.
 *
 * Take care that the clips in one annotation track do not overlap in
 * time.
 *
 * This is an example of such a multi-track annotation specification:
\verbatim
<clip track="german" lang="de" id="intro_ge" start="npt:0">
  <a href="http://www.blah.au/fish.html">Lesen Sie mehr &#252;ber Fische.</a>
  <desc>Dies ist die Einleitung zum Film &#252;ber Fische von Joe.</desc>
</clip>

<clip track="default" lang="en" id="intro" start="npt:0">
  <a href="http://www.blah.au/fish.html">Read more about fish.</a>
  <desc>This is the introduction to the film Joe made about fish.</desc>
</clip>

<clip track="german" id="dolphin_ge" start="npt:3.5">
  <desc>Joe hat einen Delphin im Meer entdeckt.</desc>
  <meta name="Thema" content="Delphin"/>
</clip>

<clip id="dolphin" start="npt:3.5">
  <desc>Here, Joe caught sight of a dolphin in the ocean.</desc>
  <meta name="Subject" content="dolphin"/>
</clip>
\endverbatim
 *
 */

