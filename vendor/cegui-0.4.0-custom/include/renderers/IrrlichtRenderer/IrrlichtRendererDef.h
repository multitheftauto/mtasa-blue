/************************************************************************
filename: 	IrrlichtRendererDef.h
created:	20/7/2004
author:		Thomas Suter

changes: 
- Irrlicht patching not needed anymore
- using the irrlicht filesystem to load config files etc.
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

/*
	Beginning from version 0.7 Irrlicht does NOT need any changes for the GUI-renderer.
	Thanks to Nikolaus Gebhardt for including the missing methods in the renderer.
*/

#ifndef IRRLICHTRENDERERDEF_H_INCLUDED
#define IRRLICHTRENDERERDEF_H_INCLUDED

#if defined( __WIN32__ ) || defined( _WIN32 ) || defined (WIN32)
#   ifdef IRRRENDERER_EXPORTS
#       define IRRLICHT_GUIRENDERER_API __declspec(dllexport)
#   else
#       define IRRLICHT_GUIRENDERER_API __declspec(dllimport)
#   endif
#else
#   define IRRLICHT_GUIRENDERER_API
#endif

#endif
