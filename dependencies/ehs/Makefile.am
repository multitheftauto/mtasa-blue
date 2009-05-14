
# Build EHS library
lib_LTLIBRARIES=libehs.la

# Sources for building EHS library
libehs_la_SOURCES=ehs.cpp dynamicssllocking.cpp securesocket.cpp socket.cpp\
sslerror.cpp staticssllocking.cpp datum.cpp httpresponse.cpp httprequest.cpp

# ??
ehsincludedir=${includedir}

# headers to be installed with the library
ehsinclude_HEADERS=ehs.h networkabstraction.h socket.h securesocket.h\
sslerror.h staticssllocking.h threadabstractionlayer.h dynamicssllocking.h\
datum.h httpresponse.h httprequest.h ehstypes.h formvalue.h\
contentdisposition.h

# Test programs to be built but not installed
noinst_PROGRAMS=ehs_formtest ehs_test ehs_testharness ehs_uploader ehs_simple ehs_mirror ehs_https

# Extra stuff to install 
EXTRA_DIST=ehs_development_guide.txt ehs-stress.pl vs6 building_for_windows.txt

# Rules for building test programs
ehs_formtest_SOURCES=samples/ehs_formtest.cpp
ehs_formtest_LDADD=libehs.la

ehs_test_SOURCES=samples/ehs_test.cpp
ehs_test_LDADD=libehs.la

ehs_testharness_SOURCES=samples/ehs_testharness.cpp
ehs_testharness_LDADD=libehs.la

ehs_uploader_SOURCES=samples/ehs_uploader.cpp
ehs_uploader_LDADD=libehs.la

ehs_simple_SOURCES=samples/ehs_simple.cpp
ehs_simple_LDADD=libehs.la

ehs_mirror_SOURCES=samples/ehs_mirror.cpp
ehs_mirror_LDADD=libehs.la

ehs_https_SOURCES=samples/ehs_https.cpp
ehs_https_LDADD=libehs.la

