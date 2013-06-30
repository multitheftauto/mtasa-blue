//////////////////////////////////////////////////////////////////////////
//
// tags_impl.h - defines a common interface for tag readers.
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// Abstract:	The library consists of several tag readers. This interface
//				is a uniform way to access them. 
//
// (internal)

#ifndef TAG_IMPL_H
#define TAG_IMPL_H

#include <string>
#include <vector>
#include "../../../bass/bass.h"
#include "config.h"
#include "simplemap.h"

#define BASS_TAG_APE		6	// APE tags
#define BASS_TAG_MP4		7	// MP4 tags
#define BASS_TAG_WMA		8	// WMA tags

// tag reader is unable to find tag information of its type
struct no_tag{};

// bad tag data (tag type/reader mismatch?)
struct bad_tag: std::runtime_error
{
	bad_tag( const std::string& s ): std::runtime_error( s ){}
};

// exception thrown by tag_reader::operator[] when tag identifier is not in the defined set
struct bad_ident{};

struct tag_reader
{
	virtual std::string operator[] ( const char* name ) const = 0;
	virtual ~tag_reader() {}
};

class tag_readers
{
	std::vector<tag_reader*> m_readers;

public:
	tag_readers(DWORD handle);
	~tag_readers()
	{
		for (std::vector<tag_reader*>::iterator i=m_readers.begin(); i!=m_readers.end(); ++i)
			delete *i;
	}
	tag_reader* operator[] (int i) { return (i<m_readers.size()?m_readers[i]:0); }
};


extern int g_utf8;
extern std::string g_err;
extern std::string g_cache;

#endif
