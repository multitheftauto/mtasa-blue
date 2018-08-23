/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendHierarchySA.cpp
 *  PURPOSE:     Header file for animation blend hierarchy class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

/* CAnimBlendHierarchy: data tree on animation blending?*/

#ifndef __CAnimBlendHierarchySA_H
#define __CAnimBlendHierarchySA_H

#include <game/CAnimBlendHierarchy.h>
#include "Common.h"

#define FUNC_CAnimBlendHierarchy_SetName                     0x4CF2D0
#define FUNC_CAnimBlendHierarchy_RemoveAnimSequences         0x4CF8E0
#define FUNC_CAnimBlendHierarchy_RemoveFromUncompressedCache 0x4D42A0
#define FUNC_CAnimBlendHierarchy_RemoveQuaternionFlips       0x4CF4E0
#define FUNC_CAnimBlendHierarchy_CalculateTotalTime          0x4CF2F0
#define FUNC_CAnimBlendHierarchy_GetAnimSequence             0x4ce8f0
#define FUNC_CAnimBlendHierarchy_GetAnimSequences            0x4d1350

class CAnimBlendSequence;

class CAnimBlendHierarchySAInterface
{
public:
    // Careful, GetIndex will not work for custom animations
    int GetIndex(void);

    unsigned int                   iHashKey;
    CAnimBlendSequenceSAInterface* pSequences;
    unsigned short                 usNumSequences;
    bool                           bRunningCompressed;
    BYTE                           pad;
    int                            iAnimBlockID;
    float                          fTotalTime;
    DWORD*                         pLinkPtr;
    // class CLink<class CAnimBlendHierarchy *> *      pLinkPtr;
};

class CAnimBlendHierarchySA : public CAnimBlendHierarchy
{
public:
    CAnimBlendHierarchySA(CAnimBlendHierarchySAInterface* pInterface) { m_pInterface = pInterface; }
    void                            Initialize(void);
    void                            SetName(const char* szName);
    void                            SetSequences(CAnimBlendSequenceSAInterface* pSequences) { m_pInterface->pSequences = pSequences; }
    void                            SetNumSequences(unsigned short uNumSequences) { m_pInterface->usNumSequences = uNumSequences; }
    void                            SetRunningCompressed(bool bCompressed) { m_pInterface->bRunningCompressed = bCompressed; }
    void                            SetAnimationBlockID(int iBlockID) { m_pInterface->iAnimBlockID = iBlockID; }
    void                            RemoveAnimSequences(void);
    void                            RemoveFromUncompressedCache(void);
    void                            RemoveQuaternionFlips(void);
    void                            CalculateTotalTime(void);
    CAnimBlendSequenceSAInterface*  GetSequence(DWORD dwIndex);
    CAnimBlendSequenceSAInterface*  GetSequences(void) { return m_pInterface->pSequences; }
    unsigned short                  GetNumSequences(void) { return m_pInterface->usNumSequences; }
    bool                            isRunningCompressed(void) { return m_pInterface->bRunningCompressed; }
    int                             GetAnimBlockID(void) { return m_pInterface->iAnimBlockID; }
    CAnimBlendHierarchySAInterface* GetInterface(void) { return m_pInterface; }

protected:
    CAnimBlendHierarchySAInterface* m_pInterface;
};

#endif
