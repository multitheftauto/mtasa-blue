/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CFxManagerSA.cpp
 *  PURPOSE:     Game effects handling
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CFxManagerSA.h"
#include "CFxSystemSA.h"

CFxSystem* CFxManagerSA::CreateFxSystem(const char* szBlueprint, const CVector& vecPosition, RwMatrix* pRwMatrixTag, unsigned char bSkipCameraFrustumCheck,
                                        bool bSoundEnable)
{
    const CVector*        pvecPosition = &vecPosition;
    DWORD                 dwThis = (DWORD)m_pInterface;
    DWORD                 dwFunc = 0x4A9BE0;
    CFxSystemSAInterface* pFxSystem = 0;

    _asm
    {
        mov     ecx, dwThis
        push    bSkipCameraFrustumCheck
        push    pRwMatrixTag
        push    pvecPosition
        push    szBlueprint
        call    dwFunc
        mov     pFxSystem, eax
    }

    if (pFxSystem)
    {
        if (!bSoundEnable)
            pFxSystem->audioEntity.audio = nullptr;

        CFxSystemSA* pFxSystemSA = new CFxSystemSA(pFxSystem);
        return pFxSystemSA;
    }
    return nullptr;
}

void CFxManagerSA::DestroyFxSystem(CFxSystem* pFxSystem)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x4A9810;
    void* pFxSA = pFxSystem->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    pFxSA
        call    dwFunc
    }
}

//
// Called when GTA deletes an FxSystem
//
void CFxManagerSA::OnFxSystemSAInterfaceDestroyed(CFxSystemSAInterface* pFxSystemSAInterface)
{
    // Delete our wrapper object if we have one
    CFxSystemSA* pFxSystemSA = GetFxSystem(pFxSystemSAInterface);
    if (pFxSystemSA)
        delete pFxSystemSA;
}

CFxSystemBPSAInterface* CFxManagerSA::GetFxSystemBlueprintByName(SString sName)
{
    return (reinterpret_cast<CFxSystemBPSAInterface*(__thiscall*)(CFxManagerSAInterface * pInterface, const char* pChars)>(0x4A9360))(m_pInterface, sName);
}

bool CFxManagerSA::IsValidFxSystemBlueprintName(SString sName)
{
    return GetFxSystemBlueprintByName(sName) != nullptr;
}

//
// AddToList/RemoveFromList called from CFxSystemSA constructor/destructor
//
void CFxManagerSA::AddToList(CFxSystemSAInterface* pFxSystemSAInterface, CFxSystemSA* pFxSystemSA)
{
    MapSet(m_FxInterfaceMap, pFxSystemSAInterface, pFxSystemSA);
}

void CFxManagerSA::RemoveFromList(CFxSystemSA* pFxSystemSA)
{
    MapRemoveByValue(m_FxInterfaceMap, pFxSystemSA);
}

//
// Find our wrapper object for the GTA object
//
CFxSystemSA* CFxManagerSA::GetFxSystem(CFxSystemSAInterface* pFxSystemSAInterface)
{
    return MapFindRef(m_FxInterfaceMap, pFxSystemSAInterface);
}
