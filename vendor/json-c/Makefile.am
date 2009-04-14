AM_CFLAGS = -Wall -std=gnu99 -D_GNU_SOURCE -D_REENTRANT

noinst_LTLIBRARIES = libjson-c.la

#pkgconfigdir = $(libdir)/pkgconfig
#pkgconfig_DATA = json.pc

#libjson_cincludedir = $(includedir)/json
#libjson_cinclude_HEADERS = \
#        json.h \
#	bits.h \
#	debug.h \
#	linkhash.h \
#	arraylist.h \
#	json_util.h \
#	json_object.h \
#	json_tokener.h

libjson_c_la_LDFLAGS = -version-info 0:1:0

libjson_c_la_SOURCES = \
	arraylist.c \
	debug.c \
	json_object.c \
	json_tokener.c \
	json_util.c \
	linkhash.c \
	printbuf.c

#check_PROGRAMS = test1 test2

#test1_SOURCES = test1.c
#test1_LDADD = $(lib_LTLIBRARIES)

#test2_SOURCES = test2.c
#test2_LDADD = $(lib_LTLIBRARIES)
