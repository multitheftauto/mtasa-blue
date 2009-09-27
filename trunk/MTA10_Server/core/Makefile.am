INCLUDES += -I$(srcdir)/../sdk/
INCLUDES += -I$(srcdir)/../../Shared/sdk/

if PRECOMPILE_HEADERS
BUILT_SOURCES = StdInc.h.gch
AM_CXXFLAGS += -Winvalid-pch

StdInc.h.gch: StdInc.h
	-rm -f StdInc.h.gch
	$(CXX) -fPIC -DPIC $(DEFS) $(DEFAULT_INCLUDES) \
		$(AM_CXXFLAGS) $(CXXFLAGS) $(CPPFLAGS) \
		$(INCLUDES) -o StdInc.h.gch -xc++-header StdInc.h

clean-local:
	-rm -f StdInc.h.gch
endif

lib_LTLIBRARIES = \
	libcore.la

libcore_la_LIBTOOLFLAGS = --tag=disable-static

libcore_la_SOURCES = \
	StdInc.cpp \
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

libcore_la_LDFLAGS = -lcurses
