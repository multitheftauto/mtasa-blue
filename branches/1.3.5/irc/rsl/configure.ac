#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT(RSL, 0.2, rydencillo@gmail.com)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([net/socket/client.h])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile file/Makefile file/ini/Makefile net/Makefile
                 net/http/Makefile net/irc/Makefile net/socket/Makefile
                 stream/Makefile])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Includes
sinclude(file/acinclude.m4)
sinclude(file/ini/acinclude.m4)
sinclude(net/acinclude.m4)
sinclude(net/http/acinclude.m4)
sinclude(net/irc/acinclude.m4)
sinclude(net/socket/acinclude.m4)
sinclude(stream/acinclude.m4)

# Checks for library functions.
sinclude(doxygen.m4)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(rsl, doxygen.cfg)

AC_OUTPUT
