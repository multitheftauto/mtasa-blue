## Process this file with automake to produce Makefile.in

# Make sure that when we re-make ./configure, we get the macros we need
ACLOCAL_AMFLAGS = -I m4

# This is so we can #include <sparsehash/foo>
AM_CPPFLAGS = -I$(top_srcdir)/src

# These are good warnings to turn on by default
if GCC
AM_CXXFLAGS = -Wall -W -Wwrite-strings -Woverloaded-virtual -Wshadow
endif

docdir = $(prefix)/share/doc/$(PACKAGE)-$(VERSION)
## This is for HTML and other documentation you want to install.
## Add your documentation files (in doc/) in addition to these boilerplate
## Also add a TODO file if you have one
dist_doc_DATA = AUTHORS COPYING ChangeLog INSTALL NEWS README README_windows.txt \
   TODO \
   doc/dense_hash_map.html			\
   doc/dense_hash_set.html			\
   doc/sparse_hash_map.html			\
   doc/sparse_hash_set.html			\
   doc/sparsetable.html                     	\
   doc/implementation.html			\
   doc/performance.html                         \
   doc/index.html                               \
   doc/designstyle.css

## The libraries (.so's) you want to install
lib_LTLIBRARIES =
## The location of the windows project file for each binary we make
WINDOWS_PROJECTS = sparsehash.sln

## unittests you want to run when people type 'make check'.
## TESTS is for binary unittests, check_SCRIPTS for script-based unittests.
## TESTS_ENVIRONMENT sets environment variables for when you run unittest,
## but it only seems to take effect for *binary* unittests (argh!)
TESTS =
check_SCRIPTS =
TESTS_ENVIRONMENT =

## This should always include $(TESTS), but may also include other
## binaries that you compile but don't want automatically installed.
noinst_PROGRAMS = $(TESTS) time_hash_map
WINDOWS_PROJECTS += vsprojects/time_hash_map/time_hash_map.vcproj


## vvvv RULES TO MAKE THE LIBRARIES, BINARIES, AND UNITTESTS

# All our .h files need to read the config information in config.h.  The
# autoheader config.h has too much info, including PACKAGENAME, that
# might conflict with other config.h's an application might #include.
# Thus, we create a "minimal" config.h, called sparseconfig.h, that
# includes only the #defines we really need, and that are unlikely to
# change from system to system.  NOTE: The awk command is equivalent to
#  fgrep -B2 -f$(top_builddir)/src/config.h.include $(top_builddir)/src/config.h
#  | fgrep -vx -e -- > _sparsehash_config
# For correctness, it depends on the fact config.h.include does not have
# any lines starting with #.
src/sparsehash/internal/sparseconfig.h: $(top_builddir)/src/config.h \
                                        $(top_srcdir)/src/config.h.include
	[ -d $(@D) ] || mkdir -p $(@D)
	echo "/*" > $(@D)/_sparsehash_config
	echo " * NOTE: This file is for internal use only." >> $(@D)/_sparsehash_config
	echo " *       Do not use these #defines in your own program!" >> $(@D)/_sparsehash_config
	echo " */" >> $(@D)/_sparsehash_config
	$(AWK) '{prevline=currline; currline=$$0;} \
	        /^#/ {in_second_file = 1;} \
	        !in_second_file {if (currline !~ /^ *$$/) {inc[currline]=0}}; \
	       	in_second_file { for (i in inc) { \
	       	                   if (index(currline, i) != 0) { \
	       	                      print "\n"prevline"\n"currline; \
	       	                      delete inc[i]; \
	       	                   } \
	       	                 } }' \
	$(top_srcdir)/src/config.h.include $(top_builddir)/src/config.h \
	>> $(@D)/_sparsehash_config
	mv -f $(@D)/_sparsehash_config $@
# This is how we tell automake about auto-generated .h files
BUILT_SOURCES = src/sparsehash/internal/sparseconfig.h
CLEANFILES = src/sparsehash/internal/sparseconfig.h

sparsehashincludedir = $(includedir)/sparsehash
## The .h files you want to install (that is, .h files that people
## who install this package can include in their own applications.)
sparsehashinclude_HEADERS =			\
   src/sparsehash/dense_hash_map		\
   src/sparsehash/dense_hash_set		\
   src/sparsehash/sparse_hash_map		\
   src/sparsehash/sparse_hash_set		\
   src/sparsehash/sparsetable			\
   src/sparsehash/template_util.h		\
   src/sparsehash/type_traits.h

internalincludedir = $(sparsehashincludedir)/internal
internalinclude_HEADERS =					\
   src/sparsehash/internal/densehashtable.h			\
   src/sparsehash/internal/sparsehashtable.h			\
   src/sparsehash/internal/hashtable-common.h			\
   src/sparsehash/internal/libc_allocator_with_realloc.h
nodist_internalinclude_HEADERS = src/sparsehash/internal/sparseconfig.h

# This is for backwards compatibility only.
googleincludedir = $(includedir)/google
googleinclude_HEADERS =					\
   src/google/dense_hash_map				\
   src/google/dense_hash_set				\
   src/google/sparse_hash_map				\
   src/google/sparse_hash_set				\
   src/google/sparsetable				\
   src/google/template_util.h				\
   src/google/type_traits.h

googleinternalincludedir = $(includedir)/google/sparsehash
googleinternalinclude_HEADERS=		\
   src/google/sparsehash/densehashtable.h		\
   src/google/sparsehash/sparsehashtable.h		\
   src/google/sparsehash/hashtable-common.h		\
   src/google/sparsehash/libc_allocator_with_realloc.h

