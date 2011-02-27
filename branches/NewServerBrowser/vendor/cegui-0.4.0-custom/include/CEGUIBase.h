/************************************************************************
	filename: 	CEGUIBase.h
	created:	20/2/2004
	author:		Paul D Turner
	
	purpose:	Base include used within the system
				This contains various lower level bits required
				by other parts of the system.  All other library 
				headers will include this file.
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
#ifndef _CEGUIBase_h_
#define _CEGUIBase_h_

#include <cassert>

// bring in configuration options
#include "CEGUIConfig.h"

// add CEGUI version defines
#include "CEGUIVersion.h"

/*************************************************************************
	Dynamic Library import / export control conditional
	(Define CEGUIBASE_EXPORTS to export symbols, else they are imported)
*************************************************************************/
#if defined( __WIN32__ ) || defined( _WIN32 )
#   ifdef CEGUIBASE_EXPORTS
#       define CEGUIEXPORT __declspec(dllexport)
#   else
#       define CEGUIEXPORT __declspec(dllimport)
#   endif
#       define CEGUIPRIVATE
#else
#       define CEGUIEXPORT
#       define CEGUIPRIVATE
#endif

// Statically linked, so blank it out
#undef CEGUIEXPORT
#define CEGUIEXPORT

// totally kill this warning (debug info truncated to 255 chars etc...) on <= VC6
#if defined(_MSC_VER) && (_MSC_VER <= 1200)
#   pragma warning(disable : 4786)
#endif


// Detect macros for min / max and undefine (with a warning where possible)
#if defined(max)
#   if defined(_MSC_VER)
#       pragma message("Macro defintion of max detected - undefining")
#   elif defined (__GNUC__)
#       warning ("Macro defintion of max detected - undefining")
#   endif
#   undef max
#endif
#if defined(min)
#   if defined(_MSC_VER)
#       pragma message("Macro defintion of min detected - undefining")
#   elif defined (__GNUC__)
#       warning ("Macro defintion of min detected - undefining")
#   endif
#   undef min
#endif


// include this to see if it defines _STLPORT_VERION
#	include <string>

// fix to undefine _STLP_DEBUG if STLport is not actually being used
// (resolves some unresolved externals concerning boost)
#if defined(_STLP_DEBUG) && defined(_MSC_VER) && (_MSC_VER >= 1200)
#	if !defined(_STLPORT_VERSION)
#		undef _STLP_DEBUG
#	endif
#endif


// The following defines macros used within CEGUI for std::min/std::max
// usage, and is done as a compatibility measure for VC6 with native STL.
#if defined(_MSC_VER) && (_MSC_VER <= 1200) && !defined(_STLPORT_VERSION)
#    define ceguimin	std::_cpp_min
#    define ceguimax	std::_cpp_max
#else
#    define ceguimin	std::min
#    define ceguimax	std::max
#endif


/*************************************************************************
	Documentation for the CEGUI namespace itself
*************************************************************************/
/*!
\brief
	Main namespace for Crazy Eddie's GUI Library

	The CEGUI namespace contains all the classes and other items that comprise the core
	of Crazy Eddie's GUI system.
*/
namespace CEGUI
{

/*************************************************************************
	Simplification of some 'unsigned' types
*************************************************************************/
typedef	unsigned long	ulong;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned char	uchar;

typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;


/*************************************************************************
	System wide constants
*************************************************************************/
static const float		DefaultNativeHorzRes	= 640.0f;		//!< Default native horizontal resolution (for fonts and imagesets)
static const float		DefaultNativeVertRes	= 480.0f;		//!< Default native vertical resolution (for fonts and imagesets)


/*************************************************************************
    Additional typedefs
*************************************************************************/
typedef std::ostream OutStream;     //!< Output stream class.
}  // end of CEGUI namespace section


/*!
\brief
	Macro used to return a float value rounded to the nearest integer.

	This macro is used throughout the library to ensure that elements are
	kept at integer pixel positions on the display.

\param x
	Expression to be rounded to nearest whole number

\return
	\a x after having been rounded
*/
#if defined(CEGUI_ALIGN_ELEMENTS_TO_PIXELS)
#	define PixelAligned(x)	( (float)(int)(( x ) + 0.5f) )
#else
#	define PixelAligned(x)	( x )
#endif


/*************************************************************************
	Bring in forward references to all GUI base system classes
*************************************************************************/
#include "CEGUIForwardRefs.h"


#endif	// end of guard _CEGUIBase_h_
