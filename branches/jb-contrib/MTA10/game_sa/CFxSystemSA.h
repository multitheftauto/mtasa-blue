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

// int __stdcall CFxSystem::AddParticle(CParticleFx* pThis, CVector* pVecPosition, CVector* pVecVelocity, float fBlur, CParticleInfo* pParticleInfo, int a6, float fBrightness, float a8, int a9)
#define FUNC_CFxSystem__AddParticle					0x4AA440

class CFxSystemSAInterface
{
public:
    uint32 pad1;
    uint32 pNext;
    void* pUnk; // must be initialized before this class is used
    RwMatrix* pMatrixPos;
    RwMatrix transformMatrix;
    uint8 pad2[4];
    uint32 pad3[2];
    uint16 pad4[3];
    uint8 flags;
    uint8 pad5;
    uint32 pad6;
    CVector vecUnk;
    uint32 pad7;
    class CParticleEmitterManager** pParticleEmitterManagers; // allocated array of pointers
    uint32 pad8[34];
};
C_ASSERT(sizeof(CFxSystemSAInterface) == 0x104);

class CFxSystemSA
{
public:
    CFxSystemSA(CFxSystemSAInterface* pInterface) { m_pInterface = pInterface; }
private:
    CFxSystemSAInterface * m_pInterface;
};