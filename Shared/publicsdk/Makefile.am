noinst_LTLIBRARIES = libml_base.la

libml_base_la_SOURCES = \
	CFunctions.cpp \
	CThread.cpp \
	CThreadData.cpp \
	ml_base.cpp \
	extra/CLuaArgument.cpp \
	extra/CLuaArguments.cpp

#TODO:
# the windows project uses the internal LUA, why?

libml_base_la_CXXFLAGS = \
	-I$(top_srcdir)/vendor/lua5.4/src/ \
	-I$(top_builddir)/vendor/lua5.4/src/

libml_base_la_LIBADD = \
	$(top_builddir)/vendor/lua5.4/src/liblua.la
