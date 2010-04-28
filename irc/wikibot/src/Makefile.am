bin_PROGRAMS=wikibot wikibot_mkpasswd

wikibot_SOURCES=wikibot.cpp keys.cpp configuration.cpp main.cpp
wikibot_LDADD=-lrsl -lpthread

wikibot_mkpasswd_SOURCES=mkpasswd.cpp keys.cpp

AM_CPPFLAGS=-I. -I.. -I../include -pthread -pipe
