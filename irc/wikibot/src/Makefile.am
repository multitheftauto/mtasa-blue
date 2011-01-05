bin_PROGRAMS=wikibot wikibot_mkpasswd

wikibot_SOURCES=wikibot.cpp keys.cpp configuration.cpp main.cpp syntax.c
wikibot_LDADD=$(top_builddir)/../rsl/librsl.la -lpthread

wikibot_mkpasswd_SOURCES=mkpasswd.cpp keys.cpp

AM_CPPFLAGS=-I$(top_srcdir)/include -I$(top_srcdir)/.. -pthread -DDATADIR='"$(datadir)"'

syntax.c: syntax.l
	$(LEX) -o $@ $<

clean-local:
	$(RM) -f syntax.c
