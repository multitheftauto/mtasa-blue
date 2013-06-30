//////////////////////////////////////////////////////////////////////////
//
// id3v2.cpp - ID3v2.2.0 tag reader implementation
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// Modified to support ID3v2.2 by Dylan Fitterer
// 
// (internal)
//

#include <vector>
#include <assert.h>
#include <algorithm>
#include "tags_impl.h"
#include "keywords.h"
#include "id3_genres.h"
#include "rselect.h"
//#include <iostream>

////////////////////////////// mp3 id3v2.2 tag ////////////////////////////

namespace{
//	typedef unsigned long DWORD;

//	DWORD c_id3v2_size_limit = 1000000;
 
// this one's good... 
unsigned int weird2size( const unsigned char weird[4] )
{
	return	(weird[3]&127) + 
		128*(weird[2]&127) + 
	128*128*(weird[1]&127) + 
128*128*128*(weird[0]&127);
}

unsigned int weird3size(const unsigned char weird[3]){
//	int frame_size_long = weird[0] << 8;
//    frame_size_long = (frame_size_long | weird[1]) << 8;
//    frame_size_long |= weird[2];
//	return frame_size_long;
	return weird[2] + 
		256*weird[1] + 
	256*256*weird[0];
}

#include <pshpack1.h>
struct id3v22_frame;

struct id3v22_header
{
	char magic[3];	// always "ID3"
	unsigned char version_major;
	unsigned char version_minor;
	unsigned char flags;
	unsigned char weird_size[4];

	unsigned long getsize() const { return weird2size( weird_size ); }

	const id3v22_frame* get_frames() const
	{ return reinterpret_cast<const id3v22_frame*>( 
		reinterpret_cast<const char*>(this + 1)
		);
	} 

	void check() const 
	{ 
		if( 
			((*(DWORD*)magic)&0xFFFFFF) != 0x334449 
//			|| getsize() >= c_id3v22_size_limit
			|| version_major != 2
//			|| version_minor != 0
			|| flags&0x80
			) 
//		throw bad_tag("ID3v2_2 tag: header is corrupted.");
		throw no_tag();
	}
};

struct id3v22_frame
{
	char id[3]; // frame id, eg. "TT2" stands for song name, etc.
	unsigned char value_size[3];	// size of the tag value
//	unsigned short flags;	// flags
	char value[1];

