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

    template <class type, typename returnType>
    bool                    ForAllTextures( returnType (__cdecl*callback)( RwTexture *tex, type ud ), type ud )
    {
        RwListEntry <RwTexture> *child;

        for ( child = textures.root.next; child != &textures.root; child = child->next )
        {
            if ( !callback( (RwTexture*)( (unsigned int)child - offsetof(RwTexture, TXDList) ), ud ) )
                return false;
        }

        return true;
    }

    template <class type, typename returnType>
    bool                    ForAllTexturesSafe( returnType (__cdecl*callback)( RwTexture *tex, type ud ), type ud )
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

typedef RwTexture* (__cdecl*texdictTextureIterator_t)( RwTexture *texture, void *data );

// TexDictionary API.
RwTexture* __cdecl          RwTexDictionaryAddTexture       ( RwTexDictionary *texDict, RwTexture *texture );   // US exe: 0x007F3980
RwTexture* __cdecl          RwTexDictionaryFindNamedTexture ( RwTexDictionary *texDict, const char *name );     // US exe: 0x007F39F0
RwTexDictionary* __cdecl    RwTexDictionaryForAllTextures   ( RwTexDictionary *texDict, texdictTextureIterator_t callback, void *data );    // US exe: 0x007F3730
RwTexDictionary* __cdecl    RwTexDictionaryForAllTextures   ( RwTexDictionary *texDict, void *callback, void *data );
RwTexDictionary* __cdecl    RwTexDictionarySetCurrent       ( RwTexDictionary *texDict );                       // US exe: 0x007F3A70
RwTexDictionary* __cdecl    RwTexDictionaryGetCurrent       ( void );                                           // US exe: 0x007F3A90
RwTexDictionary* __cdecl    RwTexDictionaryStreamReadEx     ( RwStream *stream );
RwTexDictionary* __cdecl    RwTexDictionaryGtaStreamRead    ( RwStream *stream );
RwTexDictionary* __cdecl    RwTexDictionaryCreate           ( void );                                           // US exe: 0x007F3600

// Swap the current txd with another
class RwTxdStack
{
public:
    AINLINE RwTxdStack( RwTexDictionary *txd )
    {
        m_txd = RwTexDictionaryGetCurrent();
        RwTexDictionarySetCurrent( txd );
    }

    AINLINE ~RwTxdStack( void )
    {
        RwTexDictionarySetCurrent( m_txd );
    }

private:
    RwTexDictionary*    m_txd;
};

#endif //_RENDERWARE_TEXDICTIONARY_