INCLUDES += -I$(srcdir)/../sdk/
INCLUDES += -I$(srcdir)/../../vendor/tinyxml/

if PRECOMPILE_HEADERS
BUILT_SOURCES = StdInc.h.gch
AM_CXXFLAGS += -Winvalid-pch

StdInc.h.gch:
	$(LIBTOOL) --tag=CXX $(xmll_la_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile \
		$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
		$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)  \
		-o xmll_la-StdInc.h.lo $(srcdir)/StdInc.h \
		-MT xmll_la-StdInc.h.gch -MD -MP -MF $(DEPDIR)/xmll_la-StdInc.h.Tpo

	mv -f $(DEPDIR)/xmll_la-StdInc.h.Tpo $(DEPDIR)/xmll_la-StdInc.h.Plo
	mv -f .libs/xmll_la-StdInc.h.o $@

include ./$(DEPDIR)/xmll_la-StdInc.h.Plo

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
