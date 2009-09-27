AM_CFLAGS = -Wall -std=gnu99 -D_GNU_SOURCE -D_REENTRANT

noinst_LTLIBRARIES = libjson.la

libjsonincludedir = $(includedir)/json
libjsoninclude_HEADERS = \
        json.h \
	bits.h \
	debug.h \
	linkhash.h \
	arraylist.h \
	json_util.h \
	json_object.h \
	json_tokener.h

libjson_la_LDFLAGS = -version-info 0:1:0

libjson_la_SOURCES = \
	arraylist.c \
	debug.c \
	json_object.c \
	json_tokener.c \
	json_util.c \
	linkhash.c \
	printbuf.c
