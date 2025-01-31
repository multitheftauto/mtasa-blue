/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFireSA.h
 *  PURPOSE:     Header file for fire class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/CFire.h>

class CEntitySAInterface;
class FxSystem_c;

#define FUNC_Extinguish                 0x5393F0
#define FUNC_CreateFxSysForStrength     0x539360

class CFireSAInterface
{
public:
    BYTE                bActive : 1;
    BYTE                bCreatedByScript : 1;
    BYTE                bMakesNoise : 1;
    BYTE                bBeingExtinguished : 1;
    BYTE                bFirstGeneration : 1;
    WORD                ScriptReferenceIndex;
    CVector             vecPosition;
    CEntitySAInterface* entityTarget;
    CEntitySAInterface* entityCreator;
    DWORD               nTimeToBurn;
    float               Strength;
    signed char         nNumGenerationsAllowed;
    BYTE                RemovalDist;
    FxSystem_c*         m_fxSysPtr;
};

class CFireSA : public CFire
{
private:
    CFireSAInterface* internalInterface;

public:
    CFireSA(CFireSAInterface* fireInterface) { internalInterface = fireInterface; }

    void              Extinguish();
    CVector*          GetPosition();
    void              SetPosition(CVector& vecPosition);
    void              SetTimeToBurnOut(DWORD dwTime);
    DWORD             GetTimeToBurnOut();
    CEntity*          GetCreator();
    CEntity*          GetEntityOnFire();
    void              SetTarget(CEntity* entity);
    bool              IsIgnited();
    bool              IsFree();
    void              SetSilent(bool bSilent);
    bool              IsBeingExtinguished();
    void              Ignite();
    float             GetStrength();
    void              SetStrength(float fStrength);
    void              SetNumGenerationsAllowed(char generations);
    CFireSAInterface* GetInterface() { return internalInterface; }

    static void StaticSetHooks();
};
