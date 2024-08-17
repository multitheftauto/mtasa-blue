/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPed.h
 *  PURPOSE:     Ped entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <memory>
#include "Common.h"
#include "CPhysical.h"
#include "CWeaponInfo.h"

class CObject;
class CPedIK;
class CPedIKSAInterface;
class CPedIntelligence;
class CPedSound;
class CTaskManager;
class CVehicle;
class CWeapon;
class CWeaponStat;
class CProjectileSAInterface;

enum ePedPieceTypes
{
    PED_PIECE_UNKNOWN = 0,

    PED_PIECE_TORSO = 3,
    PED_PIECE_ASS,
    PED_PIECE_LEFT_ARM,
    PED_PIECE_RIGHT_ARM,
    PED_PIECE_LEFT_LEG,
    PED_PIECE_RIGHT_LEG,
    PED_PIECE_HEAD
};

enum eBone
{
    BONE_ROOT = 0,
    BONE_PELVIS1,
    BONE_PELVIS,
    BONE_SPINE1,
    BONE_UPPERTORSO,
    BONE_NECK,
    BONE_HEAD2,
    BONE_HEAD1,
    BONE_HEAD,
    BONE_RIGHTUPPERTORSO = 21,
    BONE_RIGHTSHOULDER,
    BONE_RIGHTELBOW,
    BONE_RIGHTWRIST,
    BONE_RIGHTHAND,
    BONE_RIGHTTHUMB,
    BONE_LEFTUPPERTORSO = 31,
    BONE_LEFTSHOULDER,
    BONE_LEFTELBOW,
    BONE_LEFTWRIST,
    BONE_LEFTHAND,
    BONE_LEFTTHUMB,
    BONE_LEFTHIP = 41,
    BONE_LEFTKNEE,
    BONE_LEFTANKLE,
    BONE_LEFTFOOT,
    BONE_RIGHTHIP = 51,
    BONE_RIGHTKNEE,
    BONE_RIGHTANKLE,
    BONE_RIGHTFOOT,
    BONE_BELLY = 201,
    BONE_RIGHTBREAST = 301,
    BONE_LEFTBREAST = 302,
};

enum
{
    BIKE_KNOCK_OFF_DEFAULT = 0,
    BIKE_KNOCK_OFF_NEVER,
    BIKE_KNOCK_OFF_ALWAYS_NORMAL,
};

enum
{
    ATTACH_DIRECTION_FRONT = 0,
    ATTACH_DIRECTION_LEFT,
    ATTACH_DIRECTION_BACK,
    ATTACH_DIRECTION_RIGHT
};

enum eFightingStyle
{
    STYLE_STANDARD = 4,
    STYLE_BOXING,
    STYLE_KUNG_FU,
    STYLE_KNEE_HEAD,
    // various melee weapon styles
    STYLE_GRAB_KICK = 15,
    STYLE_ELBOWS = 16,
};

enum eMoveAnim
{
    MOVE_DEFAULT = 0,
    MOVE_PLAYER = 54,
    MOVE_PLAYER_F,
    MOVE_PLAYER_M,
    MOVE_ROCKET,
    MOVE_ROCKET_F,
    MOVE_ROCKET_M,
    MOVE_ARMED,
    MOVE_ARMED_F,
    MOVE_ARMED_M,
    MOVE_BBBAT,
    MOVE_BBBAT_F,
    MOVE_BBBAT_M,
    MOVE_CSAW,
    MOVE_CSAW_F,
    MOVE_CSAW_M,
    MOVE_SNEAK,
    MOVE_JETPACK,
    MOVE_MAN = 118,
    MOVE_SHUFFLE,
    MOVE_OLDMAN,
    MOVE_GANG1,
    MOVE_GANG2,
    MOVE_OLDFATMAN,
    MOVE_FATMAN,
    MOVE_JOGGER,
    MOVE_DRUNKMAN,
    MOVE_BLINDMAN,
    MOVE_SWAT,
    MOVE_WOMAN,
    MOVE_SHOPPING,
    MOVE_BUSYWOMAN,
    MOVE_SEXYWOMAN,
    MOVE_PRO,
    MOVE_OLDWOMAN,
    MOVE_FATWOMAN,
    MOVE_JOGWOMAN,
    MOVE_OLDFATWOMAN,
    MOVE_SKATE,
};

enum eLandedPedFoot
{
    LANDED_PED_RIGHT_FOOT = 0,
    LANDED_PED_LEFT_FOOT = 1,
};

inline bool IsValidMoveAnim(uint iMoveAnim)
{
    return (iMoveAnim == MOVE_DEFAULT) || (iMoveAnim >= MOVE_PLAYER && iMoveAnim <= MOVE_JETPACK) || (iMoveAnim >= MOVE_MAN && iMoveAnim <= MOVE_SKATE);
}

enum
{
    PLAYER_PED,
    CIVILIAN_PED
};

namespace EPedWeaponAudioEvent
{
    enum EPedWeaponAudioEventType
    {
        FIRE = 0x91,
    };
}
using EPedWeaponAudioEvent::EPedWeaponAudioEventType;

struct SSatchelsData
{
    CProjectileSAInterface* pProjectileInterface;
    CVector*                vecAttachedOffsets;
    CVector*                vecAttachedRotation;
};

class CPed : public virtual CPhysical
{
public:
    virtual ~CPed(){};

