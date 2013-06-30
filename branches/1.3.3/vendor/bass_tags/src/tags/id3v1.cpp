//////////////////////////////////////////////////////////////////////////
//
// id3v1.cpp - ID3v1 tag reader implementation
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// (internal)
//

#include "tags_impl.h"
#include "id3_genres.h"

namespace{

typedef __int64 longlong;

#include <pshpack1.h>
struct id3v1_tag
{
	char magic[3]; // always "TAG"
	char title[30];
	char artist[30];
	char album[30];
	char year[4];
	char comment[28];
	unsigned char zero;
	unsigned char track; // track #
	unsigned char genre; // genre

	void check() const { if( magic[0]!='T' || magic[1]!='A' || magic[2]!='G' ) 
		throw bad_tag( "ID3v1 tag: invalid header" );
	}
};
#include <poppack.h>

#ifndef offsetof
#define offsetof( struc, member )\
	(  (int) &( ((struc*)0)->member) )
#endif

// field parsing function
typedef std::string (*fn_field_parser)( const id3v1_tag* tag, const char* field, int sz );

// parsers

// simple text
std::string text( const id3v1_tag* tag, const char* field, int len )
{
	const char* p = field;
	for( ; *p && p-field<len; ++p );
#ifdef TAGS_ID3V1_REMOVE_STUPID_PADDING
	while( ' '==*--p && p>=field ); // remove padding
	++p;
#endif
	return std::string( field, p );
}

// comment parser: ID3v1 and ID3v1.1
std::string comment( const id3v1_tag* tag, const char* field, int len )
{
	return text( tag, field, tag->zero ? 30 : 28 );
}

// number
std::string number( const id3v1_tag* tag, const char* field, int len )
{
	longlong v = 0;
	for(int i=0; i<len; ++i )
		v += longlong(field[i])<<(i*8);
	char sz[32];
	sprintf(sz,"%I64d",v);
	return sz;
}

std::string track( const id3v1_tag* tag, const char* field, int len )
{
	return !tag->zero && tag->track ? number( tag, field, len ) : "";
}
// genre
std::string genre( const id3v1_tag* tag, const char* field, int len )
{
	size_t i = *(unsigned char*)field; // genre id
	if( i>=g_id3_genre_count )
		return "";
	return g_id3_genres[ i ];
}



const
struct
{
	const char* name; // component name, eg. "Title"
	int offs;	// offset from 
	int sz;		// field size
	fn_field_parser pfn;
} remap[] = 
{
	{ "TITL",	offsetof(id3v1_tag,title),	30,	&text },
	{ "ARTI",	offsetof(id3v1_tag,artist),	30,	&text },
	{ "ALBM",	offsetof(id3v1_tag,album),	30,	&text },
	{ "YEAR",	offsetof(id3v1_tag,year),	4,	&text },
	{ "CMNT",	offsetof(id3v1_tag,comment),28,	&comment },
	{ "TRCK",	offsetof(id3v1_tag,track),	1,	&track},
	{ "GNRE",	offsetof(id3v1_tag,genre),	1,	&genre}
};
const int remap_sz = SIZE_OF_ARRAY(remap);

class id3v1_reader: public tag_reader
{
	const id3v1_tag* m_header;

public:
	id3v1_reader( DWORD handle ): m_header( 0 )
	{
		m_header = (const id3v1_tag*) BASS_ChannelGetTags( handle, BASS_TAG_ID3 );
		if( !m_header )
			throw no_tag();
		m_header->check();
	}

	std::string operator[]( const char* name ) const
	{
		for( int i=0; i<remap_sz; ++i )
		{
			if( !strcmp(name, remap[i].name) )
			{
				return 
					remap[i].pfn ?
					remap[i].pfn( m_header, (const char*)m_header + remap[i].offs, remap[i].sz )
					: "";
			}
		}
		throw bad_ident();
	}
};

} // anon namespace

tag_reader* reader_id3v1( DWORD handle )
{
	return new id3v1_reader( handle );
}
