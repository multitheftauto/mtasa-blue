#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT(WikiBot, 0.1, rydencillo@gmail.com)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([include/wikibot.h])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile src/Makefile data/Makefile])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
case $LEX in
:) AC_MSG_ERROR([No lex program found]);;
esac

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h sys/resource.h])

# Checks for functions
AC_CHECK_FUNCS([setrlimit])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.

AC_OUTPUT
