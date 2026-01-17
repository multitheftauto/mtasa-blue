/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEventDamageSA.h
 *  PURPOSE:     Header file for damage event class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CEventDamage.h>
#include "CPedDamageResponseSA.h"

class CEntitySAInterface;
enum eWeaponType;
enum ePedPieceTypes;

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

class CEventDamageSAInterface
{
public:
    DWORD                         vmt;                           // 0
    DWORD                         ticks;                         // 4
    BYTE                          unk8;                          // 8
    BYTE                          unk9;                          // 9
    BYTE                          unkA;                          // Ah
    BYTE                          unkB;                          // Bh
    BYTE                          unkC;                          // Ch
    BYTE                          unkD;                          // Dh
    WORD                          unkE;                          // Eh
    WORD                          unk10;                         // 10h
    WORD                          unk12;                         // 12h
    CEntitySAInterface*           pInflictor;                    // 14h
    DWORD                         damageTime;                    // 18h
    eWeaponType                   weaponUsed;                    // 1Ch
    ePedPieceTypes                pedPieceType;                  // 20h
    BYTE                          ucDirection;                   // 24h
    BYTE                          ucFlags;                       // 25h
    BYTE                          unk26;                         // 26h
    BYTE                          unk27;                         // 27h
    DWORD                         dwAnimGroup;                   // 28h
    DWORD                         dwAnimID;                      // 2Ch
    float                         fAnimBlend;                    // 30h
    float                         fAnimSpeed;                    // 34h
    CPedDamageResponseSAInterface damageResponseData;            // 38h
};

class CEventDamageSA : public CEventDamage
{
private:
    CEventDamageSAInterface* m_pInterface;
    CPedDamageResponse*      m_pDamageResponse;
    bool                     m_bDestroyInterface;
    EDamageReasonType        m_DamageReason;

public:
    CEventDamageSA(CEntity* pEntity, unsigned int i_1, eWeaponType weaponType, ePedPieceTypes hitZone, unsigned char uc_2, bool b_3, bool b_4);
    CEventDamageSA(CEventDamageSAInterface* pInterface);
    ~CEventDamageSA();

    CEventDamageSAInterface* GetInterface() { return m_pInterface; }

    void Destroy() { delete this; }

    // Flag accessors
    bool DidPedFallDown() { return (m_pInterface->ucFlags & 2) != 0; }
    bool WasStealthAttack() { return (m_pInterface->ucFlags & 16) != 0; }

    void MakePedFallDown() { m_pInterface->ucFlags |= 2; }

    DWORD          GetDamageTime() { return m_pInterface->damageTime; }
    eWeaponType    GetWeaponUsed() { return m_pInterface->weaponUsed; }
    ePedPieceTypes GetPedPieceType() { return m_pInterface->pedPieceType; }
    char           GetDirection() { return m_pInterface->ucDirection; }

    CEntity*            GetInflictingEntity();
    CPedDamageResponse* GetDamageResponse() { return m_pDamageResponse; }

    bool              HasKilledPed();
    float             GetDamageApplied();
    AssocGroupId      GetAnimGroup();
    AnimationId       GetAnimId();
    bool              GetAnimAdded();
    void              ComputeDeathAnim(CPed* pPed, bool bUnk);
    void              ComputeDamageAnim(CPed* pPed, bool bUnk);
    bool              AffectsPed(CPed* pPed);
    void              SetDamageReason(EDamageReasonType damageReason) { m_DamageReason = damageReason; }
    EDamageReasonType GetDamageReason() { return m_DamageReason; }
};
