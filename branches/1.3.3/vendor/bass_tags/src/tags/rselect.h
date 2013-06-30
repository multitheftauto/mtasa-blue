//////////////////////////////////////////////////////////////////////////
//
// reselect.h - tag reader generator function forward declaration
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// Abstract:	Tag readers are created using functions of the same signature.
//				This helps selecting the right one from an array. This header
//				lists one reader generator declaration for each corresponding
//				tag type. More readers should be added as needed.
//
// (internal)

#ifndef RSELECT_H
#define RSELECT_H

typedef tag_reader* (generator_signature)( DWORD handle ); 
typedef generator_signature* pfn_generator; 

tag_reader* reader_ape( DWORD handle );
tag_reader* reader_id3v1( DWORD handle );
tag_reader* reader_id3v2( DWORD handle );
tag_reader* reader_id3v22( DWORD handle );
tag_reader* reader_mod( DWORD handle );
tag_reader* reader_mp4( DWORD handle );
tag_reader* reader_ogg( DWORD handle );
tag_reader* reader_wma( DWORD handle );

#endif
