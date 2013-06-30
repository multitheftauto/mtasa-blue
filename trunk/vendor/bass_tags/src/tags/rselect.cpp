//////////////////////////////////////////////////////////////////////////
//
// rselect.cpp - tag reader selector implementation
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// (internal)
//


#include "tags_impl.h"
#include "rselect.h"


struct no_tag;
struct bad_tag;

namespace
{


const pfn_generator all_readers[] =
{
	reader_ogg,
	reader_mp4,
	reader_wma,
	reader_ape,
	reader_id3v22,
	reader_id3v2,
	reader_id3v1,
	reader_mod,

	NULL // end marker
};

}

tag_readers::tag_readers( DWORD dwHandle )
{
	g_err = "ok";
	g_utf8 = 0;
	try{
		for( int i=0; all_readers[i]; ++i )
		{
			try{
				m_readers.push_back(all_readers[i]( dwHandle ));
			}catch( const no_tag& )
			{
				// no tag or unsupported, just try again using new type
			}
		}
		if (!m_readers.size()) g_err = "TAGS: no supported tag(s) found.";
	}catch( const bad_tag& e )
	{
		g_err = e.what();
	}
}