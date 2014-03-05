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
    RenderWare::GetInterface()->m_strncpy( this->name, name, sizeof(this->name) );

    if ( RenderWare::GetInterface()->m_strlen( this->name ) >= sizeof(this->name) )
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
        _txd - the new TXD to apply this texture to
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