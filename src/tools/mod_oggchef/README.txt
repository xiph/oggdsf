mod_oggchef
===========

mod_oggchef is an Apache 2 module which enables the web server to
dynamically recompose media, according to the client's wishes.  It can:

* Serve out only certain tracks (logical bitstreams) from the host's Ogg
  file, by using the HTTP "Accept:" header field.  For example, an HTTP
  request with the header "Accept: video/x-theora" can be used to serve
  out only the video track, with no audio track.

* Serve out the media with a specific time interval via timed URIs.  For
  example, a web client can request playback of the media beginning at 30
  minutes into a movie.

Note that mod_oggchef is effectively a re-implementation of mod_annodex
<http://www.annodex.net/installation.html>, using illiminable's C++ Ogg
framework.  mod_annodex is full more featureful, however, being able to
dynamically generate Annodex media on-the-fly from CMML files, which
mod_oggchef doesn't do yet.  We hope to add these features to mod_oggchef
in the future.


Extracting Tracks
-----------------

Track extraction is performed via the HTTP "Accept:" header.  mod_oggchef
maintains a map of MIME types to logical bitstream formats, as follows:

* application/x-annodex: Annodex-encapsulated file (i.e. serve out the
  entire media file)
* application/ogg & application/x-ogg: Ogg-encapsulated file (i.e. serve
  out the entire media file).  Note that if the host file is an Annodex
  file, the Annodex headers will be stripped off, so a standard Ogg file
  is delivered.
* audio/x-speex: the Speex audio codec encapsulated inside an Ogg
  stream.
* audio/x-vorbis: the Vorbis audio codec encapsulated inside an Ogg
  stream.
* text/x-cmml: CMML metadata.
* video/x-theora: the Theora video codec encapsulated inside an Ogg
  stream.


Timed URIs (server-side seeking)
--------------------------------

NOTE: This is currently broken, so currently, requested timed URIs won't
work (it'll just be served out from the starting time, always).  I'll fix
it up over the next day or two.

Currently, if a media player client wishes to start playback of
a multimedia file from an HTTP server at 15 seconds (i.e. seek to 15
seconds), it has two choices.  It can:

    1.  Download the media file from the beginning, and simply wait until
	15 seconds of data have been received.  This is what many media
	players do.

    2.  Perform a time to byte mapping if the media file format enables
	this, and issue an HTTP/1.1 Range: request, so that the web server
	starts serving the data at the byte specified.  This has the
	disadvantage of being imprecise, and for accurate seeking, the
	client generally has to perform a bisection search (such as
	a binary search) over the network.

With mod_oggchef, the client can ask the web server to play back
a resource at a particular time point by using a timed URI.  As an
example, to play back the media file http://example.com/foo.anx starting
at 15 seconds, the client simply appends the timed URI query '?t=15', e.g.

    http://www.example.com/foo.anx?t=15

The web server is then responsible for delivering the data to the client
at 15 seconds.  For more information on timed URIs, including various
different time scheme formats, see
<http://www.annodex.net/specifications.html>.



Restrictions
------------

* Currently, only Annodex media files are supported.  Support for Ogg
  files will be forthcoming.


Requirements
------------

Windows:

* The official Apache 2 Win32 binary provided as an MSI installer,
  available at <http://httpd.apache.org/download.cgi>.  Apache 2.0.52 is
  used for development.

UNIX:

* An Apache 2 installation of some sort with the Apache 2
  development tools (such as apxs).


Building
--------

Windows:

* Windows developers can simply open up the mod_oggchef.vcproj Visual
  Studio project file and build that.  Note that the project file is
  pre-configured to use the standard Apache 2 installation path of
  C:\Program Files\Apache Group\Apache2, and of course requires the Apache
  2 development libraries and headers to be installed.  If your Apache
  2 installation path differs, edit the project properties and adjust them
  accordingly.  (If you have ideas on how to make this easier for
  non-standard installations, let me know.)

UNIX:

* We use a simple apxs-based build system, which has been tested on
  Debian GNU/Linux.  You may need to modify some Makefile variables to get
  things working: see the first few lines of the Makefile to see what
  variables to tweak.


Installing
----------

* Add the "application/x-annodex" MIME type to your Apache's mime.types
  file, for file extensions anx, axa and axv.

* Edit your httpd.conf file to enable mod_oggchef by inserting the
  following line:

  LoadModule oggchef_module /path/to/mod_oggchef.so

Windows:

* Copy mod_oggchef.so (and mod_oggchef.pdb, if you want to use the
  Visual Studio Debugger with Apache) to Apache's module directory,
  usually at C:\Program Files\Apache Group\Apache2\modules.

UNIX:

* Just run 'make install'.  You may need adminstrator privileges (i.e.
  sudo or su)


Copyright
---------

Copyright (C) 2004-2005, Zentaro Kavanagh
Copyright (C) 2004-2005, Commonwealth Scientific and Industrial Research
                         Organisation (CSIRO) Australia

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

    * Neither the name of Zentaro Kavanagh, CSIRO Australia nor the names
of its contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

