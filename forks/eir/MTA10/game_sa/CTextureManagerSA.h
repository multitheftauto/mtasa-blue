/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTextureManagerSA.h
*  PURPOSE:     Internal texture management definitions
*               SubInterface of RenderWare
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CTextureManagerSA_H_
#define _CTextureManagerSA_H_

#include <game/CTextureManager.h>

class CTexDictionarySA;

#include "CTextureManagerSA.instance.h"


class CTextureManagerSA : public CTextureManager
{
    friend class CTexDictionarySA;
public:
                        CTextureManagerSA           ( void );
                        ~CTextureManagerSA          ( void );

    int                 FindTxdEntry                ( const char *name ) const;
    int                 CreateTxdEntry              ( const char *name );

    CTexDictionarySA*   CreateTxd                   ( CFile *stream );

    int                 LoadDictionary              ( const char *filename );
    int                 LoadDictionaryEx            ( const char *name, const char *filename );

    bool                SetCurrentTexture           ( unsigned short id );

    void                DeallocateTxdEntry          ( unsigned short id );
    void                RemoveTxdEntry              ( unsigned short id );

    void                InitGameHooks               ( void );

private:
    // Managed textures
    RwList <CTexDictionarySA>               m_txdList;
};

// Definition of the pool containing all texture dictionaries of the game.
typedef CPool <CTxdInstanceSA, MAX_TXD> CTxdPool;

namespace TextureManager
{
    inline CTxdPool*&   GetTxdPool                  ( void )            { return *(CTxdPool**)0x00C8800C; }
};

typedef std::list <CTextureSA*> dictImportList_t;

extern dictImportList_t g_dictImports[MAX_TXD];
extern RwTexDictionary *g_textureEmitter;

#endif //_CTextureManagerSA_H_