/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwTexDictionary.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_TEXDICTIONARY_
#define _RENDERWARE_TEXDICTIONARY_

struct RwTexDictionary : public RwObject
{
    RwList <RwTexture>              textures;
    RwListEntry <RwTexDictionary>   globalTXDs;
    RwTexDictionary*                m_parentTxd;

    template <class type>
    bool                    ForAllTextures( bool (*callback)( RwTexture *tex, type ud ), type ud )
    {
        RwListEntry <RwTexture> *child;

        for ( child = textures.root.next; child != &textures.root; child = child->next )
        {
            if ( !callback( (RwTexture*)( (unsigned int)child - offsetof(RwTexture, TXDList) ), ud ) )
                return false;
        }

        return true;
    }

    template <class type>
    bool                    ForAllTexturesSafe( bool (*callback)( RwTexture *tex, type ud ), type ud )
    {
        RwListEntry <RwTexture> *child = textures.root.next;

        while ( child != &textures.root )
        {
            RwListEntry <RwTexture> *nchild = child->next;

            if ( !callback( LIST_GETITEM( RwTexture, child, TXDList ), ud ) )
                return false;

            LIST_VALIDATE( *nchild );

            child = nchild;
        }

        return true;
    }

    RwTexture*              GetFirstTexture( void );
    RwTexture*              FindNamedTexture( const char *name );
};

// TexDictionary API.
RwTexDictionary*    RwTexDictionaryCreate( void );      // US exe: 0x007F3600

#endif //_RENDERWARE_TEXDICTIONARY_