	unsigned int vsize() const{
		return weird3size( value_size );
	}
	const id3v22_frame* next() const{
		return reinterpret_cast<const id3v22_frame*>( value + vsize() );
	} // weird...
};
#include <poppack.h>

// field parsers
typedef std::string (*pfn_frame_parser)( const id3v22_frame* frame, bool utf8 );

void byte_swap( wchar_t& x )
{
	x=(x<<8)|(x>>8);
}

// removes 0xD symbols from string
void purge_crlf( std::string& s )
{
	s.erase( std::remove( s.begin(), s.end(), 0xD ), s.end() ); 
}

// well, unicode happens...
// anyway, this retrieves a unicode string from a memory block...
std::string get_uncode_string( const char* b, const char* e, bool utf8 )
{
	const wchar_t* p = reinterpret_cast<const wchar_t*>( b ), 
		*q = reinterpret_cast< const wchar_t*>( e );

	if( *p != 0xFFFe && *p != 0xfeff || b>=e)
		return ""; // may opt for throwing

	std::vector< wchar_t > tmp( p, q );
	tmp.push_back(0);
	p = &tmp[0]; q = &tmp.back()+1;

	if( *p == 0xFFFE )
		std::for_each( tmp.begin(), tmp.end(), &byte_swap );

	assert( *p == 0xFEFF ); // if this happens, call for a computer psychiatrist

	return parser::unicode_string( &p[1], utf8 );
}

// why so many options in the standard?? one UNICODE encoding would have been quite enough
std::string text( const id3v22_frame* frame, bool utf8 )
{
	switch( frame->value[0] )
	{
	case 0:
		return std::string( frame->value+1, frame->value + frame->vsize());
	case 1: return get_uncode_string( frame->value +1, frame->value + frame->vsize(), utf8 );

#if TAGS_ID3V2_FACIST == 0
		// maybe some stupid tagger wrote the frame value just so, without any encoding sign...
	default: return std::string( frame->value, frame->value + frame->vsize());
#endif
	}
	return ""; // bogus
}

// as per http://www.id3.org/id3v2.3.0.html#TCON
//
// a person who invented that piece of id3v2 standard should be hanged, drawn and quartered
// for more information refer to http://en.wikipedia.org/wiki/Drawing_and_quartering

std::string genre( const id3v22_frame* frame, bool utf8 )
{
	std::string str = text( frame, utf8 );
	if( str.empty() )
		return str;

	typedef std::string::const_iterator sir;
	const char* p = str.c_str();
	const char* e = p + str.length();

	std::string ret;

	while( p != e && p[0] == '(' )
	{
		++p;
		if( !ret.empty() )
			ret += TAGS_ID3V2_GENRE_REFINEMENT_SEPARATOR;

		if( p>=e )
			return ""; // bogus

		if( *p == '(' )
		{
			return std::string( p,e ); // just text in parentheses
		}else
		if( p+2<e && p[0] =='R' && p[1] == 'X' && p[2] == ')' )
		{
			ret = "Remix";
			p+=3;
		}else
		if( p+2<e && p[0] =='C' && p[1] == 'R' && p[2] == ')' )
		{
			ret = "Cover";
			p+=3;
		}else
		if( isdigit(*p) )
		{
			size_t n = 0;
			for( ; isdigit(*p) && p<e; ++p )	n = n*10 + (*p - '0');
#if TAGS_ID3V2_FACIST == 1
			if( *p != ')' )
				return ""; // bogus
#endif
			++p;
			if( n>=g_id3_genre_count )
				return ""; // bogus
			ret = g_id3_genres[ n ];
		}else // a non-digit inside un-escaped parentheses 
#if TAGS_ID3V2_FACIST == 1
			return ""; // bogus
#else 
			return str; // winamp does this kind of sh*t, so let's pretend this is acceptable.
#endif
	}
	if (p!=e) {
		if( !ret.empty() )
			ret += TAGS_ID3V2_GENRE_REFINEMENT_SEPARATOR; 
		ret.append( p, e );
	}
	return ret;
}

std::string comment( const id3v22_frame* frame, bool utf8 )
{
	const char* p = frame->value + 1;
	const char* e = frame->value + frame->vsize();

	char lang[4] = {0,};
	if( p+3>=e )
		return "";

	// do something with the lang ID? maybe set a locale? not here though.
	p+=3;

	std::string short_str, long_str;

	if( frame->value[0] ==  1 )
	{
		short_str = get_uncode_string( p, e, utf8 );
		while (p<e && *(short*)p) p+=2;
		p+=2;
		if( p>=e )
#if TAGS_ID3V2_FACIST == 1
			return ""; // bogus
#else
			return short_str;
#endif
		long_str = get_uncode_string( p, e, utf8 );
	}else
#if TAGS_ID3V2_FACIST == 1
		if( frame->value[0] == 0) // enforce encoding or merely assume otherwise it's iso.
#endif
		{
			std::string short_str( p );
			while (p<e && *p) p++;
			p++;
			if( p>=e )
#if TAGS_ID3V2_FACIST == 1
				return ""; // bogus
#else
				return short_str;
#endif
			long_str = std::string( p, e );
		}
#if TAGS_ID3V2_FACIST == 1
		else return ""; // unknown encoding - bogus
#endif

	purge_crlf( short_str );
	purge_crlf( long_str );
	
#define S short_str
#define L long_str
	return TAGS_ID3V2_COMMENT_RETURNS;
#undef S
#undef L

}

struct field
{ 
	const char* mapped;
	pfn_frame_parser pfn;
	field( const char* x = 0, pfn_frame_parser fn = 0 ):mapped(x), pfn(fn) {}
};

typedef tools::simplemap< const char*, field, tools::cmp_sz >::pair_type vpair;
const vpair remap_data[] = 
{
	vpair("TITL", field("TT2", text)),
	vpair("ARTI", field("TP1", text)),
	vpair("ALBM", field("TAL", text)),
	vpair("GNRE", field("TCO", genre)), 
	vpair("YEAR", field("TYE", text)),
	vpair("CMNT", field("COM", comment)),
	vpair("TRCK", field("TRK", text)),
	vpair("COMP", field("TCM", text)),
	vpair("COPY", field("TCR", text)),
	vpair("SUBT", field("TT3", text)),
	vpair("AART", field("TP2", text)),
	vpair("DISC", field("TPA", text)),
};

class id3v22_reader:public tag_reader
{
	typedef tools::simplemap< const char*, field, tools::cmp_sz > map_t;

	const id3v22_header* m_header;
	map_t m_remap;

public:
	typedef map_t::pair_type vpair;

	id3v22_reader( DWORD handle ): m_header( 0 ), m_remap( remap_data,END_OF_ARRAY(remap_data) )
	{
		m_header = reinterpret_cast< const id3v22_header* >( BASS_ChannelGetTags( handle, BASS_TAG_ID3V2 ) );
		if( !m_header )
			throw no_tag(); // not supported
		m_header->check();
	}

	std::string operator[] (const char* name ) const
	{
		std::string ret;

		field f = m_remap[ name ];
		if( !f.mapped )
			throw bad_ident();

		if( const id3v22_frame* frame = find_frame( f.mapped ) )
		{
			if( f.pfn )
				ret = f.pfn( frame, !!g_utf8 );

			if( !ret.empty() )
				return ret;
		}

		return "";
	}

	const id3v22_frame* find_frame( const char* name ) const
	{
		const char* limit = reinterpret_cast<const char*>(m_header->get_frames()) + m_header->getsize();

		for( const id3v22_frame* p = m_header->get_frames(); 
			reinterpret_cast< const char* >( p ) < limit; 
			p = p->next() )
		{
			// frame sanity check
			unsigned int vsz = p->vsize();
			if( !vsz || vsz > limit - p->value )
				return 0; // bogus
			
			if( *(DWORD*) name == *(DWORD*) p->id )
				return p;
		}
        return 0;
	}

};

} // anon namespace


tag_reader* reader_id3v22( DWORD handle )
{
	return new id3v22_reader( handle );
}