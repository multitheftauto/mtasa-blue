lib_LTLIBRARIES=librsl_net_socket.la
librsl_net_socket_la_CPPFLAGS = -I$(top_srcdir)/..
librsl_net_socket_la_SOURCES=client.cpp ip.cpp ipv4.cpp ipv6.cpp server.cpp \
                             tcpclient.cpp tcpserver.cpp udpclient.cpp udpserver.cpp \
														 socketstream.cpp tcpstream.cpp udpstream.cpp \
														 tcpclientssl.cpp tcpstreamssl.cpp

if RSL_USE_EPOLL
select_epoll.cpp: select.h
select.h: select.h.in
	sed -e 's#@select_header@#select_epoll.h#' < $< > $@

librsl_net_socket_la_SOURCES+=select_epoll.cpp
endif

if RSL_USE_POLL
select.h: select.h.in
	sed -e 's#@select_header@#select_poll.h#' < $< > $@

librsl_net_socket_la_SOURCES+=select_poll.cpp
endif

if RSL_USE_SELECT
select.h: select.h.in
	sed -e 's#@select_header@#select_select.h#' < $< > $@

librsl_net_socket_la_SOURCES+=select_select.cpp
endif

librsl_net_socket_la_LIBADD=-lssl
