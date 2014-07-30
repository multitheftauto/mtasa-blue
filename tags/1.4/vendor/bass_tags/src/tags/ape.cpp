//////////////////////////////////////////////////////////////////////////
//
// ape.cpp - ape tag reader implementation
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// (internal)
//


#include "tag_preproc.h"

#ifdef BASS_TAG_APE

namespace
{
	typedef preproc_reader::vpair vpair;
	const vpair remap[] = 
	{
		vpair("TITL", "TITLE"),
		vpair("ARTI", "ARTIST"),
		vpair("ALBM", "ALBUM"),
		vpair("GNRE", "GENRE"), 
		vpair("YEAR", "YEAR"),
		vpair("CMNT", "COMMENT"),
		vpair("TRCK", "TRACK"),
		vpair("COMP", "COMPOSER"),
		vpair("COPY", "COPYRIGHT"),
		vpair("SUBT", "SUBTITLE"),
		vpair("AART", "ALBUMARTIST|ALBUM ARTIST"),
		vpair("DISC", "DISC|DISCNUMBER"),
	};

	// ape tag format:
	//
	// tag=value\0
	// tag=value\0
	// ...
	// \0
} // anon namspace

tag_reader* reader_ape( DWORD handle )
{
	return new preproc_reader( 
		(const char*) BASS_ChannelGetTags( handle, BASS_TAG_APE ),
		"APE: bad tag format", remap, SIZE_OF_ARRAY(remap), '=', 0 );
}

#else
#pragma message("APE tag support disabled.")
#endif