/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CTextureManager.h
*  PURPOSE:     Texture management definitions
*               SubInterface of RenderWare
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CTextureManager_H_
#define _CTextureManager_H_

// CStreamer supports 5000 due to hard-coding
#define MAX_TXD     5000

class CTextureManager
{
public:
    virtual int                     FindTxdEntry                        ( const char *name ) const = 0;
    virtual int                     CreateTxdEntry                      ( const char *name ) = 0;

#ifdef RENDERWARE_VIRTUAL_INTERFACES
    virtual CTexDictionary*         CreateTxd                           ( CFile *stream ) = 0;
#endif //RENDERWARE_VIRTUAL_INTERFACES

    virtual int                     LoadDictionary                      ( const char *filename ) = 0;
    virtual int                     LoadDictionaryEx                    ( const char *name, const char *filename ) = 0;

    virtual bool                    SetCurrentTexture                   ( unsigned short id ) = 0;

    virtual void                    DeallocateTxdEntry                  ( unsigned short id ) = 0;
    virtual void                    RemoveTxdEntry                      ( unsigned short id ) = 0;

#ifdef TEXTURE_MANAGER_SHADER_IMPL
    virtual unsigned short          GetTXDIDForModelID                  ( unsigned short model ) = 0;
    virtual void                    InitWorldTextureWatch               ( PFN_WATCH_CALLBACK pfnWatchCallback ) = 0;
    virtual bool                    AddWorldTextureWatch                ( CSHADERDUMMY* pShaderData, const char* szMatch, float fShaderPriority ) = 0;
    virtual void                    RemoveWorldTextureWatch             ( CSHADERDUMMY* pShaderData, const char* szMatch ) = 0;
    virtual void                    RemoveWorldTextureWatchByContext    ( CSHADERDUMMY* pShaderData ) = 0;
    virtual void                    PulseWorldTextureWatch              ( void ) = 0;
    virtual void                    GetModelTextureNames                ( std::vector < SString >& outNameList, unsigned short model ) = 0;
    virtual void                    GetTxdTextures                      ( std::vector < struct RwTexture* >& outTextureList, unsigned short txd ) = 0;
    virtual const SString&          GetTextureName                      ( CD3DDUMMY* pD3DData ) = 0;
#endif //TEXTURE_MANAGER_SHADER_IMPL
};

#endif //_CTextureManager_H_