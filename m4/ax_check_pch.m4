AC_DEFUN([AM_CHECK_PCH], [

GCC_PCH=0
PCH_FLAGS=
pch_default="yes"
AM_CONDITIONAL([PRECOMPILE_HEADERS], [false])
AC_ARG_ENABLE(pch, [AC_HELP_STRING([--enable-pch], [use precompiled headers if available (default YES)])],,
                                                enable_pch=$pch_default)
if test "x$enable_pch" = "x" -o "x$enable_pch" = "xyes" ; then
        if test "x$GCC" = "xyes"; then
                dnl test if we have gcc-3.4:
                AC_MSG_CHECKING([if the compiler supports precompiled headers])
                AC_TRY_COMPILE([],
                        [
                                #if !defined(__GNUC__) || !defined(__GNUC_MINOR__)
                                        #error "no pch support"
                                #endif
                                #if (__GNUC__ < 3)
                                        #error "no pch support"
                                #endif
                                #if (__GNUC__ == 3) && \
                                        ((!defined(__APPLE_CC__) && (__GNUC_MINOR__ < 4)) || \
                                        ( defined(__APPLE_CC__) && (__GNUC_MINOR__ < 3)))
                                        #error "no pch support"
                                #endif
                        ],
                        [
                                AC_MSG_RESULT([yes])
                                GCC_PCH=1
                                PCH_FLAGS="-DCB_PRECOMP -Winvalid-pch"
                        ],
                        [
                                AC_MSG_RESULT([no])
                        ])
                AM_CONDITIONAL([PRECOMPILE_HEADERS], [test $GCC_PCH = 1])
        fi
fi

])
