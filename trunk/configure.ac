# License here
#

AC_PREREQ(2.61)
AC_INIT(mta, 1.0-trunk, nospam@mtanet)
AC_CONFIG_AUX_DIR([.])
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])

if test x$prefix = xNONE; then
        prefix=/usr/local
fi

# Output directory
outputdir=`pwd`/MTA10_Server/output

AC_SUBST(outputdir)
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

# pcre (for bundled pme.h)
AC_ARG_ENABLE([system-pcre], 
	[AS_HELP_STRING([--disable-system-pcre],
	                [don't use system PCRE])])

AC_DEFUN([USE_BUNDLED_PCRE], [
	PCRE_CFLAGS="-I\$(top_builddir)/vendor/pcre"
	PCRE_LIBS="\$(top_builddir)/vendor/pcre/libpcre.la"
	# probably include a check if it was already built
])

if test "x$enable_system_pcre" != "xno" ; then
	PKG_CHECK_MODULES([PCRE], [libpcre],, [
		if test "x$enable_system_pcre" != "xyes" ; then
			AC_MSG_WARN(["system PCRE not found, using bundled one"])
			USE_BUNDLED_PCRE
		else
			AC_MSG_ERROR(["system PCRE not found"])
		fi
	])
else
	USE_BUNDLED_PCRE
fi

# google sparsehash (for mods/deathmatch)
AC_ARG_ENABLE([system-sparsehash],
	[AS_HELP_STRING([--disable-system-sparsehash],
	                [don't use system sparsehash])])

AC_ARG_VAR([SPARSEHASH_CFLAGS], [C compiler flags for sparsehash])
AC_ARG_VAR([SPARSEHASH_LIBS], [linker flags for sparsehash])

AC_DEFUN([USE_BUNDLED_SPARSEHASH], [
	SPARSEHASH_CFLAGS="-I\$(top_builddir)/vendor/sparsehash/current/src"
])

if test "x$enable_system_sparsehash" != "xno" ; then
	save_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $SPARSEHASH_CFLAGS"
	AC_CHECK_HEADERS([google/sparse_hash_map],, [
		if test "x$enable_system_sparsehash" != "xyes" ; then
			AC_MSG_WARN(["system sparsehash not found, using bundled one"])
			USE_BUNDLED_SPARSEHASH
		else
			AC_MSG_ERROR(["system sparsehash not found"])
		fi
	])
else
	USE_BUNDLED_SPARSEHASH
fi
CPPFLAGS="$save_CPPFLAGS"

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
	*-darwin*)
		AC_MSG_RESULT(darwin)
	;;
	*-bsd*)
		AC_MSG_RESULT(bsd)
	;;
	*)
		AC_MSG_ERROR(unsupported)
	;;
esac

# Check for BigEndian
AC_C_BIGENDIAN([AC_DEFINE(USING_BIG_ENDIAN)],[AC_DEFINE(USING_LITTLE_ENDIAN)],[])

# Check for zlib
AX_CHECK_ZLIB()

# Check for curses
AC_CHECK_LIB(curses, stdscr,, AC_MSG_ERROR([can't find curses]))

# Define compile/library flags.
AC_SUBST(AM_CXXFLAGS, "$AM_CXXFLAGS $INCLUDES -Wall -DTIXML_USE_STL")
AC_SUBST(AM_LDFLAGS, "$AM_LDFLAGS -lstdc++")

# Checks for header files.
AC_CHECK_HEADERS(curses.h sqlite3.h fcntl.h limits.h strings.h syslog.h unistd.h [sys/param.h] stdarg.h,, AC_MSG_ERROR([You are missing one or more header(s)]))

# Check for libraries
AC_CHECK_FUNCS(strndup strerror vsnprintf vasprintf open vsyslog strncasecmp)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

AC_LANG(C++)

# Checks for library functions.

# Configure subdirectories where necessary
#   NOTE: Put these on separate single lines due to a bug in autoconf
#   http://readlist.com/lists/lists.sourceforge.net/mingw-users/2/12488.html
AC_CONFIG_SUBDIRS([vendor/lua])
AC_CONFIG_SUBDIRS([vendor/json-c])

# Generate makefiles
AC_CONFIG_FILES([
	Makefile
	vendor/Makefile
	vendor/tinyxml/Makefile
	vendor/pme/Makefile
	vendor/ehs/Makefile
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
