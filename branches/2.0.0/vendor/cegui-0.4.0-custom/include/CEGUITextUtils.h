/************************************************************************
	filename: 	CEGUITextUtils.h
	created:	30/5/2004
	author:		Paul D Turner
	
	purpose:	Interface to a static class containing some utility
				functions for text / string operations
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#ifndef _CEGUITextUtils_h_
#define _CEGUITextUtils_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Text utility support class.  This class is all static members.  You do not create instances of this class.
*/
class CEGUIEXPORT TextUtils
{
public:
	/*************************************************************************
		Constants
	*************************************************************************/
	static const String	DefaultWhitespace;		//!< The default set of whitespace
	static const String	DefaultAlphanumerical;	//!< default set of alphanumericals.
	static const String	DefaultWrapDelimiters;	//!< The default set of word-wrap delimiters


	/*************************************************************************
		Methods
	*************************************************************************/
	/*!
	\brief
		return a String containing the the next word in a String.

		This method returns a String object containing the the word, starting at index \a start_idx, of String \a str
		as delimited by the code points specified in string \a delimiters (or the ends of the input string).

	\param str
		String object containing the input data.

	\param start_idx
		index into \a str where the search for the next word is to begin.  Defaults to start of \a str.

	\param delimiters
		String object containing the set of delimiter code points to be used when determining the start and end
		points of a word in string \a str.  Defaults to whitespace.

	\return
		String object containing the next \a delimiters delimited word from \a str, starting at index \a start_idx.
	*/
	static	String	getNextWord(const String& str, String::size_type start_idx = 0, const String& delimiters = DefaultWhitespace);


	/*!
	\brief
		Return the index of the first character of the word at \a idx.

	/note
		This currently uses DefaultWhitespace and DefaultAlphanumerical to determine groupings for what constitutes a 'word'.

	\param str
		String containing text.

	\param idx
		Index into \a str where search for start of word is to begin.

	\return
		Index into \a str which marks the begining of the word at index \a idx.
	*/
	static	String::size_type	getWordStartIdx(const String& str, String::size_type idx);


	/*!
	\brief
		Return the index of the first character of the word after the word at \a idx.

	/note
		This currently uses DefaultWhitespace and DefaultAlphanumerical to determine groupings for what constitutes a 'word'.

	\param str
		String containing text.

	\param idx
		Index into \a str where search is to begin.

	\return
		Index into \a str which marks the begining of the word at after the word at index \a idx.
		If \a idx is within the last word, then the return is the last index in \a str.
	*/
	static	String::size_type	getNextWordStartIdx(const String& str, String::size_type idx);


	/*!
	\brief
		Trim all characters from the set specified in \a chars from the begining of \a str.

	\param str
		String object to be trimmed.

	\param chars
		String object containing the set of code points to be trimmed.
	*/
	static	void	trimLeadingChars(String& str, const String& chars);


	/*!
	\brief
		Trim all characters from the set specified in \a chars from the end of \a str.

	\param str
		String object to be trimmed.

	\param chars
		String object containing the set of code points to be trimmed.
	*/
	static	void	trimTrailingChars(String& str, const String& chars);


private:
	/*************************************************************************
		Data
	*************************************************************************/
	static	String		d_delimiters;			//!< Current set of delimiters.
	static	String		d_whitespace;			//!< Current set of whitespace.


	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor and Destructor are private.  This class has all static members.
	*/
	TextUtils(void);
	~TextUtils(void);
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUITextUtils_h_
