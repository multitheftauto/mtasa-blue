AC_CHECK_HEADERS([sys/epoll.h], [ HAVE_EPOLL=true ], [
  HAVE_EPOLL=false
  AC_CHECK_HEADERS([sys/poll.h], [ HAVE_POLL=true ], [
    HAVE_POLL=false
    AC_CHECK_HEADERS([sys/select.h], [ HAVE_POLL=true ], AC_MSG_ERROR([Unable to find a socket selection engine]))
  ])
])
AM_CONDITIONAL([RSL_USE_EPOLL], [test x$HAVE_EPOLL = xtrue])
AM_CONDITIONAL([RSL_USE_POLL], [test x$HAVE_POLL = xtrue])
AM_CONDITIONAL([RSL_USE_SELECT], [test x$HAVE_SELECT = xtrue])
