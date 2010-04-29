bin_PROGRAMS=mantisbot mantisbot_mkpasswd

mantisbot_SOURCES=commands.cpp mantisbot.cpp keys.cpp configuration.cpp main.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp tinystr.cpp
mantisbot_LDADD=-lrsl -lpthread

mantisbot_mkpasswd_SOURCES=mkpasswd.cpp keys.cpp

AM_CPPFLAGS=-I. -I.. -I../include -pthread -pipe -DTINYXML_USE_STL
