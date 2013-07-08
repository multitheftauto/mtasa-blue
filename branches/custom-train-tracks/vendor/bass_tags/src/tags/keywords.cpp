//////////////////////////////////////////////////////////////////////////
//
// keywords.cpp - format string parser implementation
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// (internal)
//


#include <string>
#include <vector>
#include <algorithm>
#include "simplemap.h"
#include "tags_impl.h"
#include "keywords.h"
#include <stdlib.h>


namespace parser
{

// a function, performing formatting operations like %IFV1() 
typedef std::string (*keywordfunc)(sir& curr, sir last, tag_readers& reader );

// returns a formatting operation function by its name
keywordfunc get_keyword( const char* name );


namespace{

namespace err
{
const char* LP_EXP = "<***( expected! ***";
const char* CM_EXP = "<***, expected! ***";
const char* RP_EXP = "<***) expected! ***";
}

std::string trim( const std::string& str )
{
	std::string ret;
	if( str.empty() )
		return ret;
	std::string::const_iterator beg = str.begin();
	std::string::const_iterator end = str.end()-1;
	for(; *beg == ' ' && beg <= end; ++beg );
	for(; *end == ' ' && beg <= end; --end );
	ret.assign( beg, end+1 );
	return ret;
}


//////////////////////////////////////////////////////////////////////////////////////
// keywords implementations...

std::string do_IFV1(sir& itor, sir last, tag_readers& reader )
{
	if( *itor != '(' )
		return err::LP_EXP;
	++itor; // step through (

	std::string x = expr( itor, last, reader );

	if( *itor != ',' )
		return err::CM_EXP;
	++itor; // step through ,

	std::string a = expr( itor, last, reader );
	if( *itor != ')' )
		return err::RP_EXP;
	++itor; // 
	
	return x.empty() ? "" : a;
}

std::string do_IFV2(sir& itor, sir last, tag_readers& reader )
{
	if( *itor != '(' )
		return err::LP_EXP;
	++itor; // step through (

	std::string x = expr( itor, last, reader );

	if( *itor != ',' )
		return err::CM_EXP;
	++itor; // step through first ,

	std::string a = expr( itor, last, reader );
	if( *itor != ',' )
		return err::CM_EXP;
	++itor; // step through second ,


	std::string b = expr( itor, last, reader );
	if( *itor != ')' )
		return err::RP_EXP;
	++itor; // step through )

	return  x.empty() ? b : a;
}

void _upc( char& t )
{
	t = toupper(t);
}

void _lwc( char& t )
{
	t = tolower(t);
}


// upcase
// %IUPC(foO)  yields FOO
std::string do_IUPC(sir& itor, sir last, tag_readers& reader )
{
	if( *itor != '(' )
		return err::LP_EXP;
	++itor; // step through (

	std::string a = expr( itor, last, reader );
	
	if( *itor !=')' )
		return err::RP_EXP;
	++itor; // step through )

	// a hack here...
	std::for_each( a.begin(), a.end(), _upc );
	return a;
}

// lower case
// %ILWC(foO)  yields foo
std::string do_ILWC(sir& itor, sir last, tag_readers& reader )
{
	if( *itor != '(' )
		return err::LP_EXP;
	++itor;

	std::string a = expr( itor, last, reader );
	
	if( *itor !=')' )
		return err::RP_EXP;
	++itor;

	std::for_each( a.begin(), a.end(), _lwc );
	return a;
}

// capitalization:
// %ICAP(fOo) yields Foo
//
std::string do_ICAP(sir& itor, sir last, tag_readers& reader )
{
	if( *itor != '(' )
		return err::LP_EXP;
	++itor;

	std::string a = expr( itor, last, reader );
	
	if( *itor !=')' )
		return err::RP_EXP;
	++itor;

	for( std::string::iterator i = a.begin(); i!=a.end(); ++i )
		(( i == a.begin() || !isalnum( *(i-1) ) ) ? _upc : _lwc )( *i );

	return a;
}

// string trim
// %ITRM(  fOo   ) yields "fOo"
std::string do_ITRM(sir& itor, sir last, tag_readers& reader )
{
	if( *itor != '(' )
		return err::LP_EXP;
	++itor;

	std::string a = expr( itor, last, reader );
	
	if( *itor !=')' )
		return err::RP_EXP;
	++itor;

	return trim(a);
}

// UTF-8 string
// %UTF8(fOo) yields "fOo" in UTF-8 form
std::string do_UTF8(sir& itor, sir last, tag_readers& reader )
{
	if( *itor != '(' )
		return err::LP_EXP;
	++itor;

	g_utf8++;
	std::string a = expr( itor, last, reader );
	g_utf8--;
	
	if( *itor !=')' )
		return err::RP_EXP;
	++itor;

	return a;
}

}

std::string expr( sir& itor, sir last, tag_readers& reader ) throw()
{
	std::string ret;
	while( itor < last )
	{
		switch( *itor )
		{
		case '%':
			{
				++itor;
				if( strchr("%(,)", *itor ) ) // special characters are escaped
				{
					ret += *itor++; break;
				}

				if( last < itor+4 )
				{
					ret += " < unexpected end-of-string.";
					itor = last; // done with it
					return ret;
				}
				std::string keyword( itor, itor+4 );
				itor += 4; // and move the pointer

				keywordfunc func = get_keyword( keyword.c_str() );
				if( func )
					ret += func( itor, last, reader );
				else // can be a tag
				{
					for( int i=0; reader[i]; ++i ) {
						try{
							std::string tag = (*reader[i])[ keyword.c_str() ];
							int p0=tag.find((char)0); // look for NULL...
							if (p0>=0) tag.resize(p0); // and trim at it
							if (tag.empty()) continue;
							int utf8=-1,b=0;
							for (int a=0; a<tag.length(); a++) {
								if (tag[a]&0x80) {
									if (!(tag[a]&0x40)) {
										if (b) {
											b--;
											continue;
										}
										utf8=0;
										break;
									}
									if (b) break;
									for (b=1; tag[a]&(0x40>>b); b++) ;
									utf8=1;
								} else if (b)
									break;
							}
							if (b) utf8=0;
							if (utf8>=0 && (!utf8^!g_utf8)) { // needs converting
								int n=MultiByteToWideChar(g_utf8?CP_ACP:CP_UTF8, 0, &tag[0], -1, 0, 0);
								std::vector< wchar_t > tmp( n+1 );
								MultiByteToWideChar(g_utf8?CP_ACP:CP_UTF8, 0, &tag[0], -1, &tmp[0], n+1);
								ret += unicode_string(&tmp[0], !!g_utf8);
							} else
								ret += tag;
							break;
						}catch( const bad_ident& )
						{
							// just skip it
						}
					}
				}
				break;
			}
		case ',':
		case ')':
			return ret;

		default:
			ret += *itor++; // just let it in...
			break;
		}
	}
	return ret;
}

std::string unicode_string( const wchar_t *utext, bool utf8 )
{
	int n=WideCharToMultiByte(utf8?CP_UTF8:CP_ACP, 0, utext, -1, 0, 0, 0, 0);
	std::vector< char > text( n+1 );
	WideCharToMultiByte(utf8?CP_UTF8:CP_ACP, 0, utext, -1, &text[0], n+1, 0, 0);
	return std::string( &text[0] );
}


//////////////////////////////////////////////////////////////////////////////////
// available keywords

namespace
{

typedef tools::simplemap<const char*, keywordfunc, tools::cmp_sz > map_t;
typedef map_t::pair_type vpair;

vpair known_keywords [] = 
{
	vpair("IFV1", &do_IFV1),
	vpair("IFV2", &do_IFV2),
	vpair("IUPC", &do_IUPC),
	vpair("ILWC", &do_ILWC),
	vpair("ICAP", &do_ICAP),
	vpair("ITRM", &do_ITRM),
	vpair("UTF8", &do_UTF8)
};
map_t keyword_map(known_keywords, END_OF_ARRAY(known_keywords));

} // anon namespace


keywordfunc get_keyword( const char* name )
{
	return keyword_map[name];
}



}



