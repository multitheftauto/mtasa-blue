//////////////////////////////////////////////////////////////////////////
//
// tags.cpp - library exports implementation
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
//
// (internal)
// 

#include <memory>
#include <windows.h>
#include "tags.h"
#include "tags_impl.h"
#include "keywords.h"

int g_utf8;
std::string g_err;
std::string g_cache;

const char*  _stdcall TAGS_GetLastErrorDesc()
{
	return g_err.c_str();
}


const char*  _stdcall TAGS_Read( DWORD dwHandle, const char* fmt )
{
	tag_readers reader( dwHandle );
	if( reader[0] )
	{
		std::string str(fmt);
		std::string::const_iterator beg = str.begin();
		g_cache = parser::expr( beg, str.end(), reader );
		return g_cache.c_str();
	}
	return TAGS_MAIN_ERROR_RETURN_VALUE;
}

DWORD _stdcall TAGS_GetVersion()
{
	return MAKEWORD(0,TAGS_VERSION);
}