    virtual class CPedSAInterface* GetPedInterface() = 0;

    virtual void DetachPedFromEntity() = 0;

    virtual CVehicle* GetVehicle() = 0;
    virtual void      Respawn(CVector* position, bool cameraCut) = 0;

    virtual void SetModelIndex(DWORD modelIndex) = 0;
    virtual void RemoveGeometryRef() = 0;

    virtual float    GetHealth() = 0;
    virtual void     SetHealth(float health) = 0;
    virtual float    GetArmor() = 0;
    virtual void     SetArmor(float armor) = 0;
    virtual float    GetOxygenLevel() = 0;
    virtual void     SetOxygenLevel(float oxyggen) = 0;
    virtual bool     AddProjectile(eWeaponType weaponType, CVector origin, float force, CVector* target, CEntity* targetEntity) = 0;
    virtual CWeapon* GiveWeapon(eWeaponType weaponType, std::uint32_t ammo, eWeaponSkill weaponSkill) = 0;
    virtual CWeapon* GetWeapon(eWeaponSlot weaponSlot) = 0;
    virtual CWeapon* GetWeapon(eWeaponType weaponType) = 0;
    virtual void     ClearWeapons() = 0;
    virtual void     RemoveWeaponModel(int model) = 0;
    virtual void     ClearWeapon(eWeaponType weaponType) = 0;

    virtual void              SetIsStanding(bool standing) = 0;
    virtual DWORD             GetType() = 0;
    virtual CPedIntelligence* GetPedIntelligence() = 0;
    virtual CPedSound*        GetPedSound() = 0;

    virtual float       GetCurrentRotation() = 0;
    virtual float       GetTargetRotation() = 0;
    virtual void        SetCurrentRotation(float rotation) = 0;
    virtual void        SetTargetRotation(float rotation) = 0;
    virtual eWeaponSlot GetCurrentWeaponSlot() = 0;
    virtual void        SetCurrentWeaponSlot(eWeaponSlot weaponSlot) = 0;

    virtual CVector* GetBonePosition(eBone bone, CVector* position) = 0;
    virtual CVector* GetTransformedBonePosition(eBone bone, CVector* position) = 0;

    virtual bool IsDucking() = 0;
    virtual void SetDucking(bool duck) = 0;
    virtual bool IsInWater() = 0;
    virtual int  GetCantBeKnockedOffBike() = 0;
    virtual void SetCantBeKnockedOffBike(int cantBeKnockedOffBike) = 0;

    virtual void SetBleeding(bool bleeding) = 0;

    virtual bool IsWearingGoggles() = 0;
    virtual void SetGogglesState(bool isWearingThem) = 0;

    virtual void SetClothesTextureAndModel(const char* texture, const char* model, int textureType) = 0;
    virtual void RebuildPlayer() = 0;

    virtual eFightingStyle GetFightingStyle() = 0;
    virtual void           SetFightingStyle(eFightingStyle style, std::uint8_t styleExtra) = 0;

    virtual CEntity* GetContactEntity() = 0;

    virtual std::uint8_t GetRunState() = 0;

    virtual CEntity* GetTargetedEntity() = 0;
    virtual void     SetTargetedEntity(CEntity* targetEntity) = 0;

    virtual bool GetCanBeShotInVehicle() = 0;
    virtual bool GetTestForShotInVehicle() = 0;

    virtual void SetCanBeShotInVehicle(bool shot) = 0;
    virtual void SetTestForShotInVehicle(bool test) = 0;

    virtual std::uint8_t GetOccupiedSeat() const noexcept = 0;
    virtual void SetOccupiedSeat(std::uint8_t seat) noexcept = 0;

    virtual void RemoveBodyPart(std::uint8_t boneID, std::uint8_t direction) = 0;

    virtual void         SetFootBlood(std::uint32_t footBlood) = 0;
    virtual std::uint32_t GetFootBlood() = 0;

    virtual bool IsOnFire() = 0;
    virtual void SetOnFire(bool onFire) = 0;

    virtual bool GetStayInSamePlace() = 0;
    virtual void SetStayInSamePlace(bool stay) = 0;

    virtual void GetVoice(std::int16_t* voiceType, std::int16_t* voiceID) = 0;
    virtual void GetVoice(const char** voiceType, const char** voice) = 0;
    virtual void SetVoice(std::int16_t voiceType, std::int16_t voiceID) = 0;
    virtual void SetVoice(const char* voiceType, const char* voice) = 0;
    virtual void ResetVoice() = 0;
    virtual void SetLanding(bool isLanding) = 0;
    virtual void SetUpdateMetricsRequired(bool required) = 0;

    virtual CWeaponStat* GetCurrentWeaponStat() = 0;
    virtual float        GetCurrentWeaponRange() = 0;
    virtual void         AddWeaponAudioEvent(EPedWeaponAudioEventType audioEventType) = 0;

    virtual int  GetCustomMoveAnim() = 0;
    virtual bool IsDoingGangDriveby() = 0;

    virtual CPedIKSAInterface*      GetPedIKInterface() = 0;
    virtual void*                   GetPedNodeInterface(std::int32_t nodeId) = 0;
    virtual std::unique_ptr<CPedIK> GetPedIK() = 0;

    virtual void GetAttachedSatchels(std::vector<SSatchelsData> &satchelsList) const = 0;
};
