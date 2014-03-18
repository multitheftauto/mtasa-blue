/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwTexture.cpp
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include <StdInc.h>
#include "../gamesa_renderware.h"

/*=========================================================
    RwTexture::SetName

    Arguments:
        name - NULL-terminated string of the new texture name
    Purpose:
        Changes the name of this texture. If the name is too long
        it triggers a RenderWare error.
    Binary offsets:
        (1.0 US): 0x007F38A0
        (1.0 EU): 0x007F38E0
=========================================================*/
void RwTexture::SetName( const char *name )
{
    RwInterface *rwInterface = RenderWare::GetInterface();

    rwInterface->m_strncpy( this->name, name, sizeof(this->name) );

    if ( rwInterface->m_strlen( this->name ) >= sizeof(this->name) )
    {
        RwError err;
        err.err1 = 1;
        err.err2 = 0x8000001E;

        RwSetError( &err );
    }
}

/*=========================================================
    RwTexture::AddToDictionary

    Arguments:
        txd - the new TXD to apply this texture to
    Purpose:
        Assigns this texture to another TXD container.
        It unlinks this texture from the previous TXD.
    Binary offsets:
        (1.0 US): 0x007F3980
        (1.0 EU): 0x007F39C0
    Note:
        At the binary offset location actually is
        RwTexDictionaryAddTexture.
=========================================================*/
void RwTexture::AddToDictionary( RwTexDictionary *txd )
{
    if ( this->txd )
        LIST_REMOVE( TXDList );

    LIST_INSERT( txd->textures.root, TXDList );

    this->txd = txd;
}

RwTexture* __cdecl RwTexDictionaryAddTexture( RwTexDictionary *texDict, RwTexture *texture )    { texture->AddToDictionary( texDict ); return texture; }
/*=========================================================
    RwTexture::RemoveFromDictionary

    Purpose:
        Unlinks this texture from the TXD container it is
        assigned to.
    Binary offsets:
        (1.0 US): 0x007F39C0
        (1.0 EU): 0x007F3A00
=========================================================*/
void RwTexture::RemoveFromDictionary( void )
{
    if ( !txd )
        return;

    LIST_REMOVE( TXDList );

    txd = NULL;
}

/*=========================================================
    RwTextureStreamReadEx (GTA:SA extension)

    Arguments:
        stream - RenderWare stream which contains the texture
    Purpose:
        Reads a RenderWare texture from the given stream and
        returns it. Returns NULL if...
            the version is invalid,
            there was no texture in the stream,
            the reading process failed or
            the data chunks were unable to be read.
        This function is used by GTA:SA to plugin-load a high
        quality texture.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00730E60
=========================================================*/
RwTexture* RwTextureStreamReadEx( RwStream *stream )
{
    unsigned int version;
    unsigned int size;

    if ( !RwStreamFindChunk( stream, 0x15, &size, &version ) )
        return NULL;

    if ( version < 0x34000 && version > 0x36003 )
    {
        RwError error;
        error.err1 = 1;
        error.err2 = -4;

        RwSetError( &error );
        return NULL;
    }

    // Here actually is performance measurement logic of GTA:SA
    // We do not require it, so I leave it out
    RwTexture *tex;

    if ( RenderWare::GetInterface()->m_readTexture( stream, &tex, size ) == 0 || !tex )
        return NULL;

    if ( !RwPluginRegistryReadDataChunks( (void*)0x008E23CC, stream, tex ) )
    {
        RwTextureDestroy( tex );
        return NULL;
    }

    // Apply special flags
    unsigned int flags = tex->flags_a;

    if ( flags == 1 )
        tex->flags_a = 2;
    else if ( flags == 3 )
        tex->flags_b = 4;

    // Note: MTA usually has a fxQuality hook here, but it did nothing for the texture loading.
    if ( *(bool*)0x00C87FFC && tex->anisotropy > 0 && g_effectManager->GetEffectQuality() > 1 )
        tex->anisotropy = RenderWare::GetDeviceInformation().maxAnisotropy;

    return tex;
}