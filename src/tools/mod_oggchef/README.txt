mod_timeslice
=============

mod_timeslice is an Apache 2 module which enables server-side seeking into
media files via timed URIs.  Currently, if a media player client wishes to
start playback of a multimedia file from an HTTP server at 15 seconds
(i.e. seek to 15 seconds), it has two choices.  It can:

    1.  Download the media file from the beginning, and simply wait until
	15 seconds of data have been received.  This is what many media
	players do.

    2.  Perform a time to byte mapping if the media file format enables
	this, and issue an HTTP/1.1 Range: request, so that the web server
	starts serving the data at the byte specified.  This has the
	disadvantage of being imprecise, and for accurate seeking, the
	client generally has to perform a bisection search (such as
	a binary search) over the network.

With mod_timeslice, the client can ask the web server to play back
a resource at a particular time point by using a timed URI.  As an
example, to play back the media file http://example.com/foo.anx starting
at 15 seconds, the client simply appends the timed URI query '?t=15', e.g.

    http://www.example.com/foo.anx?t=15

The web server is then responsible for delivering the data to the client
at 15 seconds.  For more information on timed URIs, including various
different time scheme formats, see
<http://www.annodex.net/specifications.html>.

Note that mod_timeslice is effectively a re-implementation of the timed
URI capability of mod_annodex <http://www.annodex.net/installation.html>.
mod_annodex is far more powerful, being able to dynamically generate
Annodex media from CMML files, and serve out only the CMML portion of
Annodex media.  These features will be added in the future, though
possibly not to mod_timeslice itself.


What Works
----------

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

Note that this module has been tested to work on Linux systems, but
I haven't packaged the build system for it yet (though it's a standard
apxs2 module build).


Building
--------

Windows:

* Windows developers can simply open up the mod_timeslice.vcproj Visual
  Studio project file and build that.  Note that the project file is
  pre-configured to use the standard Apache 2 installation path of
  C:\Program Files\Apache Group\Apache2, and of course requires the Apache
  2 development libraries and headers to be installed.  If your Apache
  2 installation path differs, edit the project properties and adjust them
  accordingly.  (If you have ideas on how to make this easier for
  non-standard installations, let me know.)

UNIX:

* We use a simple apxs-based build system, which is tested on
  Debian GNU/Linux.  You may need to modify some Makefile variables to get
  things working: see the first few lines of the Makefile to see what
  variables to tweak.


Installing
----------

* Add the "application/x-annodex" MIME type to your Apache's mime.types
  file, for file extensions anx, axa and axv.

* Edit your httpd.conf file to enable mod_timeslice by inserting the
  following line:

  LoadModule timeslice_module /path/to/mod_timeslice.so

Windows:

* Copy mod_timeslice.so (and mod_timeslice.pdb, if you want to use the
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

