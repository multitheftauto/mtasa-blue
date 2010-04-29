INCLUDES += -I$(srcdir)/../sdk/
INCLUDES += -I$(srcdir)/../../vendor/tinyxml/

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
	StdInc.cpp \
	StdInc.h \
	XML.cpp
