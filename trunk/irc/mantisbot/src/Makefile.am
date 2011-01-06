bin_PROGRAMS=mantisbot mantisbot_mkpasswd

mantisbot_SOURCES=commands.cpp mantisbot.cpp keys.cpp configuration.cpp main.cpp
mantisbot_LDADD=$(top_builddir)/../rsl/librsl.la -lpthread
mantisbot_mkpasswd_SOURCES=mkpasswd.cpp keys.cpp

AM_CPPFLAGS=-I$(top_srcdir)/include -I$(top_srcdir)/.. -I$(top_builddir)/.. -pthread

mantisbot_LDADD += $(top_builddir)/../../vendor/tinyxml/libtinyxml.la
AM_CPPFLAGS+=-I$(top_srcdir)/../../vendor/tinyxml/ -DTINYXML_USE_STL
