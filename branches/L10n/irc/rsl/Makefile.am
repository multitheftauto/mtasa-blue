SUBDIRS=file net stream
lib_LTLIBRARIES=librsl.la
librsl_la_SOURCES=
librsl_la_LIBADD=file/librsl_file.la net/librsl_net.la
nobase_pkginclude_HEADERS=config.h \
				                   file/ini/iniparser.h \
				                   net/http/client.h net/http/stream.h \
				                   net/irc/client.h net/irc/irc.h net/irc/message.h \
				                   net/irc/text.h net/irc/user.h \
				                   net/socket/client.h net/socket/ip.h net/socket/ipv4.h net/socket/ipv6.h \
				                   net/socket/select_epoll.h net/socket/select_poll.h \
				                   net/socket/select_select.h net/socket/select.h \
				                   net/socket/server.h net/socket/socketstream.h net/socket/tcpclient.h net/socket/tcpclientssl.h \
				                   net/socket/tcpserver.h net/socket/tcpstream.h net/socket/tcpstreamssl.h \
				                   net/socket/udpclient.h net/socket/udpserver.h net/socket/udpstream.h \
						   stream/stream.h

#include doxygen.am

#dist_man1_MANS = doxygen-doc

#MOSTLYCLEANFILES = $(DX_CLEANFILES)

