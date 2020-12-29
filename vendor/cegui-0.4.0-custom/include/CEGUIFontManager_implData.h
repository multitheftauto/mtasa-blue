/************************************************************************
	filename: 	CEGUIFontManager_implData.h
	created:	23/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation data for FontManager to break 
				dependency on FreeType2 for clients
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
#ifndef _CEGUIFontManager_implData_h_
#define _CEGUIFontManager_implData_h_

#include <ft2build.h>
#include FT_FREETYPE_H


// Start of CEGUI namespace section
namespace CEGUI
{
struct FontManager::FontManagerImplData
{
	FT_Library		d_ftlib;		//!< FreeType library;
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIFontManager_implData_h_
