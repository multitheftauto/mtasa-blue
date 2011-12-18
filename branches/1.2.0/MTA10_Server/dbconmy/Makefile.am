ZIPDIR=$(srcdir)/../../vendor/zip/
LUADIR=$(srcdir)/../../vendor/lua
LUABDIR=$(builddir)/../../vendor/lua

INCLUDES += -I$(srcdir)/logic/
INCLUDES += -I$(srcdir)/../sdk/
INCLUDES += -I$(srcdir)/../../Shared/sdk/
INCLUDES += -I$(srcdir)/../../Shared/publicsdk/include/
INCLUDES += -I$(srcdir)/../../vendor/ehs/
INCLUDES += -I$(srcdir)/../../vendor/pme/
INCLUDES += -I$(srcdir)/../../vendor/json-c/
INCLUDES += -I$(srcdir)/../../vendor/bob_withers/
INCLUDES += -I$(srcdir)/../../vendor/bochs/
INCLUDES += -I/usr/include/mysql
INCLUDES += -I$(srcdir)/../../Shared/animation/
INCLUDES += -I$(ZIPDIR)/
INCLUDES += -I$(LUADIR)/src
INCLUDES += -I$(LUABDIR)/src
INCLUDES += $(SPARSEHASH_CFLAGS)
INCLUDES += $(SQLITE3_CFLAGS)
# pcre headers used by pme which is used by ehs
INCLUDES += $(PCRE_CFLAGS)

if PRECOMPILE_HEADERS
BUILT_SOURCES = StdInc.h.gch
AM_CXXFLAGS += -Winvalid-pch

StdInc.h.gch:
	$(LIBTOOL) --tag=CXX $(dbconmy_la_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile \
		$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
		$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)  \
		-o dbconmy_la-StdInc.h.lo $(srcdir)/StdInc.h \
		-MT dbconmy_la-StdInc.h.gch -MD -MP -MF $(DEPDIR)/dbconmy_la-StdInc.h.Tpo

	mv -f $(DEPDIR)/dbconmy_la-StdInc.h.Tpo $(DEPDIR)/dbconmy_la-StdInc.h.Plo
	mv -f .libs/dbconmy_la-StdInc.h.o $@

include ./$(DEPDIR)/dbconmy_la-StdInc.h.Plo

clean-local:
	-rm -f StdInc.h.gch
endif

libdir = $(outputdir)/mods/deathmatch
lib_LTLIBRARIES = dbconmy.la
dbconmy_la_LIBTOOLFLAGS = --tag=disable-static
dbconmy_la_SOURCES = \
	CDatabaseConnectionMySql.cpp \
	StdInc.cpp

dbconmy_la_LIBADD = \
	../sdk/libplatform.la \
    -lmysqlclient

dbconmy_la_LDFLAGS = \
	-module -avoid-version
