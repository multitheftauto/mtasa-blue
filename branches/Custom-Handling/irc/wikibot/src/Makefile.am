bin_PROGRAMS=wikibot wikibot_mkpasswd

wikibot_SOURCES=wikibot.cpp keys.cpp configuration.cpp main.cpp syntax.c
wikibot_LDADD=-lrsl -lpthread

wikibot_mkpasswd_SOURCES=mkpasswd.cpp keys.cpp

AM_CPPFLAGS=-I. -I.. -I../include -pthread -pipe -DDATADIR='"$(datadir)"'

syntax.c: syntax.l
	$(LEX) -o $@ $<

clean-local:
	$(RM) -f syntax.c
