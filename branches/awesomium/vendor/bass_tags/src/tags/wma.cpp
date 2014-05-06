
// wma tag reader implementation

#include "tag_preproc.h"
#include "rselect.h"

#ifdef BASS_TAG_WMA

namespace
{
	typedef preproc_reader::vpair vpair;
	const vpair	remap_old[] = 
	{
		vpair("TITL", "Title "),
		vpair("ARTI", "Author "),
		vpair("ALBM", "WM/AlbumTitle "),
		vpair("GNRE", "WM/Genre "), 
		vpair("YEAR", "WM/Year "),
		vpair("CMNT", "Description "),
		vpair("TRCK", "WM/TrackNumber "),
		vpair("COMP", "WM/Composer "),
		vpair("COPY", "Copyright "),
		vpair("SUBT", "WM/SubTitle "),
		vpair("AART", "WM/AlbumArtist "),
		vpair("DISC", "WM/PartOfSet "),
	};

	// <=2.3 wma tag format:
	//
	// tag : value\0
	// tag : value\0
	// ...
	// \0

	const vpair	remap[] = 
	{
		vpair("TITL", "Title"),
		vpair("ARTI", "Author"),
		vpair("ALBM", "WM/AlbumTitle"),
		vpair("GNRE", "WM/Genre"), 
		vpair("YEAR", "WM/Year"),
		vpair("CMNT", "Description"),
		vpair("TRCK", "WM/TrackNumber"),
		vpair("COMP", "WM/Composer"),
		vpair("COPY", "Copyright"),
		vpair("SUBT", "WM/SubTitle"),
		vpair("AART", "WM/AlbumArtist"),
		vpair("DISC", "WM/PartOfSet"),
	};

	// >=2.4 wma tag format:
	//
	// tag=value\0
	// tag=value\0
	// ...
	// \0


struct reader: public preproc_reader
{
	typedef preproc_reader myb;
	reader( DWORD handle ): myb(
		BASS_ChannelGetTags( handle, BASS_TAG_WMA ),
		"WMA: bad tag format",
		remap_old,
		SIZE_OF_ARRAY(remap),
		':', 0
		)
	{}
	
	// this one needs tweaking a bit...
	std::string operator[]( const char* name ) const
	{
		std::string ret = myb::operator[](name);
		if( !ret.empty() )
			ret.erase( size_t(0), size_t(1) ); // pop out the first space character
		return ret;
	}

};

} // anon namespace

tag_reader* reader_wma( DWORD handle )
{
	if (HIWORD(BASS_GetVersion())<0x204)
		return new reader( handle );

	return new preproc_reader( 
		BASS_ChannelGetTags( handle, BASS_TAG_WMA ),
		"WMA: bad tag format", remap, SIZE_OF_ARRAY(remap), '=', 0 );
}

#else
#pragma message("WMA tag support disabled.")
#endif
