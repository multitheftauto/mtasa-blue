//////////////////////////////////////////////////////////////////////////
//
// ogg.cpp - ogg tag reader implementation
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// (internal)
//

#include "tag_preproc.h"

namespace{

	typedef preproc_reader::vpair vpair;
	vpair remap[] = 
	{
		vpair("TITL", "TITLE"),
		vpair("ARTI", "ARTIST"),
		vpair("ALBM", "ALBUM"),
		vpair("GNRE", "GENRE"),
		vpair("YEAR", "DATE"),
		vpair("CMNT", "COMMENT"),
		vpair("TRCK", "TRACKNUMBER"),
		vpair("COMP", "COMPOSER"),
		vpair("COPY", "COPYRIGHT"),
		vpair("SUBT", "SUBTITLE"),
		vpair("AART", "ALBUMARTIST|ALBUM ARTIST"),
		vpair("DISC", "DISCNUMBER"),
	};

	struct reader: public preproc_reader
	{
		reader( DWORD handle ): preproc_reader(
			BASS_ChannelGetTags( handle, BASS_TAG_OGG ),
			"OGG: bad tag format", remap, SIZE_OF_ARRAY(remap), '=', 0 )
		{}
		
		std::string operator[]( const char* name ) const
		{
			std::string ret = preproc_reader::operator[](name);
			if( !ret.empty() ) {
				if (*(DWORD*)name == *(DWORD*)"TRCK") { // append total track count...
					const char* total = findTag("TRACKTOTAL");
					if( *total ) {
						ret+='/';
						ret+=total;
					}
				} else if (*(DWORD*)name == *(DWORD*)"DISC") { // append total disc count...
					const char* total = findTag("DISCTOTAL");
					if( *total ) {
						ret+='/';
						ret+=total;
					}
				}
			}
			return ret;
		}

	};

} // anon namespace

tag_reader* reader_ogg( DWORD handle )
{
/*	return new preproc_reader( 
		BASS_ChannelGetTags( handle, BASS_TAG_OGG ),
		"OGG: bad tag format", remap, SIZE_OF_ARRAY(remap), '=', 0 );*/
	return new reader( handle );
}
