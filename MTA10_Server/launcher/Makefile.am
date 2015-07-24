
bindir = $(outputdir)
bin_PROGRAMS = mta-server
mta_server_SOURCES = \
	CDynamicLibrary.cpp \
	Main.cpp

INCLUDES += -I$(srcdir)/../../Shared/sdk/
INCLUDES += -I$(srcdir)/../../MTA10_Server/sdk/

mta_server_LDADD = -ldl -lz
