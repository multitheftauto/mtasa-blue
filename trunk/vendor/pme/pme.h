
////////////////////////////////////////////////////////////////////////////////////////////////
//
// This file is redistributable on the same terms as the PCRE 4.3 license, except copyright
// notice must be attributed to:
//
// (C) 2003 Zachary Hansen xaxxon@slackworks.com
//
// Distribution under the GPL or LGPL overrides any other restrictions, as in the PCRE license
//
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PME_H
#define PME_H

#include <string.h>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <vector>

#include <stdio.h>

using namespace std;

#include <pcre.h>



/// PME wraps the PCRE C API into a more perl-like syntax
/**
 * PME wraps the PCRE C API into a more perl-liek syntax.  It supports single matching,
 *   global matching (where the regex resumes where it left off on the previous iteration),
 *   single and global substitutions (including using backrefs in the substituted strings),
 *   splits based on regex comparisons, and a syntactically easy way to get substrings out f
 *   from backrefs and splits.
 */
typedef vector < std::string > StringVector;
class PME
{
public:

	/// default constructor -- virtually worthless
	PME( );

	/// s is the regular expression, opts are PCRE flags bit-wise or'd together
	PME(const std::string & s, unsigned opts );

	/// s is the regular expression, opts is a perl-like string of modifier letters "gi" is global and case insensitive
	PME(const std::string & s, const std::string & opts = "");

	/// s is the regular expression, opts are PCRE flags bit-wise or'd together
	PME(const char * s, unsigned opts );

	/// s is the regular expression, opts is a perl-like string of modifier letters "gi" is global and case insensitive  
	PME ( const char * s, const std::string & opts = "" );

	/// PME copy constructor
	PME(const PME & r);

	/// PME = PME assignment operator
	const PME & operator = (const PME & r);

	/// destructor
	~PME();

	/// stores results from matches
	typedef pair<int, int> markers;
	
	/// returns options set on this object
	unsigned				options();

	/// sets new options on the object -- doesn't work?
	void					options(unsigned opts);

	/// runs a match on s against the regex 'this' was created with -- returns the number of matches found
	int         			match(const std::string & s, unsigned offset = 0);

	/// splits s based on delimiters matching the regex.
	int         			split(const std::string & s, ///< string to split on
								  unsigned maxfields = 0 ///< maximum number of fields to be split out.  0 means split all fields, but discard any trailing empty bits.  Negative means split all fields and keep trailing empty bits.  Positive means keep up to N fields including any empty fields less than N.  Anything remaining is in the last field.
);

	/// substitutes out whatever matches the regex for the second paramter
	std::string             sub ( const std::string & s, 
								  const std::string & r,
								  int dodollarsubstitution = 1 );

	/// study the regular expression to make it faster
	void                    study();

	/// returns the substring from the internal m_marks vector requires having run match or split first
	std::string             operator[](int);

	/// resets the regex object -- mostly useful for global matching
	void                    reset();

	/// returns the number of back references returned by the last match/sub call
	int                     NumBackRefs ( ) { return nMatches; }

	/// whether this regex is valid
	int IsValid ( ) { return nValid; }

	/// returns a vector of strings for the last match/split/sub
	StringVector GetStringVector ( );


protected:

	/// used internally for operator[]
	/** \deprecated going away */
	std::string		       	substr(const std::string & s,
								const vector<markers> & marks, unsigned index);


	pcre * re; ///< pcre structure from pcre_compile

	unsigned _opts; ///< bit-flag options for pcre_compile

	pcre_extra * extra;	///< results from pcre_study

	int nMatches; ///< number of matches returned from last pcre_exec call

	vector<markers> m_marks; ///< last set of indexes of matches

	std::string laststringmatched; ///< copy of the last string matched
	void * addressoflaststring; ///< used for checking for change of string in global match

	int m_isglobal; ///< non-pcre flag for 'g' behaviour
	int lastglobalposition; ///< end of last match when m_isglobal != 0
	
	/// compiles the regex -- automatically called on construction
	void compile(const std::string & s);

	/// used to make a copy of a regex object
	static pcre * clone_re(pcre * re);

	/// takes perl-style character modifiers and determines the corresponding PCRE flags
	unsigned int DeterminePcreOptions ( const std::string & opts = "" );
	
	/// deals with $1-type constructs in the replacement string in a substitution
	std::string UpdateReplacementString ( const std::string & r );
	

	/// flag as to whether this regex is valid (compiled without error)
	int                     nValid;

};


#endif // PME_H
