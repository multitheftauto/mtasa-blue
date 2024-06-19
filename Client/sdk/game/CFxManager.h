/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CFxManager.h
 *  PURPOSE:     Game effects interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CFxSystem;
class CFxSystemBPSAInterface;
class CFxSystemSAInterface;
class CVector;
struct RwMatrix;

class CFxManager
{
public:
    virtual CFxSystem* CreateFxSystem(const char* szBlueprint, const CVector& vecPosition, RwMatrix* pRwMatrixTag, unsigned char bSkipCameraFrustumCheck,
                                      bool bSoundEnable) = 0;
    virtual void       DestroyFxSystem(CFxSystem* pFxSystem) = 0;
    virtual void       OnFxSystemSAInterfaceDestroyed(CFxSystemSAInterface* pFxSystemSAInterface) = 0;
    virtual CFxSystemBPSAInterface* GetFxSystemBlueprintByName(SString sName) = 0;
    virtual bool                    IsValidFxSystemBlueprintName(SString sName) = 0;
};
