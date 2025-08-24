/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CEventDamage.h
 *  PURPOSE:     Event damage interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CEntity;
class CEventDamageSAInterface;
class CPed;
class CPedDamageResponse;
enum ePedPieceTypes;
enum eWeaponType;

typedef unsigned long AnimationId;
typedef unsigned long AssocGroupId;

namespace EDamageReason
{
    enum EDamageReasonType
    {
        OTHER,
        PISTOL_WHIP,
    };
};
using EDamageReason::EDamageReasonType;

class CEventDamage
{
public:
    virtual void Destroy() = 0;

    // Flag accessors
    virtual bool DidPedFallDown() = 0;
    virtual bool WasStealthAttack() = 0;

    virtual void MakePedFallDown() = 0;

    virtual DWORD          GetDamageTime() = 0;
    virtual eWeaponType    GetWeaponUsed() = 0;
    virtual ePedPieceTypes GetPedPieceType() = 0;
    virtual char           GetDirection() = 0;

    virtual CEntity*            GetInflictingEntity() = 0;
    virtual CPedDamageResponse* GetDamageResponse() = 0;

    virtual bool         HasKilledPed() = 0;
    virtual float        GetDamageApplied() = 0;
    virtual AssocGroupId GetAnimGroup() = 0;
    virtual AnimationId  GetAnimId() = 0;
    virtual bool         GetAnimAdded() = 0;
    virtual void         ComputeDeathAnim(CPed* pPed, bool bUnk) = 0;
    virtual void         ComputeDamageAnim(CPed* pPed, bool bUnk) = 0;
    virtual bool         AffectsPed(CPed* pPed) = 0;

    virtual void              SetDamageReason(EDamageReasonType damageReason) = 0;
    virtual EDamageReasonType GetDamageReason() = 0;
};
