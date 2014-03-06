/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpMaterial.cpp
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
    RpMaterial::SetTexture

    Arguments:
        tex - new texture to assign to material (can be NULL)
    Purpose:
        Changes the texture of this material. If tex is NULL,
        then this material will not use a texture anymore.
    Binary offsets:
        (1.0 US): 0x0074DBC0
        (1.0 EU): 0x0074DC10
=========================================================*/
void RpMaterial::SetTexture( RwTexture *tex )
{
    // Reference our texture for usage
    if ( tex )
        tex->refs++;

    // Dereference the previous texture
    if ( this->texture )
        RwTextureDestroy( this->texture );

    // Assign the new texture
    this->texture = tex;
}

RpMaterials::RpMaterials( unsigned int max )
{
    data = (RpMaterial**)RenderWare::GetInterface()->m_memory.m_malloc( sizeof(long) * max, 0 );

    max = max;
    entries = 0;
}

RpMaterials::~RpMaterials( void )
{
    if ( data )
    {
        for ( unsigned int n = 0; n < entries; n++ )
            RpMaterialDestroy( data[n] );

        RenderWare::GetInterface()->m_memory.m_free( data );

        data = NULL;
    }

    entries = 0;
    max = 0;
}

/*=========================================================
    RpMaterials::Add

    Arguments:
        mat - new material to add to the storage
    Purpose:
        Registers another material to this storage. It fails if
        this container is full.
    Binary offsets:
        (1.0 US): 0x0074E350
        (1.0 EU): 0x0074E3A0
=========================================================*/
bool RpMaterials::Add( RpMaterial *mat )
{
    if ( entries == max )
        return false;   // We do not need RW error handlers anymore?

    // Reference it
    mat->refs++;

    data[ entries++ ] = mat;
    return true;
}

/*=========================================================
    RwLinkedMaterials::Get

    Arguments:
        index - index of the material you wish to retrieve
    Purpose:
        Returns an indexed linked material from this storage.
=========================================================*/
RwLinkedMaterial* RwLinkedMateria::Get( unsigned int index )
{
    if ( index >= count )
        return NULL;

    return (RwLinkedMaterial*)( this + 1 ) + index;
}