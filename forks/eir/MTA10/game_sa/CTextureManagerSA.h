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
#ifdef TEXTURE_MANAGER_SHADER_IMPL
#include "CTextureManagerSA.shader.h"
#endif //TEXTURE_MANAGER_SHADER_IMPL


class CTextureManagerSA : public CTextureManager
{
    friend class CTexDictionarySA;
public:
                        CTextureManagerSA           ( void );
                        ~CTextureManagerSA          ( void );

    int                 FindTxdEntry                ( const char *name ) const;
    int                 CreateTxdEntry              ( const char *name );

#ifdef RENDERWARE_VIRTUAL_INTERFACES
    CTexDictionarySA*   CreateTxd                   ( CFile *stream );
#endif //RENDERWARE_VIRTUAL_INTERFACES

    int                 LoadDictionary              ( const char *filename );
    int                 LoadDictionaryEx            ( const char *name, const char *filename );

    bool                SetCurrentTexture           ( unsigned short id );

    void                DeallocateTxdEntry          ( unsigned short id );
    void                RemoveTxdEntry              ( unsigned short id );

#ifdef TEXTURE_MANAGER_SHADER_IMPL
    ushort              GetTXDIDForModelID          ( ushort usModelID );
    void                InitWorldTextureWatch       ( PFN_WATCH_CALLBACK pfnWatchCallback );
    bool                AddWorldTextureWatch        ( CSHADERDUMMY* pShaderData, const char* szMatch, float fOrderPriority );
    void                RemoveWorldTextureWatch     ( CSHADERDUMMY* pShaderData, const char* szMatch );
    void                RemoveWorldTextureWatchByContext ( CSHADERDUMMY* pShaderData );
    void                PulseWorldTextureWatch      ( void );
    void                GetModelTextureNames        ( std::vector < SString >& outNameList, ushort usModelID );
    void                GetTxdTextures              ( std::vector < RwTexture* >& outTextureList, ushort usTxdId );
    const SString&      GetTextureName              ( CD3DDUMMY* pD3DData );
#endif //TEXTURE_MANAGER_SHADER_IMPL

private:
#ifdef TEXTURE_MANAGER_SHADER_IMPL
    void                InitShaderSystem            ( void );
    void                ShutdownShaderSystem        ( void );

    void                AddActiveTexture            ( ushort usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData );
    void                RemoveTxdActiveTextures     ( ushort usTxdId );
    void                FindNewAssociationForTexInfo( STexInfo* pTexInfo );
    STexInfo*           CreateTexInfo               ( ushort usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData );
    void                OnDestroyTexInfo            ( STexInfo* pTexInfo );
    SShadInfo*          CreateShadInfo              ( CSHADERDUMMY* pShaderData, const SString& strTextureName, float fOrderPriority );
    void                OnDestroyShadInfo           ( SShadInfo* pShadInfo );
    void                MakeAssociation             ( SShadInfo* pShadInfo, STexInfo* pTexInfo );
    void                BreakAssociation            ( SShadInfo* pShadInfo, STexInfo* pTexInfo );
#endif //TEXTURE_MANAGER_SHADER_IMPL

#ifdef RENDERWARE_VIRTUAL_INTERFACES
    // Managed textures
    RwList <CTexDictionarySA>               m_txdList;
#endif //RENDERWARE_VIRTUAL_INTERFACES

#ifdef TEXTURE_MANAGER_SHADER_IMPL
    // Watched world textures
    std::list < SShadInfo >                 m_ShadInfoList;
    std::list < STexInfo >                  m_TexInfoList;

    std::map < SString, STexInfo* >         m_UniqueTexInfoMap;
    std::map < CD3DDUMMY*, STexInfo* >      m_D3DDataTexInfoMap;
    std::map < SString, SShadInfo* >        m_UniqueShadInfoMap;
    std::multimap < float, SShadInfo* >     m_orderMap;

    PFN_WATCH_CALLBACK                      m_pfnWatchCallback;
#endif //TEXTURE_MANAGER_SHADER_IMPL
};

#ifdef RENDERWARE_VIRTUAL_INTERFACES
typedef std::list <CTextureSA*> dictImportList_t;
extern dictImportList_t g_dictImports[MAX_TXD];
#endif //RENDERWARE_VIRTUAL_INTERFACES

extern RwTexDictionary *g_textureEmitter;

#endif //_CTextureManagerSA_H_