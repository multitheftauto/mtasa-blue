/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto 
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/mods/logic/lua/CLuaShared.h
*
*****************************************************************************/
#pragma once
class CLuaShared
{
public:
    static void        EmbedChunkName          ( SString strChunkName, const char** pcpOutBuffer, uint* puiOutSize );
    static bool        CheckUTF8BOMAndUpdate ( const char** pcpOutBuffer, uint* puiOutSize );
};
