/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlockSA.h
 *  PURPOSE:     Header file for animation block class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAnimBlock.h>

class CAnimBlendAssocGroupSA;

class CAnimBlockSAInterface            // 32 bytes
{
public:
    int GetIndex();            // hacky, i know

    char           szName[16];
    bool           bLoaded;            // ?
    BYTE           pad[1];
    unsigned short usRefs;
    int            idOffset;
    size_t         nAnimations;
    DWORD          dwAssocGroup;
};

class CAnimBlockSA : public CAnimBlock
{
    friend class CAnimBlendAssocGroupSA;

public:
    CAnimBlockSA(CAnimBlockSAInterface* pInterface) { m_pInterface = pInterface; }

    CAnimBlockSAInterface*          GetInterface() { return m_pInterface; }
    char*                           GetName() { return m_pInterface->szName; }
    int                             GetIndex() { return m_pInterface->GetIndex(); }
    void                            AddRef() { m_pInterface->usRefs++; }
    unsigned short                  GetRefs() { return m_pInterface->usRefs; }
    bool                            IsLoaded() { return m_pInterface->bLoaded; }
    int                             GetIDOffset() { return m_pInterface->idOffset; }
    size_t                          GetAnimationCount() { return m_pInterface->nAnimations; }
    void                            Request(EModelRequestType requestType, bool bAllowBlockingFail = false);
    CAnimBlendHierarchySAInterface* GetAnimationHierarchyInterface(size_t iAnimation);

protected:
    CAnimBlockSAInterface* m_pInterface;
};
