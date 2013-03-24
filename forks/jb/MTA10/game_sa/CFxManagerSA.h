/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleSystemManagerSA.h
*  PURPOSE:     Header file for particle system manager class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include <game/CFxManager.h>

#define FUNC_CFxManager__DestroyFxSystem                                0x4A9810
#define FUNC_CFxManager__InitialiseFxSystem                             0x4A96B0
#define FUNC_CFxManager__InitialiseFxSystem2                            0x4A95C0
#define FUNC_CFxManager__FindFxSystemBP                                 0x4A9360                           
#define FUNC_CFxManager__CreateFxSystem                                 0x4A9BE0
#define FUNC_CFxManager__SetWindData                                    0x4A93E0

class CFxSystem;
class CFxSystemSA;
class CFxSystemBPSA;

class LListHead
{
    void* listHead;
    void* pad1;
    uint32 count;
};

class CFxFrustumInfoSAInterface
{
public:
    uint32 pad1[5];
    RwPlane cameraFrustumPlanes[4];
};

class SimpleBuffer
{
public:
    void* ptr;
    uint32 size;
    uint32 position;
};

class CFxManagerSAInterface
{
public:
    LListHead fxSystemBPList;
    LListHead fxSystemList;
    uint32 pad1;
    LListHead unkList;
    uint32 txdIndex;
    CVector* pVecWind;
    float fWindStrength;
    CFxFrustumInfoSAInterface frustumInfo;
    uint32 currentMatrix;
    RwMatrix* matrices[8];
    SimpleBuffer buffer;
};
C_ASSERT(sizeof(CFxManagerSAInterface) == 0xB8);

class CFxManagerSA : public virtual CFxManager
{
public:
    CFxManagerSA(CFxManagerSAInterface * pInterface);
    void SetWindData(const CVector& vecWindDirection, float fWindStrength);
    void DestroyFxSystem(CFxSystemSA* pFxSystem);
    CFxSystemSA* InitialiseFxSystem(CFxSystemBPSA* pFxSystemBP, CVector& vecPos, RwMatrix* pMatrix, bool bUnknown);
    CFxSystemSA* InitialiseFxSystem(CFxSystemBPSA* pFxSystemBP, RwMatrix* pMatrix1, RwMatrix* pMatrix2, bool bUnknown);
private:
    CFxManagerSAInterface * m_pInterface;
};
