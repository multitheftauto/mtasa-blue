/************************************************************************
	filename: 	CEGUIDefaultResourceProvider.cpp
	created:	8/7/2004
	author:		James '_mental_' O'Sullivan
	
	purpose:	Implements the Resource Manager common functionality
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
#include "StdInc.h"
#include "CEGUIDefaultResourceProvider.h"
#include "CEGUIExceptions.h"

#include <fstream>

// Start of CEGUI namespace section
namespace CEGUI
{
//    void DefaultResourceProvider::loadInputSourceContainer(const String& filename, InputSourceContainer& output)
//    {
//        if (filename.empty() || (filename == (utf8*)""))
//        {
//            throw InvalidRequestException((utf8*)
//                "DefaultResourceProvider::load - Filename supplied for data loading must be valid");
//        }
//
//        XERCES_CPP_NAMESPACE_USE
//        XMLCh* pval = XMLString::transcode(filename.c_str());
//        InputSource* mInputSource = new LocalFileInputSource(pval);
//        XMLString::release(&pval);
//
//        output.setData(mInputSource);
//    }

    void DefaultResourceProvider::loadRawDataContainer(const String& filename, RawDataContainer& output, const String& resourceGroup)
    {
        if (filename.empty() || (filename == (utf8*)""))
        {
            throw InvalidRequestException((utf8*)
                "DefaultResourceProvider::load - Filename supplied for data loading must be valid");
        }

        SString strFilename = filename.c_str();
        if ( !FileExists( strFilename ) )
        {
            // If filename does not exist, try preserving original 128-255 characters
            strFilename = "";
            for ( uint i = 0 ; i < filename.length () ; i++ )
            {
                strFilename += filename[i];
            }
        }

        std::ifstream dataFile(strFilename, std::ios::binary|std::ios::ate);
        if( dataFile.fail())
        {
            throw InvalidRequestException((utf8*)
                "DefaultResourceProvider::load - " + filename + " does not exist");
        }
        std::streampos size = dataFile.tellg();
        dataFile.seekg (0, std::ios::beg);

        unsigned char* buffer = new unsigned char [size];

        try {
            dataFile.read(reinterpret_cast<char*>(buffer), size);
        }
        catch(std::ifstream::failure e) {
            delete [] buffer;
            throw GenericException((utf8*)
                "DefaultResourceProvider::loadRawDataContainer - Problem reading " + filename);
        }

        dataFile.close();

        //memcpy(container->getDataPtr(), buffer, size);
        output.setData(buffer);
        output.setSize(size);
        //delete [] buffer;
    }

} // End of  CEGUI namespace section
