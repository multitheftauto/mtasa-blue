/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendHierarchySA.cpp
 *  PURPOSE:     Header file for animation blend hierarchy class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

/* CAnimBlendHierarchy: data tree on animation blending?*/
#pragma once

#include <game/CAnimBlendHierarchy.h>

class CAnimBlendSequence;

#define FUNC_CAnimBlendHierarchy_SetName                     0x4CF2D0
#define FUNC_CAnimBlendHierarchy_RemoveAnimSequences         0x4CF8E0
#define FUNC_CAnimBlendHierarchy_RemoveFromUncompressedCache 0x4D42A0
#define FUNC_CAnimBlendHierarchy_RemoveQuaternionFlips       0x4CF4E0
#define FUNC_CAnimBlendHierarchy_CalculateTotalTime          0x4CF2F0

class CAnimBlendHierarchySAInterface
{
public:
    // Careful, GetIndex will not work for custom animations
    int GetIndex();

    unsigned int                   uiHashKey;
    CAnimBlendSequenceSAInterface* pSequences;
    unsigned short                 usNumSequences;
    bool                           bRunningCompressed;
    bool                           keepCompressed;
    int                            iAnimBlockID;
    float                          fTotalTime;
    DWORD*                         pLinkPtr;
    // class CLink<class CAnimBlendHierarchy *> *      pLinkPtr;
};

class CAnimBlendHierarchySA : public CAnimBlendHierarchy
{
public:
    CAnimBlendHierarchySA(CAnimBlendHierarchySAInterface* pInterface) { m_pInterface = pInterface; }
    void                            Initialize();
    void                            SetName(const char* szName);
    void                            SetSequences(CAnimBlendSequenceSAInterface* pSequences) { m_pInterface->pSequences = pSequences; }
    void                            SetNumSequences(unsigned short uNumSequences) { m_pInterface->usNumSequences = uNumSequences; }
    void                            SetRunningCompressed(bool bCompressed) { m_pInterface->bRunningCompressed = bCompressed; }
    void                            SetAnimationBlockID(int iBlockID) { m_pInterface->iAnimBlockID = iBlockID; }
    void                            RemoveAnimSequences();
    void                            RemoveFromUncompressedCache();
    void                            RemoveQuaternionFlips();
    void                            CalculateTotalTime();
    float                           GetTotalTime() const noexcept { return m_pInterface->fTotalTime; }
    CAnimBlendSequenceSAInterface*  GetSequence(DWORD dwIndex);
    CAnimBlendSequenceSAInterface*  GetSequences() { return m_pInterface->pSequences; }
    unsigned short                  GetNumSequences() { return m_pInterface->usNumSequences; }
    bool                            IsRunningCompressed() { return m_pInterface->bRunningCompressed; }
    bool                            IsCustom() { return m_pInterface->iAnimBlockID == -1; }
    int                             GetAnimBlockID() { return m_pInterface->iAnimBlockID; }
    CAnimBlendHierarchySAInterface* GetInterface() { return m_pInterface; }
    unsigned int                    GetNameHashKey() { return m_pInterface->uiHashKey; }

protected:
    CAnimBlendHierarchySAInterface* m_pInterface;
};
