//////////////////////////////////////////////////////////////////////////
//
// id3v2.cpp - ID3v2.3.0 tag reader implementation
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// (internal)
//

#include <vector>
#include <assert.h>
#include <algorithm>
#include <malloc.h>
#include "tags_impl.h"
#include "keywords.h"
#include "id3_genres.h"
#include "rselect.h"

////////////////////////////// mp3 id3v2.3 tag ////////////////////////////

namespace{
//	typedef unsigned long DWORD;

//	DWORD c_id3v2_size_limit = 1000000;
 
// this one's good... 
unsigned int headsize( const unsigned char size[4] )
{
	return	size[3] + 
		128*size[2] + 
	128*128*size[1] + 
128*128*128*size[0];
}

unsigned int framesize( const unsigned char size[4] )
{
	return (size[0]<<24)|(size[1]<<16)|(size[2]<<8)|size[3];
}

unsigned int frame2size(const unsigned char size[3])
{
	return (size[0]<<16)|(size[1]<<8)|size[2];
}

#include <pshpack1.h>
struct id3v2_frame;

struct id3v2_header
{
	char magic[3];	// always "ID3"
	unsigned char version_major;
	unsigned char version_minor;
	unsigned char flags;
	unsigned char weird_size[4];

	unsigned long getsize() const { return headsize( weird_size ); }
	bool has_ext_header() const { return (flags & 64) != 0; }

	unsigned long get_ext_header_size() const 
	{ return has_ext_header() ? headsize(reinterpret_cast<const unsigned char*>(this + 1 )) : 0; }

	const id3v2_frame* get_frames() const
	{ return reinterpret_cast<const id3v2_frame*>( 
		reinterpret_cast<const char*>(this + 1) + get_ext_header_size()
		);
	} 

	void check() const 
	{ 
		if( 
			((*(DWORD*)magic)&0xFFFFFF) != 0x334449 
//			|| getsize() >= c_id3v2_size_limit
			|| (version_major != 3 && version_major != 4)
//			|| version_minor != 0
			) 
		throw bad_tag("ID3v2 tag: header is corrupted.");
	}
};

struct id3v2_frame
{
	char id[4]; // frame id, eg. "TIT2" stands for song name, etc.
	unsigned char value_size[4];	// size of the tag value
	unsigned short flags;	// flags
	char value[1];

