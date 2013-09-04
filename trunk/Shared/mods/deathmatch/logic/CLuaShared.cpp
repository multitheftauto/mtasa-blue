/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CLuaShared.cpp
*
*****************************************************************************/

#include "StdInc.h"

// If compiled script, make sure correct chunkname is embedded
void EmbedChunkName( SString strChunkName, const char** pcpOutBuffer, uint* puiOutSize )
{
    const char*& cpBuffer = *pcpOutBuffer;
    uint& uiSize = *puiOutSize;

    if ( !IsLuaCompiledScript( cpBuffer, uiSize ) )
        return;

    if ( uiSize < 12 )
        return;

    // Get size of name in original
    uint uiStringSizeOrig = *(uint*)( cpBuffer + 12 );
    if ( uiSize < 12 + 4 + uiStringSizeOrig )
        return;

    static CBuffer store;
    store.Clear();
    CBufferWriteStream stream( store );

    // Ensure name starts with @ and ends with NULL
    if ( !strChunkName.BeginsWith( "@" ) )
        strChunkName = "@" + strChunkName;
    if ( strChunkName[ strChunkName.length() - 1 ] != '\0' )
        strChunkName.push_back( '\0' );

    // Header
    stream.WriteBytes( cpBuffer, 12 );
    // New name size
    stream.Write( strChunkName.length() );
    // New name bytes incl zero termination
    stream.WriteBytes( strChunkName.c_str(), strChunkName.length() );
    // And the rest
    stream.WriteBytes( cpBuffer + 12 + 4 + uiStringSizeOrig, uiSize - 12 - 4 - uiStringSizeOrig );

    cpBuffer = store.GetData();
    uiSize = store.GetSize();
}
