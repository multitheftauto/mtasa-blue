dnl File: Configure.ac 
dnl Author: Olivier Delannoy 
dnl Created: 2006-06-11

dnl Consts 
m4_define([pkg_name], [SILLY])
m4_define([pkg_prefix], [SILLY])
m4_define([pkg_maintener], [Olivier Delannoy <Olivier.Delannoy@gmail.com>])
m4_define([pkg_vers_major], [0])
m4_define([pkg_vers_minor], [1])
m4_define([pkg_vers_micro], [0])
m4_define([pkg_version], [pkg_vers_major.pkg_vers_minor.pkg_vers_micro])
m4_define([pkg_fullname],   [pkg_name pkg_version])

dnl Version requirements 
m4_define([required_autoconf_version], [2.59])
AC_PREREQ(required_autoconf_version)
AC_INIT([pkg_name], 
	[pkg_version], 
	[pkg_maintener], 
	[pkg_fullname])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([pkg_name], [pkg_version])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_HEADERS([include/SILLYOptions.h])
AC_CONFIG_SRCDIR([src/SILLYImage.cpp])
AM_SANITY_CHECK
AM_ENABLE_SHARED
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
dnl make libtool aware we may be building Win32 DLLs
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

dnl is this is on mingw32?
AC_MINGW32
AM_CONDITIONAL([SILLY_MINGW32], [test x$MINGW32 = xyes])

SILLY_OPT
SILLY_IMAGE_FORMAT

dnl versioning info for libtool 
dnl Note this is the ABI version which is not the same as our actual library version 
SILLY_CURRENT=1
SILLY_REVISION=0
SILLY_AGE=0
SILLY_VERSION_INFO=$SILLY_CURRENT:$SILLY_REVISION:$SILLY_AGE
AC_SUBST(SILLY_VERSION_INFO)
AC_CONFIG_FILES([Makefile
		 include/Makefile
		 src/Makefile
         SILLY.xcodeproj/Makefile
		 SILLY.pc])
AC_OUTPUT
dnl summary 
echo 
echo pkg_fullname
echo 
echo "Build options: "
echo "  Prefix ..........................: $prefix"
SILLY_OPT_SUM 
SILLY_IMAGE_FORMAT_SUM 
