/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareExtensionSA.h
*  PURPOSE:     RenderWare extension management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CRenderWareExtensionSA_H_
#define _CRenderWareExtensionSA_H_

// Maximum extension count, internal array size.
#define MAX_RW_EXTENSIONS           16

class CRwExtensionManagerSA
{
public:
                        CRwExtensionManagerSA   ( void );
                        ~CRwExtensionManagerSA  ( void );

    // todo: Possibility to register extensions

    RwExtension*        Allocate                ( unsigned int extId, unsigned int count, size_t size, unsigned int unk );
    void                Free                    ( RwExtension *ext );

    // not implemented yet. required?
    unsigned int        GetNumExtensions        ( void ) const;
};

// GTA:SA fixes
void    RpAtomicRenderAlpha                         ( RpAtomic *atom, unsigned int alpha );

// MTA extension functions
RwStream*           RwStreamCreateTranslated        ( CFile *file );
RwStream*           RwStreamCreateIsoTranslated     ( CFile *file );
RwStream*           RwStreamOpenTranslated          ( const char *path, RwStreamType type );
RwTexture*          RwTextureStreamReadEx           ( RwStream *stream );
RwTexDictionary*    RwTexDictionaryStreamReadEx     ( RwStream *stream );

#endif //_CRenderWareExtensionSA_H_