TESTS += template_util_unittest
# TODO(csilvers): Update windows projects for template_util_unittest.
# WINDOWS_PROJECTS += vsprojects/template_util_unittest/template_util_unittest.vcproj
template_util_unittest_SOURCES =			\
   src/template_util_unittest.cc		\
   src/sparsehash/template_util.h
nodist_template_util_unittest_SOURCES = $(nodist_internalinclude_HEADERS)

TESTS += type_traits_unittest
WINDOWS_PROJECTS += vsprojects/type_traits_unittest/type_traits_unittest.vcproj
type_traits_unittest_SOURCES =			\
   src/type_traits_unittest.cc			\
   $(internalinclude_HEADERS)			\
   src/sparsehash/type_traits.h
nodist_type_traits_unittest_SOURCES = $(nodist_internalinclude_HEADERS)

TESTS += libc_allocator_with_realloc_test
WINDOWS_PROJECTS += vsprojects/libc_allocator_with_realloc_test/libc_allocator_with_realloc_test.vcproj
libc_allocator_with_realloc_test_SOURCES =	\
   src/libc_allocator_with_realloc_test.cc	\
   $(internalinclude_HEADERS)			\
   src/sparsehash/internal/libc_allocator_with_realloc.h


TESTS += sparsetable_unittest
WINDOWS_PROJECTS += vsprojects/sparsetable_unittest/sparsetable_unittest.vcproj
sparsetable_unittest_SOURCES =			\
   src/sparsetable_unittest.cc			\
   $(internalinclude_HEADERS)			\
   src/sparsehash/sparsetable
nodist_sparsetable_unittest_SOURCES = $(nodist_internalinclude_HEADERS)

TESTS += hashtable_test
WINDOWS_PROJECTS += vsprojects/hashtable_test/hashtable_test.vcproj
hashtable_test_SOURCES =			\
   src/hashtable_test.cc			\
   src/hash_test_interface.h			\
   src/testutil.h				\
   $(sparsehashinclude_HEADERS)			\
   $(internalinclude_HEADERS)
nodist_hashtable_test_SOURCES = $(nodist_internalinclude_HEADERS)

TESTS += simple_test
WINDOWS_PROJECTS += vsprojects/simple_test/simple_test.vcproj
simple_test_SOURCES =	   		        \
   src/simple_test.cc				\
   $(internalinclude_HEADERS)
nodist_simple_test_SOURCES = $(nodist_internalinclude_HEADERS)

TESTS += simple_compat_test
simple_compat_test_SOURCES =   		        \
   src/simple_compat_test.cc			\
   $(internalinclude_HEADERS)                   \
   $(googleinclude_HEADERS)	\
   $(googleinternalinclude_HEADERS)
nodist_simple_compat_test_SOURCES = $(nodist_internalinclude_HEADERS)

time_hash_map_SOURCES =				\
   src/time_hash_map.cc				\
   $(internalinclude_HEADERS)			\
   $(sparsehashinclude_HEADERS)
nodist_time_hash_map_SOURCES = $(nodist_internalinclude_HEADERS)

# If tcmalloc is installed, use it with time_hash_map; it gives us
# heap-usage statistics for the hash_map routines, which is very nice
time_hash_map_CXXFLAGS = @tcmalloc_flags@ $(AM_CXXFLAGS)
time_hash_map_LDFLAGS = @tcmalloc_flags@
time_hash_map_LDADD = @tcmalloc_libs@

## ^^^^ END OF RULES TO MAKE THE LIBRARIES, BINARIES, AND UNITTESTS


rpm: dist-gzip packages/rpm.sh packages/rpm/rpm.spec
	@cd packages && ./rpm.sh ${PACKAGE} ${VERSION}

deb: dist-gzip packages/deb.sh packages/deb/*
	@cd packages && ./deb.sh ${PACKAGE} ${VERSION}

# http://linux.die.net/man/1/pkg-config, http://pkg-config.freedesktop.org/wiki
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = lib${PACKAGE}.pc
CLEANFILES += $(pkgconfig_DATA)

# I get the description and URL lines from the rpm spec. I use sed to
# try to rewrite exec_prefix, libdir, and includedir in terms of
# prefix, if possible.
lib${PACKAGE}.pc: Makefile packages/rpm/rpm.spec
	echo 'prefix=$(prefix)' > "$@".tmp
	echo 'exec_prefix='`echo '$(exec_prefix)' | sed 's@^$(prefix)@$${prefix}@'` >> "$@".tmp
	echo 'libdir='`echo '$(libdir)' | sed 's@^$(exec_prefix)@$${exec_prefix}@'` >> "$@".tmp
	echo 'includedir='`echo '$(includedir)' | sed 's@^$(prefix)@$${prefix}@'` >> "$@".tmp
	echo '' >> "$@".tmp
	echo 'Name: $(PACKAGE)' >> "$@".tmp
	echo 'Version: $(VERSION)' >> "$@".tmp
	-grep '^Summary:' $(top_srcdir)/packages/rpm/rpm.spec | sed s/^Summary:/Description:/ | head -n1 >> "$@".tmp
	-grep '^URL: ' $(top_srcdir)/packages/rpm/rpm.spec >> "$@".tmp
	echo 'Requires:' >> "$@".tmp
	echo 'Libs:' >> "$@".tmp
	echo 'Cflags: -I$${includedir}' >> "$@".tmp
	mv -f "$@".tmp "$@"

# Windows wants write permission to .vcproj files and maybe even sln files.
dist-hook:
	test -e "$(distdir)/vsprojects" \
	   && chmod -R u+w $(distdir)/*.sln $(distdir)/vsprojects/

EXTRA_DIST = packages/rpm.sh packages/rpm/rpm.spec packages/deb.sh packages/deb \
             src/config.h.include src/windows $(WINDOWS_PROJECTS) experimental
