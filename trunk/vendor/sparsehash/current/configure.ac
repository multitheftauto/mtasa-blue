## Process this file with autoconf to produce configure.
## In general, the safest way to proceed is to run ./autogen.sh

# make sure we're interpreted by some minimal autoconf
AC_PREREQ(2.57)

AC_INIT(sparsehash, 1.4, opensource@google.com)
# The argument here is just something that should be in the current directory
# (for sanity checking)
AC_CONFIG_SRCDIR(README)
AM_INIT_AUTOMAKE([dist-zip])
AM_CONFIG_HEADER(src/config.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AM_CONDITIONAL(GCC, test "$GCC" = yes)   # let the Makefile know if we're gcc

# Check whether some low-level functions/files are available
AC_HEADER_STDC
AC_CHECK_FUNCS(memcpy memmove)
AC_CHECK_TYPES([uint16_t])     # defined in C99 systems
AC_CHECK_TYPES([u_int16_t])    # defined in BSD-derived systems, and gnu
AC_CHECK_TYPES([__uint16])     # defined in some windows systems (vc7)
AC_CHECK_TYPES([long long])    # probably defined everywhere, but...

# These are 'only' needed for unittests
AC_CHECK_HEADERS(sys/resource.h unistd.h sys/time.h sys/utsname.h)

# If you have google-perftools installed, we can do a bit more testing.
# We not only want to set HAVE_MALLOC_EXTENSION_H, we also want to set
# a variable to let the Makefile to know to link in tcmalloc.
AC_LANG([C++])
AC_CHECK_HEADERS(google/malloc_extension.h,
                 tcmalloc_libs=-ltcmalloc,
                 tcmalloc_libs=)
# On some systems, when linking in tcmalloc you also need to link in
# pthread.  That's a bug somewhere, but we'll work around it for now.
tcmalloc_flags=""
if test -n "$tcmalloc_libs"; then
  ACX_PTHREAD
  tcmalloc_flags="\$(PTHREAD_CFLAGS)"
  tcmalloc_libs="$tcmalloc_libs \$(PTHREAD_LIBS)"
fi
AC_SUBST(tcmalloc_flags)
AC_SUBST(tcmalloc_libs)

# Figure out where hash_map lives and also hash_fun.h (or stl_hash_fun.h).
# This also tells us what namespace hash code lives in.
AC_CXX_STL_HASH
AC_CXX_STL_HASH_FUN

# Find out what namespace 'normal' STL code lives in, and also what namespace
# the user wants our classes to be defined in
AC_CXX_STL_NAMESPACE
AC_DEFINE_GOOGLE_NAMESPACE(google)

# In unix-based systems, hash is always defined as hash<> (in namespace.
# HASH_NAMESPACE.)  So we can use a simple AC_DEFINE here.  On
# windows, and possibly on future unix STL implementations, this
# macro will evaluate to something different.)
AC_DEFINE(SPARSEHASH_HASH_NO_NAMESPACE, hash,
          [The system-provided hash function, in namespace HASH_NAMESPACE.])

# Do *not* define this in terms of SPARSEHASH_HASH_NO_NAMESPACE, because
# SPARSEHASH_HASH is exported to sparseconfig.h, but S_H_NO_NAMESPACE isn't.
AC_DEFINE(SPARSEHASH_HASH, HASH_NAMESPACE::hash,
          [The system-provided hash function including the namespace.])


# Write generated configuration file
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
