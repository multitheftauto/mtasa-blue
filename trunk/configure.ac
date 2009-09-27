# License here
#

AC_PREREQ(2.61)
AC_INIT(mta, 1.0-trunk, nospam@mtanet)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])

if test x$prefix = xNONE; then
        prefix=/usr/local
fi

AC_SUBST(prefix)

AC_SUBST(INCLUDES)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE
AM_PROG_CC_C_O
AC_PROG_MAKE_SET
AM_CHECK_PCH

# Templates
AH_TEMPLATE([USING_BIG_ENDIAN], Define this if you are on a big-endian system.)
AH_TEMPLATE([USING_LITTLE_ENDIAN], Define this if you are on a non-big-endian system.)
AH_TEMPLATE([X64], Define this if you are under an X64-based operating system.)
AH_TEMPLATE([X86], Define this if you are under an X86-based 32bit operating system.)
AH_TEMPLATE([MTA_REVISION], The revision of the latest checkout)

# Flags 

AC_ARG_ENABLE(debug,
     AC_HELP_STRING([--enable-debug],[enable debug]),
     [ if test "x$enable_debug" = "xyes" ; then
           CFLAGS="$CFLAGS -g"
           CXXFLAGS="$CXXFLAGS -g"
       fi
     ],
)

# Checks for libraries.

# pcre
AX_PATH_LIB_PCRE()
AC_SUBST(PCRE_CFLAGS)
AC_SUBST(PCRE_LIBS)

# sqlite3
AX_LIB_SQLITE3()

# curl
AC_CHECK_CURL(7)

# Checking for a 64-bit system
AC_MSG_CHECKING([uname -m for detecting host operating system architecture])
ARCH=`uname -m`
case "$ARCH" in
	amd64*)
		dnl yay 21st century hardware
		AC_MSG_RESULT(x64 feel teh power)
		AC_DEFINE(X64)
		AC_SUBST(ARCH,x64)
	;;
	x86_64*)
		dnl yay 21st century hardware
		AC_MSG_RESULT(x64 feel teh power)
		AC_DEFINE(X64)
		AC_SUBST(ARCH,x64)
	;;
	*)
		AC_MSG_RESULT(x86 oh noes i ran out of address space)
		AC_DEFINE(X86)
		AC_SUBST(ARCH,x86)
	;;
esac

# Platform specific rules
AC_MSG_CHECKING([operating system])
case $host in
	*-linux*)
		AC_MSG_RESULT(linux)
		AC_CHECK_LIB(rt, clock_gettime)
	;;
	*)
		AC_MSG_RESULT(non-linux)
	;;
esac

# Check for BigEndian
AC_C_BIGENDIAN([AC_DEFINE(USING_BIG_ENDIAN)],[AC_DEFINE(USING_LITTLE_ENDIAN)],[])

# Check for zlib
AX_CHECK_ZLIB()

# Check for curses
AC_CHECK_LIB(curses, stdscr,, AC_MSG_ERROR([can't find curses]))

# Define compile/library flags.
AC_SUBST(AM_CXXFLAGS, "$AM_CXXFLAGS $INCLUDES -DLINUX $PCRE_CFLAGS -Wall -DTIXML_USE_STL")
AC_SUBST(AM_LDFLAGS, "$AM_LDFLAGS -lstdc++")

# Checks for header files.
AC_CHECK_HEADERS(curses.h sqlite3.h pcre.h fcntl.h limits.h strings.h syslog.h unistd.h [sys/param.h] stdarg.h,, AC_MSG_ERROR([You are missing one or more header(s)]))

# Check for libraries
AC_CHECK_FUNCS(strndup strerror vsnprintf vasprintf open vsyslog strncasecmp)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

AC_LANG(C++)

# Check for google sparsehash
AC_CHECK_HEADERS([google/sparse_hash_map],,AC_MSG_ERROR([can't find google sparse_hash_map]))

# Checks for library functions.

# Extract the latest SVN revision
AC_CHECK_PROG([SVNVERSION], [svnversion], yes)
if test -z "$SVNVERSION"; then
	AC_MSG_ERROR(Subversion 'svnversion' not found)
else
	MTA_REVISION=`/usr/bin/env svnversion -n`
	if test -z "$MTA_REVISION"; then MTA_REVISION="n/a"; fi

	AC_MSG_NOTICE(Extracted SVN version: $MTA_REVISION)
	AC_DEFINE_UNQUOTED(MTA_REVISION,["\$Rev: $MTA_REVISION \$"])
	AC_SUBST(MTA_REVISION)
fi

# Configure subdirectories where necessary
AC_CONFIG_SUBDIRS([
	vendor/lua
	])

# Generate makefiles
AC_CONFIG_FILES([
	Makefile
	vendor/Makefile
	vendor/tinyxml/Makefile
	vendor/pme/Makefile
	vendor/ehs/Makefile
	vendor/json-c/Makefile
	vendor/lua/Makefile
	vendor/lua/src/Makefile
	Shared/Makefile
	Shared/XML/Makefile
	MTA10_Server/Makefile
	MTA10_Server/sdk/Makefile
	MTA10_Server/core/Makefile
	MTA10_Server/mods/Makefile
	MTA10_Server/mods/deathmatch/Makefile
	MTA10_Server/launcher/Makefile
])

AC_OUTPUT
