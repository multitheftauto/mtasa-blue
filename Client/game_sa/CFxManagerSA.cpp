/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFxManagerSA.cpp
 *  PURPOSE:     Game effects handling
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CFxSystem* CFxManagerSA::CreateFxSystem(const char* szBlueprint, const CVector& vecPosition, RwMatrix* pRwMatrixTag, unsigned char bSkipCameraFrustumCheck,
                                        bool bSoundEnable)
{

    // FxManager_c::CreateFxSystem
    CFxSystemSAInterface* pFxSystem = ((CFxSystemSAInterface*(__thiscall*)(CFxManagerSAInterface*, const char*, const CVector&, RwMatrix*, unsigned char))
                  FUNC_FxManager_c__CreateFxSystem)(m_pInterface, szBlueprint, vecPosition, pRwMatrixTag, bSkipCameraFrustumCheck);

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
    // FxManager_c::DestroyFxSystem
    ((void(__thiscall*)(CFxManagerSAInterface*, void*))FUNC_FxManager_c__DestroyFxSystem)(m_pInterface, pFxSystem->GetInterface());
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
    using func_t = CFxSystemBPSAInterface*(__thiscall*)(CFxManagerSAInterface * pInterface, const char* pChars);
    auto func = reinterpret_cast<func_t>(FUNC_FxManager_c__GetSystemByName);
    return func(m_pInterface, sName);
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
