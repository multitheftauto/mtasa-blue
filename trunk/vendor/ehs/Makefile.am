
# Build EHS library
noinst_LTLIBRARIES=libehs.la

# Sources for building EHS library
libehs_la_SOURCES=ehs.cpp dynamicssllocking.cpp securesocket.cpp socket.cpp\
sslerror.cpp staticssllocking.cpp datum.cpp httpresponse.cpp httprequest.cpp
libehs_la_CXXFLAGS=-I$(srcdir)/../pme
libehs_la_LDFLAGS=-L$(srcdir)/../pme -lpme
# ??
ehsincludedir=${includedir} -I(srcdir)/../pme

# headers to be installed with the library
ehsinclude_HEADERS=ehs.h networkabstraction.h socket.h securesocket.h\
sslerror.h staticssllocking.h threadabstractionlayer.h dynamicssllocking.h\
datum.h httpresponse.h httprequest.h ehstypes.h formvalue.h\
contentdisposition.h
