//////////////////////////////////////////////////////////////////////////
//
// id3_genres.h - defines id3v1 genre table
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// Abstract:	ID3v1 tag uses 1-byte value as a genre identifier. The table
//				is indexed using this value. ID3v2 makes use of it too.
//
// (internal)
//

#ifndef ID3_GENRES_H
#define ID3_GENRES_H

// total const-correctness, anyone?
extern const char* const * const g_id3_genres;
extern const size_t g_id3_genre_count;

#endif

