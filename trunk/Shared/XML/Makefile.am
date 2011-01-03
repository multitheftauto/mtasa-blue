INCLUDES += -I$(srcdir)/../sdk/
INCLUDES += -I$(srcdir)/../../vendor/tinyxml/

if PRECOMPILE_HEADERS
BUILT_SOURCES = StdInc.h.gch
AM_CXXFLAGS += -Winvalid-pch

StdInc.h.gch: StdInc.h
	$(LIBTOOL) --tag=CXX $(xmll_la_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile \
		$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
		$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)  \
		-o StdInc.h.lo $^
	$(LN_S) -f .libs/StdInc.h.o $@

clean-local:
	-rm -f StdInc.h.gch
endif

libdir = $(outputdir)
lib_LTLIBRARIES = xmll.la
xmll_la_LIBTOOLFLAGS = --tag=disable-static
xmll_la_LDFLAGS = \
	-module -avoid-version \
	-L../../vendor/tinyxml -ltinyxml
xmll_la_SOURCES = \
	CXMLArray.cpp \
	CXMLArray.h \
	CXMLAttributeImpl.cpp \
	CXMLAttributeImpl.h \
	CXMLAttributesImpl.cpp \
	CXMLAttributesImpl.h \
	CXMLFileImpl.cpp \
	CXMLFileImpl.h \
	CXMLImpl.cpp \
	CXMLImpl.h \
	CXMLNodeImpl.cpp \
	CXMLNodeImpl.h \
	XML.cpp
