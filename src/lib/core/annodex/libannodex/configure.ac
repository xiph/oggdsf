# Process this file with autoconf to produce a configure script.
AC_INIT
AC_CONFIG_SRCDIR([src/libannodex/annodex.c])

AC_PREREQ(2.53)

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(libannodex, 0.5.66)
AM_CONFIG_HEADER(config.h)

SHARED_VERSION_INFO="0:5:0"
SHLIB_VERSION_ARG=""

# Setup libltdl -- this must come before AC_PROG_LIBTOOL
AC_LIBLTDL_CONVENIENCE
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)
AC_LIBTOOL_DLOPEN

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_C_CONST
AC_C_BIGENDIAN

# Checks for doxygen
AC_CHECK_PROG(HAVE_DOXYGEN, doxygen, true, false)
AM_CONDITIONAL(HAVE_DOXYGEN,$HAVE_DOXYGEN)
if test $HAVE_DOXYGEN = "false"; then
             AC_MSG_WARN([*** doxygen not found, docs will not be built])
fi
  
# Check for docbook
AC_CHECK_PROG(HAVE_DOCBOOKTOMAN, docbook-to-man, true, false)
AM_CONDITIONAL(HAVE_DOCBOOKTOMAN,$HAVE_DOCBOOKTOMAN)
AC_CHECK_PROG(HAVE_DOCBOOK2HTML, docbook2html, true, false)
AM_CONDITIONAL(HAVE_DOCBOOK2HTML,$HAVE_DOCBOOK2HTML)

dnl Overall configuration success flag
anx_config_ok=yes

anx_importers="annodex speex vorbis theora"
anx_read_programs=""
anx_write_programs=""

dnl
dnl  Configuration option for building of experimental code.
dnl

ac_enable_experimental=no
AC_ARG_ENABLE(experimental,
     [  --enable-experimental   enable building of experimental code ],
     [ ac_enable_experimental=yes ])

if test "x${ac_enable_experimental}" = xyes ; then
    AC_DEFINE(ANX_CONFIG_EXPERIMENTAL, [], [Define to build experimental code])
fi

dnl
dnl  Configuration option for building of reading support.
dnl

ac_enable_read=yes
AC_ARG_ENABLE(read,
     [  --disable-read          disable building of reading support ],
     [ ac_enable_read=no ], [ ac_enable_read=yes] )

if test "x${ac_enable_read}" = xyes ; then
    AC_DEFINE(ANX_CONFIG_READ, [1], [Build reading support])
    anx_read_programs="anxinfo anxrip anxcheck"
else
    AC_DEFINE(ANX_CONFIG_READ, [0], [Do not build reading support])
fi
AM_CONDITIONAL(ANX_CONFIG_READ, test "x${ac_enable_read}" = xyes)

dnl
dnl  Configuration option for building of writing support.
dnl

ac_enable_write=yes
AC_ARG_ENABLE(write,
     [  --disable-write         disable building of writing support ],
     [ ac_enable_write=no ], [ ac_enable_write=yes] )

if test "x${ac_enable_write}" = xyes ; then
    AC_DEFINE(ANX_CONFIG_WRITE, [1], [Build writing support])
    anx_write_programs="anxenc anxed"
else
    AC_DEFINE(ANX_CONFIG_WRITE, [0], [Do not build writing support])
fi
AM_CONDITIONAL(ANX_CONFIG_WRITE, test "x${ac_enable_write}" = xyes)

dnl
dnl  Check read/write option sanity
dnl 
if test "x${ac_enable_read}" = xno && test "x${ac_enable_write}" = xno ; then
  AC_MSG_ERROR([
***
*** You have attempted to explicitly disable both writing and reading.
*** 
])
fi


dnl Check for types

AC_MSG_CHECKING(for int64_t)
AC_CACHE_VAL(has_int64_t,
[AC_TRY_RUN([
#ifdef __BEOS__
#include <inttypes.h>
#endif
#include <sys/types.h>
int64_t foo;
int main() {return 0;}
],
has_int64_t=yes,
has_int64_t=no,
has_int64_t=no
)])
AC_MSG_RESULT($has_int64_t)

AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

if test x$has_int64_t = "xyes" ; then
        SIZE64="int64_t"
else
case 8 in
        $ac_cv_sizeof_int) SIZE64="int";;
        $ac_cv_sizeof_long) SIZE64="long";;
        $ac_cv_sizeof_long_long) SIZE64="long long";;
esac
fi

if test -z "$SIZE64"; then
        AC_MSG_WARN(No 64 bit type found on this platform!)
fi

AC_SUBST(SIZE64)


# Configures libltdl
AC_CONFIG_SUBDIRS(libltdl)

dnl
dnl  Detect oggz
dnl

HAVE_OGGZ="no"
PKG_CHECK_MODULES(OGGZ, oggz >= 0.8.0,
                  HAVE_OGGZ="yes", anx_config_ok="no")

if test "x$HAVE_OGGZ" = "xyes" ; then
  AC_DEFINE(HAVE_OGGZ, [], [Define if have liboggz])
  AC_SUBST(OGGZ_LIBS)
else
  AC_CHECK_LIB(oggz, oggz_open, OGGZ_LIBS="maybe")
  if test "x$OGGZ_LIBS" = "xmaybe" ; then  
    AC_MSG_ERROR([
***
*** liboggz-0.5.40 or greater seems to exist on your system, however
*** the pkg-config tool cannot find its build information.
***
*** Please set your PKG_CONFIG_PATH environment variable to include the
*** directory in which oggz.pc was installed. For example, oggz.pc
*** is installed in /usr/local/lib/pkgconfig by default; if that is the
*** case do:
***
***     export PKG_CONFIG_PATH="\$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig"
***
*** then run this ./configure again. If you configured liboggz with a
*** different --prefix option, replace /usr/local above with that prefix.
***
])
  fi
fi
AM_CONDITIONAL(HAVE_OGGZ, [test "x$HAVE_OGGZ" = "xyes"])

#
# Detect libcmml
#

HAVE_LIBCMML=no

PKG_CHECK_MODULES(CMML, cmml >= 0.7.9998,
                  HAVE_LIBCMML="yes", HAVE_LIBCMML="no")

if test "x$HAVE_LIBCMML" = "xyes" ; then
  AC_DEFINE(HAVE_LIBCMML, [], [Define if have libcmml])
  AC_SUBST(CMML_CFLAGS)
  AC_SUBST(CMML_LIBS)
  anx_importers="$anx_importers cmml"
else
  AC_CHECK_LIB(cmml, cmml_open, HAVE_LIBCMML="maybe")
  if test "x$HAVE_LIBCMML" = "xmaybe" ; then
    AC_MSG_ERROR([
***
*** libcmml-0.7.9998 or greater seems to exist on your system, however
*** the pkg-config tool cannot find its build information.
***
*** Please set your PKG_CONFIG_PATH environment variable to include the
*** directory in which cmml.pc was installed. For example, cmml.pc
*** is installed in /usr/local/lib/pkgconfig by default; if that is the
*** case do:
***
***     export PKG_CONFIG_PATH="\$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig"
***
*** then run this ./configure again. If you configured libcmml with a
*** different --prefix option, replace /usr/local above with that prefix.
***
])

  fi
fi
AM_CONDITIONAL(HAVE_LIBCMML, [test "x$HAVE_LIBCMML" = "xyes"])

dnl
dnl  Detect libsndfile 1.0
dnl

PKG_CHECK_MODULES(SNDFILE, sndfile >= 1.0.0,
                  HAVE_LIBSNDFILE1="yes", HAVE_LIBSNDFILE1="no")

if test "x$HAVE_LIBSNDFILE1" = "xyes" ; then
  AC_DEFINE(HAVE_LIBSNDFILE1, [], [Define if have libsndfile1])
  AC_SUBST(SNDFILE_CFLAGS)
  AC_SUBST(SNDFILE_LIBS)
  anx_importers="$anx_importers sndfile"
