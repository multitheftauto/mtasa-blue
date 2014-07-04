//////////////////////////////////////////////////////////////////////////
//
// mod.cpp - MOD music tag reader implementation
//
// Public domain. No warranty.
// 
// (internal)
//

#include "tags_impl.h"

namespace{

class mod_reader: public tag_reader
{
	const char* m_name;

public:
	mod_reader( DWORD handle ): m_name( 0 )
	{
		m_name = BASS_ChannelGetTags( handle, BASS_TAG_MUSIC_NAME );
		if( !m_name )
			throw no_tag();
	}

	std::string operator[]( const char* name ) const
	{
		if( !strcmp(name, "TITL") )
		{
			return m_name;
		}
		throw bad_ident();
	}
};

} // anon namespace

tag_reader* reader_mod( DWORD handle )
{
	return new mod_reader( handle );
}
