INCLUDES += -I$(srcdir)/../sdk/
INCLUDES += -I$(srcdir)/../../Shared/sdk/
INCLUDES += $(CURSES_CFLAGS)

if PRECOMPILE_HEADERS
BUILT_SOURCES = StdInc.h.gch
AM_CXXFLAGS += -Winvalid-pch

StdInc.h.gch:
	$(LIBTOOL) --tag=CXX $(core_la_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile \
		$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
		$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)  \
		-o core_la-StdInc.h.lo $(srcdir)/StdInc.h \
		-MT core_la-StdInc.h.gch -MD -MP -MF $(DEPDIR)/core_la-StdInc.h.Tpo

	mv -f $(DEPDIR)/core_la-StdInc.h.Tpo $(DEPDIR)/core_la-StdInc.h.Plo
	mv -f .libs/core_la-StdInc.h.o $@

include ./$(DEPDIR)/core_la-StdInc.h.Plo

clean-local:
	-rm -f StdInc.h.gch
endif

libdir = $(outputdir)
lib_LTLIBRARIES = core.la
core_la_LIBTOOLFLAGS = --tag=disable-static
core_la_LIBADD = $(CURSES_LIBS)
core_la_LDFLAGS = -module -avoid-version
core_la_SOURCES = \
	CCrashHandlerAPI.cpp \
	CCrashHandlerAPI.h \
	CCrashHandler.cpp \
	CCrashHandler.h \
	CCriticalSection.h \
	CDynamicLibrary.cpp \
	CDynamicLibrary.h \
	CModManagerImpl.cpp \
	CModManagerImpl.h \
	CServerImpl.cpp \
	CServerImpl.h \
	CThreadCommandQueue.cpp \
	CThreadCommandQueue.h \
	ErrorCodes.h \
	Server.cpp \
	sockets/CTCPClientSocketImpl.cpp \
	sockets/CTCPClientSocketImpl.h \
	sockets/CTCPImpl.cpp \
	sockets/CTCPImpl.h \
	sockets/CTCPServerSocketImpl.cpp \
	sockets/CTCPServerSocketImpl.h \
	sockets/CTCPServerSocketImplManager.cpp \
	sockets/CTCPServerSocketImplManager.h
