INCLUDES += -I$(srcdir)/../core/
INCLUDES += -I$(srcdir)/../sdk/
INCLUDES += -I$(srcdir)/../../Shared/sdk/
INCLUDES += $(CURSES_CFLAGS)

noinst_LTLIBRARIES = \
	libplatform.la

libplatform_la_SOURCES = \
	MTAPlatform.cpp \
	MTAPlatform.h

libplatform_la_LIBADD = \
	$(CURSES_LIBS)