else
  AC_CHECK_LIB(sndfile, sf_open, HAVE_LIBSNDFILE1="maybe")
  if test "x$HAVE_LIBSNDFILE1" = "xmaybe" ; then  
    AC_MSG_ERROR([
***
*** libsndfile-1.0.0 or greater seems to exist on your system, however
*** the pkg-config tool cannot find its build information.
***
*** Please set your PKG_CONFIG_PATH environment variable to include the
*** directory in which sndfile.pc was installed. For example, sndfile.pc
*** is installed in /usr/local/lib/pkgconfig by default; if that is the
*** case do:
***
***     export PKG_CONFIG_PATH="\$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig"
***
*** then run this ./configure again. If you configured libsndfile with a
*** different --prefix option, replace /usr/local above with that prefix.
***
])
  fi
fi
AM_CONDITIONAL(HAVE_LIBSNDFILE1, [test "x$HAVE_LIBSNDFILE1" = "xyes"])


dnl
dnl Set IMPORTERS_DIR
dnl

IMPORTERS_DIR="${libdir}/annodex/importers"
AC_SUBST(IMPORTERS_DIR)
if test "x${prefix}" = xNONE; then
  importers_dir="${ac_default_prefix}/lib/annodex/importers"
else
  importers_dir="${prefix}/lib/annodex/importers"
fi
AC_DEFINE_UNQUOTED(IMPORTERS_DIR, "$importers_dir")


dnl
dnl  Configuration tests complete -- provide summary of results.
dnl

if test $anx_config_ok = no ; then

AC_MSG_RESULT([
**************************************************************

*** $PACKAGE $VERSION: Automatic configuration FAILED.

*** The file config.log has full details.

*** The following required libraries are missing or
*** misconfigured on your system:
])

if test "x$HAVE_OGGZ" != xyes ; then
    AC_MSG_RESULT(
[*** liboggz, available from  http://www.annodex.net/software/oggz/])
fi
if test "x$HAVE_OGGZ" = xmaybe ; then
    AC_MSG_RESULT(
[      Development files missing: The liboggz library binary seems to be
      installed, but building of a test program failed.
])
fi

AC_MSG_RESULT(
[*** If you install the required libraries from source, you
*** need to inform the dynamic linker of their location. If
*** you install them in a system-wide directory such as
*** /usr/local (the default), you must ensure that
*** /usr/local/lib is listed in /etc/ld.so.conf, then run
*** ldconfig to update the dynamic linking system.
*** Alternatively, you can set your LD_LIBRARY_PATH environment
*** variable to include the library installation directory.
])


AC_MSG_RESULT(
[**************************************************************
])


AC_MSG_ERROR([
***
*** After fixing the above problems, you must run ./configure again.
***
]) 

else

AC_SUBST(SHLIB_VERSION_ARG)
AC_SUBST(SHARED_VERSION_INFO)

AC_OUTPUT([
Makefile
doc/Makefile
doc/Doxyfile
include/Makefile
include/annodex/Makefile
include/annodex/anx_int64.h
src/Makefile
src/libannodex/Version_script
src/libannodex/Makefile
src/importers/Makefile
src/tools/Makefile
src/tests/Makefile
src/examples/Makefile
annodex.pc
])

AC_MSG_RESULT([
------------------------------------------------------------------------
  $PACKAGE $VERSION:  Automatic configuration OK.

  General configuration:

    Experimental code: ........... ${ac_enable_experimental}
    Reading support: ............. ${ac_enable_read}
    Writing support: ............. ${ac_enable_write}

  Importers:

    ${anx_importers}

  Tools:

    ${anx_read_programs} ${anx_write_programs}

  Installation paths:

    libannodex: .................. ${prefix}/lib
    Importers: ................... ${importers_dir}
    C header files: .............. ${prefix}/include/$PACKAGE
    Documentation: ............... ${prefix}/share/doc/$PACKAGE

  Example programs will be built but not installed.
------------------------------------------------------------------------
])


fi
