noinst_PROGRAMS=pmetest splittest subtest
lib_LTLIBRARIES=libpme.la
libpme_la_SOURCES=pme.cpp
libpme_la_LIBADD=$(PCRE_LIBS)

EXTRA_DIST=vs6 building_for_windows.txt

pmetest_SOURCES=pmetest.cpp
pmetest_LDFLAGS=$(PCRE_LIBS)
pmetest_LDADD=libpme.la

splittest_SOURCES=splittest.cpp
splittest_LDFLAGS=$(PCRE_LIBS)
splittest_LDADD=libpme.la

subtest_SOURCES=subtest.cpp
subtest_LDFLAGS=$(PCRE_LIBS)
subtest_LDADD=libpme.la

pmeincludedir=${includedir}
pmeinclude_HEADERS=pme.h