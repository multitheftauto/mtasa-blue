//////////////////////////////////////////////////////////////////////////
//
// config.h - configures compile-time options
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// Abstract:	COntains a number of tweaks to control various library features.
//
// (internal)


#ifndef TAGS_CONFIG_H
#define TAGS_CONFIG_H


//  ===== some switches ======
// Note: (recom) stands for "recommended".


//  ****************** Section 1. Tag type guessing **********************

// REMOVED

//  ****************** Section 2. ID3v2 reader **********************

// The ID3v2 reader currently adheres to ID3v2.3.0 informal standard as described
// here: http://www.id3.org/id3v2.3.0.html. 

// TAGS_ID3V2_FACIST
// the following switch controls whether id3v2 reader should strictly follow the
// standard. It implies any particular tag field to be considered bogus if it 
// deviates from the standard (but still is readable), eg. Winamp's internal tagger
// writes custom genres without any name parsing, resulting in genres like
// "(my_genre)" to be written into a tag frame, while the standard requires the 
// tagger to escape opening parentheses, eg. "((my_genre)".
// when 0, (recom)	permits some standard deviations as above
// when 1,			strictly adheres to the standard.
#define TAGS_ID3V2_FACIST 0

// TAGS_ID3V2_GENRE_REFINEMENT_SEPARATOR
// the following macro defines a string to be used as "genre refinement" separator
// ( eg. when a numeric ID3v1 genre is followed by a text string, like "(5)Something",
// it should be translated to "Disco/Something")
#define TAGS_ID3V2_GENRE_REFINEMENT_SEPARATOR "/"

// TAGS_ID3V2_COMMENT_RETURNS
// the standard says there are two strings in the comment frame, a short string,
// and a full string ... which one should be returned?
// Syntax: this is the function return statement.
// macros: S - short, L - full, they are std::strings. Literals, expressions and
// <algoritm> functions are permitted. See id3v2_3.cpp for details.
#define TAGS_ID3V2_COMMENT_RETURNS S.empty() ? L : S + " " +L

//  ****************** Section 3. ID3v1 reader **********************

// TAGS_ID3V1_REMOVE_STUPID_PADDING
// some taggers are very smart and pad text fields in ID3v1 tags with spaces, rather
// than with zeros. This option permits ID3v1 tag reader to cut the trailing spaces off.
// when 0,			returns tag field value unchanged
// when 1, (recom)	cuts off trailing spaces
#define TAGS_ID3V1_REMOVE_STUPID_PADDING 1

//  ****************** Section 4. Exported functions **********************

// TAGS_MAIN_ERROR_RETURN_VALUE
// defines what TAGS_Read() should return if it fails to read tags. There has been
// some discussion about cross-language compatibility, notably VB app crashes if it
// gets NULL instead of a pointer while converting c-string into its vb-string.
// So an empty string is recommended.
#define TAGS_MAIN_ERROR_RETURN_VALUE ""

#endif // TAGS_CONFIG_H
