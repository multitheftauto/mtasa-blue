# Build EHS library
noinst_LTLIBRARIES=libehs.la

# Sources for building EHS library
libehs_la_SOURCES=ehs.cpp dynamicssllocking.cpp securesocket.cpp socket.cpp\
sslerror.cpp staticssllocking.cpp datum.cpp httpresponse.cpp httprequest.cpp
libehs_la_CXXFLAGS=-I$(srcdir)/../pme -I$(srcdir)/../../Shared/sdk
libehs_la_LIBADD=$(top_builddir)/vendor/pme/libpme.la

# add includes for pcre which is what pme needs
libehs_la_CXXFLAGS+=$(PCRE_CFLAGS)
