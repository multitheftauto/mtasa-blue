## Process this file with automake to produce Makefile.in

# Make sure that when we re-make ./configure, we get the macros we need
ACLOCAL_AMFLAGS = -I m4

# This is so we can #include <google/foo>
AM_CPPFLAGS = -I$(top_srcdir)/src

# These are good warnings to turn on by default
if GCC
AM_CXXFLAGS = -Wall -Wwrite-strings -Woverloaded-virtual -Wno-sign-compare -Wshadow
endif

googleincludedir = $(includedir)/google
## The .h files you want to install (that is, .h files that people
## who install this package can include in their own applications.)
googleinclude_HEADERS =				\
   src/google/dense_hash_map			\
   src/google/dense_hash_set			\
   src/google/sparse_hash_map			\
   src/google/sparse_hash_set			\
   src/google/sparsetable                       \
   src/google/type_traits.h

docdir = $(prefix)/share/doc/$(PACKAGE)-$(VERSION)
## This is for HTML and other documentation you want to install.
## Add your documentation files (in doc/) in addition to these boilerplate
## Also add a TODO file if you have one
dist_doc_DATA = AUTHORS COPYING ChangeLog INSTALL NEWS README README.windows \
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
WINDOWS_PROJECTS = google-sparsehash.sln

## unittests you want to run when people type 'make check'.
## TESTS is for binary unittests, check_SCRIPTS for script-based unittests.
## TESTS_ENVIRONMENT sets environment variables for when you run unittest,
## but it only seems to take effect for *binary* unittests (argh!)
TESTS = type_traits_unittest sparsetable_unittest hashtable_unittest \
        simple_test
# TODO(csilvers): get simple_test working on windows
WINDOWS_PROJECTS += vsprojects/type_traits_unittest/type_traits_unittest.vcproj \
                    vsprojects/sparsetable_unittest/sparsetable_unittest.vcproj \
                    vsprojects/hashtable_unittest/hashtable_unittest.vcproj
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
#    fgrep -B2 -f- $(top_builddir)/src/config.h \
#    fgrep -vx -e -- > _sparsehash_config
# For correctness, it depends on the fact config.h.include does not have
# any lines starting with #.
src/google/sparsehash/sparseconfig.h: $(top_builddir)/src/config.h \
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
BUILT_SOURCES = src/google/sparsehash/sparseconfig.h
CLEANFILES = src/google/sparsehash/sparseconfig.h

sparsehashincludedir = $(googleincludedir)/sparsehash
sparsehashinclude_HEADERS =			\
   src/google/sparsehash/densehashtable.h	\
   src/google/sparsehash/sparsehashtable.h
nodist_sparsehashinclude_HEADERS = src/google/sparsehash/sparseconfig.h

type_traits_unittest_SOURCES =			\
   src/type_traits_unittest.cc			\
   $(sparsehashinclude_HEADERS)			\
   src/google/type_traits.h
nodist_type_traits_unittest_SOURCES = $(nodist_sparsehashinclude_HEADERS)

sparsetable_unittest_SOURCES =			\
   src/sparsetable_unittest.cc			\
   $(sparsehashinclude_HEADERS)			\
   src/google/sparsetable
nodist_sparsetable_unittest_SOURCES = $(nodist_sparsehashinclude_HEADERS)

hashtable_unittest_SOURCES =			\
   src/hashtable_unittest.cc			\
   $(googleinclude_HEADERS)			\
   $(sparsehashinclude_HEADERS)			\
   src/words
nodist_hashtable_unittest_SOURCES = $(nodist_sparsehashinclude_HEADERS)

simple_test_SOURCES =	   		        \
   src/simple_test.cc				\
   $(sparsehashinclude_HEADERS)
nodist_simple_test_SOURCES = $(nodist_sparsehashinclude_HEADERS)

time_hash_map_SOURCES =				\
   src/time_hash_map.cc				\
   $(sparsehashinclude_HEADERS)			\
   $(googleinclude_HEADERS)
nodist_time_hash_map_SOURCES = $(nodist_sparsehashinclude_HEADERS)

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

# Windows wants write permission to .vcproj files and maybe even sln files.
dist-hook:
	test -e "$(distdir)/vsprojects" \
	   && chmod -R u+w $(distdir)/*.sln $(distdir)/vsprojects/

EXTRA_DIST = packages/rpm.sh packages/rpm/rpm.spec packages/deb.sh packages/deb \
             src/config.h.include src/windows $(WINDOWS_PROJECTS) experimental
