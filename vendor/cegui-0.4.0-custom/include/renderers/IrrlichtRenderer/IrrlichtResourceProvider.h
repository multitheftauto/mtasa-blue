/************************************************************************
filename: 	IrrlichtResourceProvider.h
created:	12/22/2004
author:		Thomas Suter

purpose:	DImplements the Resource Provider common functionality
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
#ifndef _CEGUIIrrlichtResourceProvider_h_
#define _CEGUIIrrlichtResourceProvider_h_

#include "IrrlichtRendererDef.h"
#include "CEGUIResourceProvider.h"
#include <IFileSystem.h>
// Start of CEGUI namespace section
namespace CEGUI
{
	class IRRLICHT_GUIRENDERER_API IrrlichtResourceProvider : public ResourceProvider
	{
	protected:
		irr::io::IFileSystem* fsys;
	public:
		/*************************************************************************
		Construction and Destruction
		*************************************************************************/
		IrrlichtResourceProvider(irr::io::IFileSystem* fsys_);

		~IrrlichtResourceProvider(void) ;

		void loadRawDataContainer(const String& filename, RawDataContainer& output, const String& resourceGroup);
        void unloadRawDataContainer(RawDataContainer& data);
	};

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIIrrlichtResourceProvider_h_
