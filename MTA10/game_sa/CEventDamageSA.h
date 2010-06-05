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
#include "CPedDamageResponseSA.h"

#define FUNC_CEventDamage_Constructor           0x4ad830
#define FUNC_CEventDamage_Destructor            0x4ad960
#define FUNC_CEventDamage_HasKilledPed          0x4ABCA0
#define FUNC_CEventDamage_GetDamageApplied      0x4ABCE0
#define FUNC_CEventDamage_GetAnimGroup          0x4B8060
#define FUNC_CEventDamage_GetAnimId             0x4B8070
#define FUNC_CEventDamage_GetAnimAdded          0x4b80a0
#define FUNC_CEventDamage_ComputeDeathAnim      0x4B3A60
#define FUNC_CEventDamage_ComputeDamageAnim     0x4b3fc0
#define FUNC_CEventDamage_AffectsPed            0x4b35a0

class CEntitySAInterface;
enum eWeaponType;
enum ePedPieceTypes;

class CEventDamageSAInterface
{
public:
    DWORD vmt;                                              // 0
    DWORD ticks;                                            // 4
    BYTE unk8;                                              // 8
    BYTE unk9;                                              // 9
    BYTE unkA;                                              // Ah
    BYTE unkB;                                              // Bh
    BYTE unkC;                                              // Ch
    BYTE unkD;                                              // Dh
    WORD unkE;                                              // Eh
    WORD unk10;                                             // 10h
    WORD unk12;                                             // 12h
    CEntitySAInterface *            pInflictor;             // 14h
    DWORD                           damageTime;             // 18h
    eWeaponType                     weaponUsed;             // 1Ch
    ePedPieceTypes                  pedPieceType;           // 20h
    BYTE                            ucDirection;            // 24h
    BYTE                            ucFlags;                // 25h
    BYTE unk26;                                             // 26h
    BYTE unk27;                                             // 27h
    DWORD                           dwAnimGroup;            // 28h
    DWORD                           dwAnimID;               // 2Ch
    float                           fAnimBlend;             // 30h
    float                           fAnimSpeed;             // 34h
    CPedDamageResponseSAInterface   damageResponseData;     // 38h
};

class CEventDamageSA : public CEventDamage
{
private:
    CEventDamageSAInterface *           m_pInterface;
    CPedDamageResponse *                m_pDamageResponse;
    bool                                m_bDestroyInterface;
public:
                                        CEventDamageSA      ( CEntity * pEntity, unsigned int i_1, eWeaponType weaponType, ePedPieceTypes hitZone, unsigned char uc_2, bool b_3, bool b_4 );
                                        CEventDamageSA      ( CEventDamageSAInterface * pInterface );
                                        ~CEventDamageSA     ( void );

    inline CEventDamageSAInterface *    GetInterface        ( void )        { return m_pInterface; }

    inline void                         Destroy             ( void )        { delete this; }

    // Flag accessors
    inline bool                         DidPedFallDown      ( void )                                    { return (m_pInterface->ucFlags & 2) != 0; }
    inline bool                         WasStealthAttack    ( void )                                    { return (m_pInterface->ucFlags & 16) != 0; }

    inline void                         MakePedFallDown     ( void )                                    { m_pInterface->ucFlags |= 2; }

    inline DWORD                        GetDamageTime       ( void )                                    { return m_pInterface->damageTime; }
    inline eWeaponType                  GetWeaponUsed       ( void )                                    { return m_pInterface->weaponUsed; }
    inline ePedPieceTypes               GetPedPieceType     ( void )                                    { return m_pInterface->pedPieceType; }
    inline char                         GetDirection        ( void )                                    { return m_pInterface->ucDirection; }
    
    CEntity *                           GetInflictingEntity ( void );
    inline CPedDamageResponse *         GetDamageResponse   ( void )        { return m_pDamageResponse; }

    bool                                HasKilledPed        ( void );
    float                               GetDamageApplied    ( void );
    AssocGroupId                        GetAnimGroup        ( void );
    AnimationId                         GetAnimId           ( void );
    bool                                GetAnimAdded        ( void );
    void                                ComputeDeathAnim    ( CPed * pPed, bool bUnk );
    void                                ComputeDamageAnim   ( CPed * pPed, bool bUnk );
    bool                                AffectsPed          ( CPed * pPed );
};

#endif