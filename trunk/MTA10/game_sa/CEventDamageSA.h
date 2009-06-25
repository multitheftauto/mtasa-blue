/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CEventDamageSA.h
*  PURPOSE:		Header file for damage event class
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CEventDamageSA_H
#define __CEventDamageSA_H

#include <windows.h>
#include <game/CEventDamage.h>
#include "CEntitySA.h"

#define FUNC_CEventDamage_HasKilledPed          0x4ABCA0
#define FUNC_CEventDamage_GetDamageApplied      0x4ABCE0
#define FUNC_CEventDamage_GetAnimGroup          0x4B8060
#define FUNC_CEventDamage_GetAnimId             0x4B8070
#define FUNC_CEventDamage_GetAnimAdded          0x4b80a0
#define FUNC_CEventDamage_ComputeDeathAnim      0x4B3A60
#define FUNC_CEventDamage_ComputeDamageAnim     0x4b3fc0
#define FUNC_CEventDamage_Destructor            0x4ad960

class CEntitySAInterface;
enum eWeaponType;
enum ePedPieceTypes;

class CEventDamageSAInterface
{
public:
    DWORD unk;                                  // 0
    DWORD unk2;                                 // 4
    DWORD unk3;                                 // 8
    DWORD unk4;                                 // 12
    DWORD unk5;                                 // 16
    CEntitySAInterface *        inflictor;      // 20
    DWORD                       damageTime;     // 24
    eWeaponType                 weaponUsed;     // 28
    ePedPieceTypes              pedPieceType;   // 32
    char                        cDirection;     // 36
    char                        cFlags;         // 37
    char pad [ 30 ];
};

class CEventDamageSA : public CEventDamage
{
private:
    CEventDamageSAInterface *           m_pInterface;
public:

    inline                              CEventDamageSA      ( CEventDamageSAInterface * pInterface )    { m_pInterface = pInterface; }

    inline CEventDamageSAInterface *    GetInterface        ( void )        { return m_pInterface; }

    void                                Destroy             ( bool bDestroyInterface );

    // Flag accessors
    inline bool                         DidPedFallDown      ( void )                                    { return (m_pInterface->cFlags & 2) == 1; }
    inline bool                         WasStealthAttack    ( void )                                    { return (m_pInterface->cFlags & 5) == 1; }

    inline void                         MakePedFallDown     ( void )                                    { m_pInterface->cFlags |= 2; }

    inline DWORD                        GetDamageTime       ( void )                                    { return m_pInterface->damageTime; }
    inline eWeaponType                  GetWeaponUsed       ( void )                                    { return m_pInterface->weaponUsed; }
    inline ePedPieceTypes               GetPedPieceType     ( void )                                    { return m_pInterface->pedPieceType; }
    inline char                         GetDirection        ( void )                                    { return m_pInterface->cDirection; }
    
    CEntity *                           GetInflictingEntity ( void );

    bool                                HasKilledPed        ( void );
    float                               GetDamageApplied    ( void );
    AssocGroupId                        GetAnimGroup        ( void );
    AnimationId                         GetAnimId           ( void );
    bool                                GetAnimAdded        ( void );
    void                                ComputeDeathAnim    ( CPed * pPed, bool bUnk );
    void                                ComputeDamageAnim   ( CPed * pPed, bool bUnk );
};

#endif