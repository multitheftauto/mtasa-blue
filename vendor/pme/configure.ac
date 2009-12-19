dnl Process this file with autoconf to produce a configure script.
AC_INIT(pme.h)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(pme, 1.0.4)




dnl Checks for programs.

AC_PROG_CXX

dnl Checks for libraries.

dnl defined in pcre.m4 in pcre-wrapper directory
dnl AC_PATH_PCRE

AC_DEFUN([PCRE_WITH_PCRE_DIR], [
    AC_ARG_WITH([pcre], AC_HELP_STRING([--with-pcre=DIR], [  Specifies the location of the PCRE library]),
    [CPPFLAGS="$CPPFLAGS -I${withval}/include"
     CXXFLAGS="$CXXFLAGS -I${withval}/include"
     LDFLAGS="$LDFLAGS -L${withval}/lib"
dnl	 AC_MSG_ERROR(HI ${withval})
],
    [])
])

PCRE_WITH_PCRE_DIR

AC_CHECK_LIB(pcre, main,, AC_MSG_ERROR(PME requires libpcre from http://www.pcre.org/))

AM_PROG_LIBTOOL

dnl Checks for header files.



dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl Checks for library functions.

AC_OUTPUT(Makefile)
