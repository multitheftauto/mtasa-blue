INCLUDES += -I$(srcdir)/logic/
INCLUDES += -I$(srcdir)/../../sdk/
INCLUDES += -I$(srcdir)/../../../Shared/sdk/
INCLUDES += -I$(srcdir)/../../../Shared/publicsdk/include/
INCLUDES += -I$(srcdir)/../../../vendor/
INCLUDES += -I$(srcdir)/../../../vendor/pme/
INCLUDES += -I$(srcdir)/../../../vendor/json-c/

BUILT_SOURCES = Makefile.files
Makefile.files: Makefile.am
	@echo Generating Makefile.files
	echo UTILS = `ls --quoting-style=literal --format=horizontal \
		$(srcdir)/utils/*.c \
		$(srcdir)/utils/*.cpp \
		$(srcdir)/utils/zip/*.c \
		$(srcdir)/utils/zip/*.cpp \
	` > $(srcdir)/Makefile.files
	echo LOGIC = `ls --quoting-style=literal --format=horizontal \
		$(srcdir)/logic/*.cpp \
		$(srcdir)/logic/luadefs/*.cpp \
		$(srcdir)/logic/lua/*.cpp \
		$(srcdir)/logic/packets/*.cpp \
	` >> $(srcdir)/Makefile.files

include $(srcdir)/Makefile.files

if PRECOMPILE_HEADERS
BUILT_SOURCES += StdInc.h.gch
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
	libdeathmatch.la

libdeathmatch_la_SOURCES = \
	Server.cpp \
	CServer.cpp \
	$(UTILS) \
	$(LOGIC) \
	StdInc.cpp

libdeathmatch_la_LIBADD = \
	@PCRE_LIBS@ \
	-lehs \
	-lpme \
	-lplatform \
	-lsqlite3 \
	-llua \
	-ljson

libdeathmatch_la_LDFLAGS = \
	-Wl,-export-dynamic \
	-L../../sdk/ \
	-L../../../vendor/json-c/ \
	-L../../../vendor/ehs/ \
	-L../../../vendor/pme/ \
	-L../../../libs/ \
	-L../../../libs/lua \
	-L/usr/lib

