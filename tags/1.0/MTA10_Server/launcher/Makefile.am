#INCLUDES += -I$(srcdir)/logic/
#INCLUDES += -I$(srcdir)/../../sdk/
#INCLUDES += -I$(srcdir)/../../../Shared/sdk/
#INCLUDES += -I$(srcdir)/../../../Shared/publicsdk/include/
#INCLUDES += -I$(srcdir)/../../../vendor/
#INCLUDES += -I$(srcdir)/../../../vendor/pme/
#INCLUDES += -I$(srcdir)/../../../vendor/json-c/

bin_PROGRAMS = mta-server

mta_server_SOURCES = \
	CDynamicLibrary.cpp \
	Main.cpp


mta_server_LDADD = -ldl -lz -lcurl
	
