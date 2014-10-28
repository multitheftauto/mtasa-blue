//////////////////////////////////////////////////////////////////////////
//
// tag_preproc.h - defines a preporocessed tag reader.
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// Abstract:	Several tag types (BASS_TAG_OGG, BASS_TAG_WMA, BASS_TAG_APE)
//				come already preprocessed into the "tag\0\value\0..." pattern.
//				The only difference is the identifier=>field name remap.
//
// (internal)

#ifndef TAG_PREPROC_H
#define TAG_PREPROC_H

#include "tags_impl.h"
#include "simplemap.h"
#include <cstdlib>

namespace
{
	class preproc_reader: public tag_reader
	{
		const char* m_start;
		typedef tools::simplemap< const char*, const char*, tools::cmp_sz > map_t;
		map_t m_remap;
		char m_sep;
		char m_eol;
	public:
		typedef map_t::pair_type vpair;

		preproc_reader( const char* data, const char* /* bad_diag */, const vpair* remap, int cnt, char separator = '=', char eol = 0 )
			:m_start( 0 ), m_remap( remap, remap + cnt ), m_sep( separator ), m_eol( eol )
		{
			m_start = data;
			if( !m_start || !*m_start )
				throw no_tag();
//			if(!*m_start)
//				throw bad_tag( bad_diag );
		}
		std::string operator[]( const char* name ) const
		{
			return findElement( name );
		}
	protected:
		const char* findElement( const char* tag ) const
		{
			tag = m_remap[tag];
			if( !tag )
				throw bad_ident();

			return findTag(tag);
		}
		const char* findTag( const char* tag ) const
		{
			const char* start = m_start;
			while(*start)
			{
				const char* q = tag;
				do {
					const char* p = start;
					while( (*q && *q != '|') && *p && tolower(*q++) == tolower(*p++) )
						;

					if( (!*q || *q == '|') && *p == m_sep )
					{
						return p+1;
					}
					while (*q && *q != '|') q++;
				} while (*q++);
				while( *start++ != m_eol );
			}
			return "";
		}

	};
} // anon namespace

#endif

