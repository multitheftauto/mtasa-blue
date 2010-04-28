AC_PREREQ(2.59)
AC_INIT([Autotoolized Lua], [5.1.3], [], [lua-at])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/lapi.c])

AM_INIT_AUTOMAKE([1.9 foreign])

AC_PROG_CC
AC_PROG_LIBTOOL

AC_ARG_WITH(
  [readline],
  [AC_HELP_STRING([--with-readline], [Use readline for interpreter input [default=yes]])],
  [use_readline=$withval],
  [use_readline=yes]
)

LUA_LIBS="-lm"

# Check for readline
READLINE_DEFS="#undef LUA_USE_READLINE"
if test "x$use_readline" == "xyes"; then
  AC_CHECK_LIB([readline], [readline], [], [use_readline=no], [-lncurses])
  AC_CHECK_HEADERS([readline/readline.h readline/history.h], [], [use_readline=no])
  if test "x$use_readline" == "xno"; then
    AC_MSG_WARN([readline headers could not be found, disabling readline support])
  else
    READLINE_DEFS="#define LUA_USE_READLINE"
    LUA_LIBS="$LUA_LIBS -lreadline -lncurses"
  fi
fi
AC_SUBST(READLINE_DEFS)

case "$host" in
  *-mingw*)  use_os=win32  ;;
  *-darwin*) use_os=macosx ;;
  *)         use_os=posix  ;;
esac

POSIX_DEFS="#undef LUA_USE_POSIX"
LUA_DL_DEFS="#undef LUA_USE_DLOPEN"
LUA_BUILD_AS_DLL_DEFS="#undef LUA_BUILD_AS_DLL"

if test "x$use_os" == "xwin32"; then
  LUA_BUILD_AS_DLL_DEFS="#define LUA_BUILD_AS_DLL"
elif test "x$use_os" == "xmacosx"; then
  POSIX_DEFS="#define LUA_USE_POSIX"
  LUA_DL_DEFS="#define LUA_DL_DYLD"
elif test "x$use_os" == "xposix"; then
  POSIX_DEFS="#define LUA_USE_POSIX"
  LUA_DL_DEFS="#define LUA_DL_DLOPEN"
  LUA_LIBS="$LUA_LIBS -ldl"
fi
AC_SUBST(POSIX_DEFS)
AC_SUBST(LUA_DL_DEFS)
AC_SUBST(LUA_BUILD_AS_DLL_DEFS)

AC_SUBST(LUA_LIBS)

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/luaconf.h.template])
AC_OUTPUT
