/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCustomWeapon.h
*  PURPOSE:     header file for custom weapons.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#ifndef __CCUSTOMWEAPON_H
#define __CCUSTOMWEAPON_H
#include <logic/CCommon.h>
#include "CCustomWeaponManager.h"
#include "CWeaponStat.h"

struct SLineOfSightFlags
{
    SLineOfSightFlags ( void )
        : bCheckBuildings ( true )
        , bCheckVehicles ( true )
        , bCheckPeds ( true )
        , bCheckObjects ( true )
        , bCheckDummies ( true )
        , bSeeThroughStuff ( false )
        , bIgnoreSomeObjectsForCamera ( false )
        , bShootThroughStuff ( false )
        , bCheckCarTires ( false )
    {}
    bool bCheckBuildings;
    bool bCheckVehicles;
    bool bCheckPeds;
    bool bCheckObjects;
    bool bCheckDummies;
    bool bSeeThroughStuff;
    bool bIgnoreSomeObjectsForCamera;
    bool bShootThroughStuff;            // not used for IsLineOfSightClear
    bool bCheckCarTires;
};

enum eTargetType
{
    TARGET_TYPE_VECTOR,
    TARGET_TYPE_ENTITY,
    TARGET_TYPE_FIXED,
};

struct SWeaponConfiguration
{
    bool bShootIfTargetOutOfRange;
    bool bShootIfTargetBlocked;
    bool bDisableWeaponModel;
    bool bInstantReload;
    SLineOfSightFlags flags;
};

class CCustomWeapon : public CObject
{
public:
    CCustomWeapon                 ( CElement* pParent, CXMLNode* pNode, CObjectManager* pObjectManager, CCustomWeaponManager* pWeaponManager, eWeaponType weaponType );
    ~CCustomWeapon                ( void );

    void                    SetWeaponTarget         ( CElement * pTarget, int subTarget );
    void                    SetWeaponTarget         ( CVector vecTarget );
    void                    ResetWeaponTarget       ( void );

    inline eWeaponType      GetWeaponType           ( void )                                        { return m_Type; }
    
    inline eWeaponState     GetWeaponState          ( void )                                        { return m_State; }
    inline void             SetWeaponState          ( eWeaponState state )                          { m_State = state; }

    void                    SetClipAmmo             ( int iAmmo )                                   { m_nAmmoInClip = iAmmo; }
    int                     GetClipAmmo             ( void )                                        { return m_nAmmoInClip; }

    void                    SetAmmo                 ( int iAmmo )                                   { m_nAmmoTotal = iAmmo; }
    int                     GetAmmo                 ( void )                                        { return m_nAmmoTotal; }

    CWeaponStat *           GetWeaponStat           ( void )                                        { return m_pWeaponStat; }
    
    CPlayer *               GetOwner                ( void )                                        { return m_pOwner; }
    void                    SetOwner                ( CPlayer * pOwner )                            { m_pOwner = pOwner; }

    eTargetType             GetTargetType           ( void )                                        { return m_targetType; }
    eBone                   GetTargetBone           ( void )                                        { return m_targetBone; }
    int                     GetTargetWheel          ( void )                                        { return m_itargetWheel; }
    CVector                 GetVectorTarget         ( void )                                        { return m_vecTarget; }
    CElement *              GetElementTarget        ( void )                                        { return m_pTarget; }
    bool                    HasChangedStats         ( void )                                        { return m_pWeaponStat->HasChanged ( ); }

    bool                    SetFlags                ( eWeaponFlags flags, bool bData );
    bool                    SetFlags                ( const SLineOfSightFlags flags );
    void                    SetFlags                ( const SWeaponConfiguration weaponConfig )     { m_weaponConfig = weaponConfig; }

    bool                    GetFlags                ( eWeaponFlags flags, bool &bData );
    bool                    GetFlags                ( SLineOfSightFlags& flags );
    SWeaponConfiguration    GetFlags                ( void )                                        { return m_weaponConfig; }

    static int              GetWeaponFireTime       ( CWeaponStat * pWeaponStat );
    void                    SetWeaponFireTime       ( int iWeaponFireTime );
    int                     GetWeaponFireTime       ( void );
    void                    ResetWeaponFireTime     ( void );

private:
    eWeaponType             m_Type;
    eWeaponState            m_State;
    DWORD                   m_dwAmmoInClip;
    CVector                 m_vecTargetDirection;
    CVector                 m_vecLastDirection;
    bool                    m_bHasTargetDirection;
    short                   m_sDamage;
    CWeaponStat *           m_pWeaponStat;
    CElement *              m_pTarget;
    CVector                 m_vecTarget;
    eTargetType             m_targetType;
    eBone                   m_targetBone;
    int                     m_itargetWheel;
    SWeaponConfiguration    m_weaponConfig;
    CPlayer *               m_pOwner;  // For lag compensation
    int                     m_nAmmoTotal;
    int                     m_nAmmoInClip;
    eWeaponState            m_PreviousState;
    CElapsedTime            m_fireTimer;
    CElapsedTime            m_reloadTimer;
    unsigned char           m_ucCounter;
    int                     m_iWeaponFireRate;

    CCustomWeaponManager *  m_pWeaponManager;


};
#endif