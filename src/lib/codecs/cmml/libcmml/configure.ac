# Process this file with autoconf to produce a configure script.
AC_INIT(configure.ac)
AC_CONFIG_SRCDIR(src/cmml.h)

AC_PREREQ(2.53)
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(libcmml,0.8.2)
AM_CONFIG_HEADER(config.h)

SHARED_VERSION_INFO="0:8:0"
SHLIB_VERSION_ARG=""

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_PROG_LIBTOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_C_CONST
AC_C_BIGENDIAN

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h getopt.h unistd.h math.h])

# Overall configuration success flag
libcmml_config_ok=yes

# check for getopt in a separate library
HAVE_GETOPT=no
AC_CHECK_LIB(getopt, getopt, HAVE_GETOPT="yes")
if test "x$HAVE_GETOPT" = xyes ; then
  GETOPT_LIBS="-lgetopt"
  AC_SUBST(GETOPT_LIBS)
fi

# check for getopt_long in standard library
HAVE_GETOPT_LONG=no
AC_CHECK_FUNC(getopt_long, HAVE_GETOPT_LONG="yes")
if test "x$HAVE_GETOPT_LONG" = xyes ; then
  AC_DEFINE(HAVE_GETOPT_LONG, [], [Define to 1 if you have the 'getop_long' function])
else
  # FreeBSD has a gnugetopt library
  AC_CHECK_LIB(gnugetopt, getopt_long, HAVE_GETOPT_lONG="yes")
  if test "x$HAVE_GETOPT_LONG" = xyes ; then
    AC_DEFINE(HAVE_GETOPT_LONG, [], 
      [Define to 1 if you have the 'getop_long' function])
    GETOPT_LONG_LIBS="-lgnugetopt"
    AC_SUBST(GETOPT_LONG_LIBS)
  fi
fi

# Checks for expat library
HAVE_EXPAT=no
AC_CHECK_LIB(expat, XML_ParserCreate, HAVE_EXPAT="yes")
if test "x$HAVE_EXPAT" = xyes ; then
  EXPAT_LIBS="-lexpat"
  AC_SUBST(EXPAT_LIBS)
else
  libcmml_config_ok="no"
fi

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup strtol])

# include Version_script
case "$target_os" in
  darwin* | rhapsody*)
    dnl Disable -Wall and -pedantic for Apple Darwin/Rhapsody.
    dnl System headers on these systems are broken.
    temp_CFLAGS=`echo $CFLAGS | sed "s/-Wall -pedantic//"`
    CFLAGS=$temp_CFLAGS
    ;;
  linux* | solaris*)
    SHLIB_VERSION_ARG="-Wl,--version-script=Version_script"
    ;;
  *)
    ;;
esac

# Checks for doxygen
AC_CHECK_PROG(HAVE_DOXYGEN, doxygen, true, false)
AM_CONDITIONAL(HAVE_DOXYGEN,$HAVE_DOXYGEN)
if test $HAVE_DOXYGEN = "false"; then
             AC_MSG_WARN([*** doxygen not found, docs will not be built])
fi
 
# Checks for docbook
AC_CHECK_PROG(HAVE_DOCBOOKTOMAN, docbook-to-man, true, false)
AM_CONDITIONAL(HAVE_DOCBOOKTOMAN,$HAVE_DOCBOOKTOMAN)


# configure complete
if test $libcmml_config_ok = no ; then

AC_MSG_RESULT([

*** libcmml $VERSION: Automatic configuration FAILED.

*** The file config.log has full details.

*** The following required libraries are missing or
*** misconfigured on your system:
])

if test "x$HAVE_EXPAT" = xno ; then
  AC_MSG_RESULT([
      XML parsing library expat is missing
      available from: http://expat.sourceforge.net/
])
fi

AC_MSG_ERROR([
***
*** After fixing the above problems, you must run ./configure again.
***
])

else

AC_SUBST(SHLIB_VERSION_ARG)
AC_SUBST(SHARED_VERSION_INFO)

AC_CONFIG_FILES([
Win32/config.h
doc/Doxyfile
doc/cmml-validate.1.sgml
doc/cmml-fix.1.sgml
doc/cmml-fortune.6.sgml
Makefile
src/Makefile
src/Version_script
tools/Makefile
examples/Makefile
tests/Makefile
doc/Makefile
cmml.pc])
AC_OUTPUT

AC_MSG_RESULT([

*** libcmml $VERSION: Automatic configuration SUCCESS.

**  Installation directories:
**
**     Library: ................. ${prefix}/lib
**     Headerfiles:...............${prefix}/include
**
**  Type 'make' to compile libcmml.
**
**  Type 'make install' to install libcmml.
**
**  Type 'make check' to test libcmml on the files *.cmml 
**  contained in the tests directory.
**
**  Command-line tools:
**  cmml-validate  for checking validity of a CMML file according 
**                 to the DTD
**
**  cmml-fix       for fixing a CMML file with sloppy errors and
**                 returning a valid CMML file
**
**  cmml-fortune   for creating a random valid CMML file
**
**
**  Example code in the examples directory.
**
**  Documentation: open doc/libcmml/html/index.html
])

fi
