/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPickup.h
 *  PURPOSE:     Pickup entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPickup;

#pragma once

#include "CColCallback.h"
#include "CColManager.h"
#include "CColSphere.h"
#include "CElement.h"
#include "CEvents.h"

class CPickup final : public CElement, private CColCallback
{
    friend class CPickupManager;

public:
    enum
    {
        HEALTH,
        ARMOR,
        WEAPON,
        CUSTOM,
        INVALID = 0xFF,
    };

    enum
    {
        WEAPON_BRASSKNUCKLE = 1,
        WEAPON_GOLFCLUB,
        WEAPON_NIGHTSTICK,
        WEAPON_KNIFE,
        WEAPON_BASEBALLBAT,
        WEAPON_SHOVEL,
        WEAPON_POOL_CUE,
        WEAPON_KATANA,
        WEAPON_CHAINSAW,

        WEAPON_DILDO1,
        WEAPON_DILDO2,
        WEAPON_VIBE1,
        WEAPON_VIBE2,
        WEAPON_FLOWERS,
        WEAPON_CANE,

        WEAPON_GRENADE,
        WEAPON_TEARGAS,
        WEAPON_MOLOTOV,
        WEAPON_FREEFALL_BOMB = 21,

        WEAPON_PISTOL,
        WEAPON_PISTOL_SILENCED,
        WEAPON_DESERT_EAGLE,
        WEAPON_SHOTGUN,
        WEAPON_SAWNOFF_SHOTGUN,
        WEAPON_SPAS12_SHOTGUN,
        WEAPON_MICRO_UZI,
        WEAPON_MP5,
        WEAPON_AK47,
        WEAPON_M4,
        WEAPON_TEC9,

        WEAPON_COUNTRYRIFLE,
        WEAPON_SNIPERRIFLE,
        WEAPON_ROCKETLAUNCHER,
        WEAPON_ROCKETLAUNCHER_HS,
        WEAPON_FLAMETHROWER,
        WEAPON_MINIGUN,
        WEAPON_REMOTE_SATCHEL_CHARGE,
        WEAPON_DETONATOR,
        WEAPON_SPRAYCAN,
        WEAPON_EXTINGUISHER,
        WEAPON_CAMERA,
        WEAPON_NIGHTVISION,
        WEAPON_INFRARED,
        WEAPON_PARACHUTE,

        WEAPON_ARMOUR = 48,

        WEAPON_INVALID = 0xFF,
    };

    CPickup(CElement* pParent, class CPickupManager* pPickupManager, CColManager* pColManager);
    ~CPickup();
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    bool IsEntity() { return true; }

    void Unlink();

    void SetPosition(const CVector& vecPosition);

    unsigned char GetPickupType() { return m_ucType; };
    void          SetPickupType(unsigned char ucType);
    bool          IsTypeRandom() { return m_bIsTypeRandom; };

    unsigned char GetWeaponType() { return m_ucWeaponType; };
    void          SetWeaponType(unsigned char ucWeaponType);
    bool          IsWeaponTypeRandom() { return m_bIsWeaponTypeRandom; };

    unsigned short GetAmmo() { return m_usAmmo; };
    void           SetAmmo(unsigned short usAmmo) { m_usAmmo = usAmmo; };

    float GetAmount() { return m_fAmount; }
    void  SetAmount(float fAmount) { m_fAmount = fAmount; }
    bool  IsHealthRandom() { return m_bIsHealthRandom; };

    unsigned long GetRespawnIntervals() { return m_ulRespawnIntervals; };
    void          SetRespawnIntervals(unsigned long ulRespawnIntervals) { m_ulRespawnIntervals = ulRespawnIntervals; };

    CTickCount GetLastUsedTime() { return m_LastUsedTime; }
    CTickCount GetCreationTime() { return m_CreationTime; }

    unsigned short GetModel() { return m_usModel; };
    void           SetModel(unsigned short usModel) { m_usModel = usModel; };

    bool IsVisible() { return m_bVisible; };
    void SetVisible(bool bVisible);

    void Randomize();

    bool CanUse(class CPlayer& Player, bool bOnfootCheck = true);
    void Use(class CPlayer& Player);

    bool IsSpawned() { return m_bSpawned; }
    void SetSpawned(bool bSpawned) { m_bSpawned = bSpawned; }

    CColShape* GetColShape() { return m_pCollision; }
    void       SetEnabled(bool bEnabled)
    {
        if (m_pCollision)
            m_pCollision->SetEnabled(bEnabled);
    }
    bool IsEnabled() { return m_pCollision && m_pCollision->IsEnabled(); }

    void SetDoneDelayHack(bool bDone) { m_bDoneDelayHack = bDone; }
    bool HasDoneDelayHack() { return m_bDoneDelayHack; }

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    void Callback_OnCollision(CColShape& Shape, CElement& Element);
    void Callback_OnLeave(CColShape& Shape, CElement& Element);
    void Callback_OnCollisionDestroy(CColShape* pShape);

    class CPickupManager* m_pPickupManager;

    unsigned char  m_ucType;
    unsigned char  m_ucWeaponType;
    unsigned short m_usAmmo;
    float          m_fAmount;
    unsigned long  m_ulRespawnIntervals;
    CTickCount     m_LastUsedTime;
    CTickCount     m_CreationTime;
    unsigned short m_usModel;
    bool           m_bVisible;

    bool m_bIsTypeRandom;
    bool m_bIsWeaponTypeRandom;
    bool m_bIsHealthRandom;

    CColSphere* m_pCollision;

    bool m_bSpawned;
    bool m_bDoneDelayHack;
};
