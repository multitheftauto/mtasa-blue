/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPickup.h
*  PURPOSE:     Pickup entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPickup;

#ifndef __CPICKUP_H
#define __CPICKUP_H

#include "CColCallback.h"
#include "CColManager.h"
#include "CColSphere.h"
#include "CElement.h"
#include "CEvents.h"

class CPickup : public CElement, private CColCallback
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

                                    CPickup                     ( CElement* pParent, CXMLNode* pNode, class CPickupManager* pPickupManager, CColManager* pColManager );
                                    ~CPickup                    ( void );

    bool                            IsEntity                    ( void )                    { return true; }

    void                            Unlink                      ( void );
    bool                            ReadSpecialData             ( void );

    void                            SetPosition                 ( const CVector& vecPosition );

    inline unsigned char            GetPickupType               ( void )                                { return m_ucType; };
    void                            SetPickupType               ( unsigned char ucType );
    inline bool                     IsTypeRandom                ( void )                                { return m_bIsTypeRandom; };

    inline unsigned char            GetWeaponType               ( void )                                { return m_ucWeaponType; };
    void                            SetWeaponType               ( unsigned char ucWeaponType );
    inline bool                     IsWeaponTypeRandom          ( void )                                { return m_bIsWeaponTypeRandom; };

    inline unsigned short           GetAmmo                     ( void )                                { return m_usAmmo; };
    inline void                     SetAmmo                     ( unsigned short usAmmo )               { m_usAmmo = usAmmo; };

    inline float                    GetAmount                   ( void )                                { return m_fAmount; }
    inline void                     SetAmount                   ( float fAmount )                       { m_fAmount = fAmount; }
    inline bool                     IsHealthRandom              ( void )                                { return m_bIsHealthRandom; };

    inline unsigned long            GetRespawnIntervals         ( void )                                { return m_ulRespawnIntervals; };
    inline void                     SetRespawnIntervals         ( unsigned long ulRespawnIntervals )    { m_ulRespawnIntervals = ulRespawnIntervals; };

    inline unsigned long            GetLastUsedTime             ( void )                                { return m_ulLastUsedTime; }

    inline unsigned short           GetModel                    ( void )                                { return m_usModel; };
    inline void                     SetModel                    ( unsigned short usModel )              { m_usModel = usModel; };

    inline bool                     IsVisible                   ( void )                                { return m_bVisible; };
    void                            SetVisible                  ( bool bVisible );

    void                            Randomize                   ( void );

    bool                            CanUse                      ( class CPlayer& Player, bool bOnfootCheck = true );
    void                            Use                         ( class CPlayer& Player );

    inline bool                     IsSpawned                   ( void )                                { return m_bSpawned; }
    inline void                     SetSpawned                  ( bool bSpawned )                       { m_bSpawned = bSpawned; }

    inline CColShape*               GetColShape                 ( void )                                { return m_pCollision; }

private:
    void                            Callback_OnCollision        ( CColShape& Shape, CElement& Element );
    void                            Callback_OnLeave            ( CColShape& Shape, CElement& Element );
    void                            Callback_OnCollisionDestroy ( CColShape* pShape );

    class CPickupManager*           m_pPickupManager;

    unsigned char                   m_ucType;
    unsigned char                   m_ucWeaponType;
    unsigned short                  m_usAmmo;
    float                           m_fAmount;
    unsigned long                   m_ulRespawnIntervals;
    unsigned long                   m_ulLastUsedTime;
    unsigned short                  m_usModel;
    bool                            m_bVisible;

    bool                            m_bIsTypeRandom;
    bool                            m_bIsWeaponTypeRandom;
    bool                            m_bIsHealthRandom;

    CColSphere*                     m_pCollision;

    bool                            m_bSpawned;
};

#endif
