dnl Process this file with autoconf to produce a configure script.
AC_INIT(ehs.h)
AM_INIT_AUTOMAKE(ehs, 1.3.1)
AM_CONFIG_HEADER(config.h)

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl Checks for libraries.
AM_PROG_LIBTOOL

# Define functions for specifying where pcre and pme live
AC_DEFUN([PCRE_WITH_PCRE_DIR], [
    AC_ARG_WITH([pcre], AC_HELP_STRING([--with-pcre=DIR], [  Specifies the location of the PCRE library]),
    [CPPFLAGS="$CPPFLAGS -I${withval}/include"
     CXXFLAGS="$CXXFLAGS -I${withval}/include"
     LDFLAGS="$LDFLAGS -L${withval}/lib"
dnl      AC_MSG_ERROR(HI ${withval})
],
    [])
])
AC_DEFUN([PME_WITH_PME_DIR], [
    AC_ARG_WITH([pme], AC_HELP_STRING([--with-pme=DIR], [  Specifies the location of the PME library]),
    [CPPFLAGS="$CPPFLAGS -I${withval}/include"
     CXXFLAGS="$CXXFLAGS -I${withval}/include"
     LDFLAGS="$LDFLAGS -L${withval}/lib"
dnl      AC_MSG_ERROR(HI ${withval})
],
    [])
])

# Call our new functions we just defined
PCRE_WITH_PCRE_DIR
PME_WITH_PME_DIR

dnl Replace `main' with a function in -lpcre:
AC_CHECK_LIB(pcre, main)
dnl Replace `main' with a function in -lpthread:
AC_CHECK_LIB(pthread, main)
dnl Replace `main' with a function in -lssl:
AC_CHECK_LIB(ssl, main)

AC_CHECK_LIB(sokt, socket)

AC_CHECK_LIB(pme, main,, AC_MSG_ERROR(EHS requires libpme from http://xaxxon.slackworks.com/pme/))

AC_ARG_WITH(ssl,[  --with-ssl              compile with SSL support (requires OpenSSL)],AC_CHECK_LIB(ssl,main,AC_DEFINE([COMPILE_WITH_SSL], [1], [Whether to compile with SSL support or not])))
AC_ARG_WITH(debug,[  --with-debug            compile with debugging information on],AC_DEFINE([EHS_DEBUG],,[Whether to compile with debugging information]))
AC_ARG_WITH(memory,[  --with-memory           compile with memory tracking information],AC_DEFINE([EHS_MEMORY],,[Whether to compile with memory tracking information]))

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(sys/ioctl.h sys/time.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(select)

AC_OUTPUT(Makefile)
