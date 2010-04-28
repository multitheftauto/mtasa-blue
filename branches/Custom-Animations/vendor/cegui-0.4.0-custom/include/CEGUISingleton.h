/************************************************************************
	filename: 	CEGUISingleton.h
	created:	22/2/2004
	author:		Paul D Turner
	
	purpose:	Singleton Base Class
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
/*************************************************************************
	
	The code in this file is taken from article 1.3 in the the book:
	Game Programming Gems from Charles River Media

*************************************************************************/
#ifndef _CEGUISingleton_h_
#define _CEGUISingleton_h_

#include "CEGUIBase.h"
#include <cassert>

// Start of CEGUI namespace section
namespace CEGUI
{
/* Copyright (C) Scott Bilas, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */


template <typename T> class CEGUIEXPORT Singleton
{
protected:
    static T* ms_Singleton;

public:
    Singleton( void )
    {
        assert( !ms_Singleton );
        ms_Singleton = static_cast<T*>(this);
    }
   ~Singleton( void )
        {  assert( ms_Singleton );  ms_Singleton = 0;  }
    static T& getSingleton( void )
        {  assert( ms_Singleton );  return ( *ms_Singleton );  }
    static T* getSingletonPtr( void )
        {  return ( ms_Singleton );  }
};

} // End of  CEGUI namespace section



#endif	// end of guard _CEGUISingleton_h_
