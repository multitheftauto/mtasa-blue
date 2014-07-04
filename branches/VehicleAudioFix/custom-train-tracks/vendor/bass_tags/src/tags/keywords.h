//////////////////////////////////////////////////////////////////////////
//
// keywords.h - text-processing subsystem, featuring an ATF-like syntax.
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// Abstract:	The parser processes a piece of string and forms an output
//				string, substituting special identifier for information
//				obtained from tags.
//
// (internal)


#ifndef KEYWORDS_H_383653
#define KEYWORDS_H_383653

namespace parser
{

// string iterator
typedef std::string::const_iterator sir;

// the parser itself
std::string expr(sir& curr, sir last, tag_readers& reader ) throw();

std::string unicode_string( const wchar_t *utext, bool utf8 );

}

#endif
