/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CLuaShared.h
*
*****************************************************************************/
#pragma once
class CLuaShared
{
public:
    static void        EmbedChunkName          ( SString strChunkName, const char** pcpOutBuffer, uint* puiOutSize );
    static bool        CheckUTF8BOMAndUpdate ( const char** pcpOutBuffer, uint* puiOutSize );
};
