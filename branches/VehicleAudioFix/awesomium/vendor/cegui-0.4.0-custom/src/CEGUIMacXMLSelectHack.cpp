/************************************************************************
	filename: 	CEGUIWin32XMLSelectHack.cpp
	created:	14/3/2005
	author:		Paul D Turner
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
    This hack of a file will become source for whichever XML Parser has
    been selected as the default.  It's easier to do this than to mess
    about trying to do the same by messing around with the project files
    for all the different VC++ compiler versions.
*************************************************************************/
#include "CEGUIConfig.h"

#ifdef CEGUI_WITH_XERCES
#   include "CEGUIXercesParser.cpp"
#else
#   include "CEGUITinyXMLParser.cpp"
#   include "tinyxml/tinystr.cpp"
#   include "tinyxml/tinyxml.cpp"
#   include "tinyxml/tinyxmlerror.cpp"
#   include "tinyxml/tinyxmlparser.cpp"
#endif
