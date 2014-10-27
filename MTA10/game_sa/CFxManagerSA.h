/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFxManagerSA.h
*  PURPOSE:     Game effects handling
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#ifndef __CFXMANAGERSA_H
#define __CFXMANAGERSA_H

#include <game/CFxManager.h>

#define FUNC_FxManager_c__CreateFxSystem    0x4A9BE0
#define FUNC_FxManager_c__DestroyFxSystem   0x4A9810

class CFxSystemBPSAInterface;
class CFxSystemSAInterface;
class CFxEmitterSAInterface;
class CFxSystemSA;

class CFxMemoryPoolSAInterface 
{
    void * pData;
    unsigned int uiSize;
    int unk;
};

class CFxManagerSAInterface // FxManager_c
{
public:
    // List_c<FxSystemBP_c>
    CFxSystemBPSAInterface * pFirstFxBlueprint;     // 0x00
    CFxSystemBPSAInterface * pUnknownFxBlueprint;   // 0x04
    unsigned int             uiBlueprintCount;      // 0x08
    
    // List_c<FxSystem_c>
    CFxSystemSAInterface *   pFirstFxSystem;        // 0x0C
    CFxSystemSAInterface *   pUnknownFxSystem;      // 0x10
    unsigned int             uiFxSystemCount;       // 0x14
    
    void * pUnk;
    // List_c<FxEmitter_c>
    CFxEmitterSAInterface *  pFirstFxEmitter;       // 0x18
    CFxEmitterSAInterface *  pUnknownFxEmitter;     // 0x1C
    unsigned int             uiFxEmitterCount;      // 0x20

    int                      iTxdSlot;              // 0x24
    CVector *                pvecWindDirection;     // 0x28
    float                    fWindStrength;         // 0x2C
    int                      unk[21];               // 0x30 (at least partially) Frustrum info
    int                      iCurrentMatrix;        // 0x88
    RwMatrix *               apmatMatrices[8];      // 0x8C
    CFxMemoryPoolSAInterface fxMemoryPool;          // 0xAC
};
C_ASSERT(sizeof(CFxManagerSAInterface) == 0xB8);

class CFxManagerSA : public CFxManager
{
public:
                    CFxManagerSA( CFxManagerSAInterface * pInterface )    { m_pInterface = pInterface; }
    // CFxManager interface
    CFxSystem*      CreateFxSystem( const char * szBlueprint, const CVector & vecPosition, RwMatrix * pRwMatrixTag, unsigned char bSkipCameraFrustumCheck );
    void            DestroyFxSystem( CFxSystem* pFxSystem );
    void            OnFxSystemSAInterfaceDestroyed ( CFxSystemSAInterface* pFxSystemSAInterface );

    // CFxManagerSA methods
    CFxSystemSA*    GetFxSystem                     ( CFxSystemSAInterface* pFxSystemSAInterface );
    void            AddToList                       ( CFxSystemSAInterface* pFxSystemSAInterface, CFxSystemSA* pFxSystemSA );
    void            RemoveFromList                  ( CFxSystemSA* pFxSystemSA );

private:
    CFxManagerSAInterface *   m_pInterface;
    CFastHashMap < CFxSystemSAInterface*, CFxSystemSA* > m_FxInterfaceMap;
};

#endif
