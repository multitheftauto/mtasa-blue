/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwTexDictionary.cpp
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include <StdInc.h>
#include "gamesa_renderware.h"

/*=========================================================
    RwTexDictionary::GetFirstTexture

    Purpose:
        Returns the first texture of this TXD.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734940
=========================================================*/
RwTexture* RwTexDictionary::GetFirstTexture( void )
{
    if ( LIST_EMPTY( textures.root ) )
        return NULL;

    return LIST_GETITEM( RwTexture, textures.root.next, TXDList );
}

/*=========================================================
    RwTexDictionary::FindNamedTexture

    Arguments:
        name - case-insensitive name to check the textures against
    Purpose:
        Scans the textures of this TXD and returns the first one
        which is found with the name. If not, it returns NULL.
    Binary offsets:
        (1.0 US): 0x007F39F0
        (1.0 EU): 0x007F3A30
=========================================================*/
RwTexture* RwTexDictionary::FindNamedTexture( const char *name )
{
    LIST_FOREACH_BEGIN( RwTexture, textures.root, TXDList )
        if ( stricmp( item->name, name ) == 0 )
            return item;
    LIST_FOREACH_END

    return NULL;
}

/*=========================================================
    RwTexDictionaryCreate

    Purpose:
        Returns a newly created TexDictionary.
    Binary offsets:
        (1.0 US): 0x007F3600
        (1.0 EU): 0x007F3640
=========================================================*/
RwTexDictionary* RwTexDictionaryCreate( void )
{
    RwInterface *rwInterface = RenderWare::GetInterface();

    RwTexDictionary *txd = (RwTexDictionary*)rwInterface->m_allocStruct( rwInterface->m_textureManager.txdStruct, 0x30016 );

    if ( !txd )
        return NULL;

    txd->type = RW_TXD;
    txd->subtype = 0;
    txd->flags = 0;
    txd->privateFlags = 0;
    txd->parent = NULL;

    LIST_CLEAR( txd->textures.root );
    LIST_APPEND( RenderWare::GetInterface()->m_textureManager.globalTxd.root, txd->globalTXDs );

    // Register the txd I guess
    RwObjectRegister( (void*)0x008E23E4, txd );
    return txd;
}