	unsigned int vsize(const id3v2_header *header) const { return (header->version_major==4?headsize:framesize)( value_size ); }
	const id3v2_frame* next(const id3v2_header *header) const 
	{ return reinterpret_cast<const id3v2_frame*>( value + vsize(header) ); } // weird...
};
#include <poppack.h>

// field parsers
typedef std::string (*pfn_frame_parser)( const id3v2_frame* frame, const id3v2_header *header, bool utf8 );

void byte_swap( wchar_t& x )
{
	x=(x<<8)|(x>>8);
}

// removes 0xD symbols from string
void purge_crlf( std::string& s )
{
	s.erase( std::remove( s.begin(), s.end(), 0xD ), s.end() ); 
}

// reverses "unsynchronisation"
unsigned long deunsync(const char *src, unsigned long len, char *dst)
{
	const char *s=src,*e=src+len;
	char *d=dst;
	while (s<e) {
		*d++=*s;
		if (s[0]==-1 && !s[1]) s++;
		s++;
	}
	return d-dst;
}

// well, unicode happens...
// anyway, this retrieves a unicode string from a memory block...
std::string get_uncode_string( const char* b, const char* e, bool utf8, bool bigend=false )
{
	if ((e-b)&1) return ""; // should have even length

	const wchar_t* p = reinterpret_cast<const wchar_t*>( b ), 
		*q = reinterpret_cast< const wchar_t*>( e );

	if(b>=e)
		return ""; // may opt for throwing

	std::vector< wchar_t > tmp( p, q );
	tmp.push_back(0);
	p = &tmp[0]; q = &tmp.back()+1;

	if (*p == 0xFFFE || bigend)
		std::for_each( tmp.begin(), tmp.end(), &byte_swap );

	if (*p == 0xFEFF ) p++;

	return parser::unicode_string( &p[0], utf8 );
}

// why so many options in the standard?? one UNICODE encoding would have been quite enough
std::string text( const id3v2_frame* frame, const id3v2_header *header, bool utf8 )
{
	const char* p = frame->value;
	const char* e = frame->value + frame->vsize(header);
	if (header->version_major==4) {
		if (frame->flags&0x100) p+=4;
		if (frame->flags&0x200) {
			char *b=(char*)alloca(e-p);
			unsigned long len=deunsync(p, e-p, b);
			p=b;
			e=p+len;
		}
	}

	int enc=*p++;
	switch( enc )
	{
	case 3: // UTF-8
	case 0:	return std::string( p, e );
	case 2: // BOM-less (BE) unicode
	case 1: return get_uncode_string( p, e, utf8, enc==2 );

#if TAGS_ID3V2_FACIST == 0
		// maybe some stupid tagger wrote the frame value just so, without any encoding sign...
	default: return std::string( frame->value, frame->value + frame->vsize(header) );
#endif
	}
	return ""; // bogus
}

// as per http://www.id3.org/id3v2.3.0.html#TCON
//
// a person who invented that piece of id3v2 standard should be hanged, drawn and quartered
// for more information refer to http://en.wikipedia.org/wiki/Drawing_and_quartering
std::string genre( const id3v2_frame* frame, const id3v2_header *header, bool utf8 )
{
	std::string str = text( frame, header, utf8 );
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

std::string comment( const id3v2_frame* frame, const id3v2_header *header, bool utf8 )
{
	const char* p = frame->value;
	const char* e = frame->value + frame->vsize(header);
	if (header->version_major==4) {
		if (frame->flags&0x100) p+=4;
		if (frame->flags&0x200) {
			char *b=(char*)alloca(e-p);
			unsigned long len=deunsync(p, e-p, b);
			p=b;
			e=p+len;
		}
	}

	int enc=*p++;

	char lang[4] = {0,};
	if( p+3>=e )
		return "";

	// do something with the lang ID? maybe set a locale? not here though.
	p+=3;

	std::string short_str, long_str;

	if( enc == 1 || enc == 2 )
	{
		short_str = get_uncode_string( p, e, utf8, enc == 2 );
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
		if( enc == 0) // enforce encoding or merely assume otherwise it's iso.
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
	vpair("TITL", field("TIT2", text)),
	vpair("ARTI", field("TPE1", text)),
	vpair("ALBM", field("TALB", text)),
	vpair("GNRE", field("TCON", genre)), 
	vpair("YEAR", field("TYER", text)),
	vpair("CMNT", field("COMM", comment)),
	vpair("TRCK", field("TRCK", text)),
	vpair("COMP", field("TCOM", text)),
	vpair("COPY", field("TCOP", text)),
	vpair("SUBT", field("TIT3", text)),
	vpair("AART", field("TPE2", text)),
	vpair("DISC", field("TPOS", text)),
};

class id3v2_reader:public tag_reader
{
	typedef tools::simplemap< const char*, field, tools::cmp_sz > map_t;

	const id3v2_header* m_header;
	map_t m_remap;
	unsigned long tagsize;
	bool unsynced;

public:
	typedef map_t::pair_type vpair;

	id3v2_reader( DWORD handle ): m_header( 0 ), m_remap( remap_data,END_OF_ARRAY(remap_data) ), unsynced( false )
	{
		m_header = reinterpret_cast< const id3v2_header* >( BASS_ChannelGetTags( handle, BASS_TAG_ID3V2 ) );
		if( !m_header )
			throw no_tag(); // not supported
		m_header->check();

		tagsize=m_header->getsize();
		if (m_header->version_major==3 && m_header->flags&0x80) {
			id3v2_header* newhead=reinterpret_cast<id3v2_header*>(::operator new(sizeof(id3v2_header)+tagsize));
			*newhead=*m_header;
			tagsize=deunsync((char*)(m_header+1), tagsize, (char*)(newhead+1));
			m_header=newhead;
			unsynced=true;
		}
	}

	~id3v2_reader()
	{
		if (unsynced) ::operator delete((void*)m_header);
	}

	std::string operator[] (const char* name ) const
	{
		std::string ret;

		field f = m_remap[ name ];
		if( !f.mapped )
			throw bad_ident();
		if (m_header->version_major>3 && !strcmp(name, "YEAR"))
			f=field("TDRC", text);

		if( const id3v2_frame* frame = find_frame( f.mapped ) )
		{
			if( f.pfn )
				ret = f.pfn( frame, m_header, !!g_utf8 );

			if( !ret.empty() )
				return ret;
		}

		return "";
	}

	const id3v2_frame* find_frame( const char* name ) const
	{
		const char* limit = reinterpret_cast<const char*>(m_header + 1) + tagsize;

		for( const id3v2_frame* p = m_header->get_frames(); 
			reinterpret_cast< const char* >( p ) < limit; 
			p = p->next(m_header) )
		{
			// frame sanity check
			unsigned int vsz = p->vsize(m_header);
			if( !vsz || vsz > limit - p->value )
				return 0; // bogus

			// ignore compressed/encrypted frames
			if (m_header->version_major==3) {
				if (p->flags&0xc000) continue;
			} else {
				if (p->flags&0xc00) continue;
			}

			if( *(DWORD*) name == *(DWORD*) p->id )
				return p;
		}
        return 0;
	}

};

} // anon namespace


tag_reader* reader_id3v2( DWORD handle )
{
	return new id3v2_reader( handle );
}
