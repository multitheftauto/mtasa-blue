//////////////////////////////////////////////////////////////////////////
//
// mp4.cpp - MP4 tag reader implementation
//
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
		vpair("TRCK", "TRACK"),
		vpair("COMP", "WRITER"),
		vpair("AART", "ALBUMARTIST"),
		vpair("DISC", "DISC"),
	};

	struct reader: public preproc_reader
	{
		reader( DWORD handle ): preproc_reader(
			BASS_ChannelGetTags( handle, BASS_TAG_MP4 ),
			"MP4: bad tag format", remap, SIZE_OF_ARRAY(remap), '=', 0 )
		{}
		
		std::string operator[]( const char* name ) const
		{
			std::string ret = preproc_reader::operator[](name);
			if( !ret.empty() ) {
				if (*(DWORD*)name == *(DWORD*)"TRCK") { // append total track count...
					const char* total = findTag("TOTALTRACKS");
					if( *total ) {
						ret+='/';
						ret+=total;
					}
				} else if (*(DWORD*)name == *(DWORD*)"DISC") { // append total disc count...
					const char* total = findTag("TOTALDISCS");
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

tag_reader* reader_mp4( DWORD handle )
{
	return new reader( handle );
}
