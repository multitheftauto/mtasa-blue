/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceConfigItem.cpp
*  PURPOSE:     Resource configuration item class
*  DEVELOPERS:  Talidan
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class basically converts standard filepaths into pointers
// The implications of this are that it can be exchanged between resources in Lua
// The file pointers are converted to userdata in Lua, and can be used anywhere where string paths exist
// Bear in mind that this only supports functions that are modified to support them

#include <StdInc.h>

CResourceFileItem::CResourceFileItem ( CResource * resource, std::string strFilePath )
{
    m_strFilePath = strFilePath;
    std::transform(strFilePath.begin(), strFilePath.end(), strFilePath.begin(), tolower);
    m_pResource = resource;
}

//Check if the specified file even exists
bool CResourceFileItem::Exists ()
{
    return DoesFileExist(SString("%s\\%s",m_pResource->GetResourceDirectoryPath(),m_strFilePath.c_str()).c_str());
}
