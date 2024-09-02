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
#include "CPhysical.h"
#include "CWeaponInfo.h"

// To avoid VS intellisense highlight errors
#include <memory>
#include <vector>

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

inline bool IsValidMoveAnim(std::uint32_t iMoveAnim)
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

    virtual CVehicle* GetVehicle() const noexcept = 0;
    virtual void      Respawn(const CVector* position, bool cameraCut) = 0;

    virtual void SetModelIndex(unsigned long ulModel) = 0;

    virtual float    GetHealth() const = 0;
    virtual void     SetHealth(float health) = 0;
    virtual float    GetArmor() const = 0;
    virtual void     SetArmor(float armor) = 0;
    virtual float    GetOxygenLevel() const = 0;
    virtual void     SetOxygenLevel(float oxygen) = 0;
    virtual bool     AddProjectile(eWeaponType weaponType, CVector origin, float force, const CVector* target, const CEntity* targetEntity) = 0;
    virtual CWeapon* GiveWeapon(eWeaponType weaponType, std::uint32_t ammo, eWeaponSkill weaponSkill) = 0;
    virtual CWeapon* GetWeapon(eWeaponSlot weaponSlot) const noexcept = 0;
    virtual CWeapon* GetWeapon(eWeaponType weaponType) const noexcept = 0;
    virtual void     ClearWeapons() noexcept = 0;
    virtual void     RemoveWeaponModel(int model) = 0;
    virtual void     ClearWeapon(eWeaponType weaponType) = 0;

    virtual void              SetIsStanding(bool standing) = 0;
    virtual DWORD             GetType() const noexcept = 0;
    virtual CPedIntelligence* GetPedIntelligence() const noexcept = 0;
    virtual CPedSound*        GetPedSound() const noexcept = 0;

    virtual float       GetCurrentRotation() const = 0;
    virtual float       GetTargetRotation() const = 0;
    virtual void        SetCurrentRotation(float rotation) = 0;
    virtual void        SetTargetRotation(float rotation) = 0;
    virtual eWeaponSlot GetCurrentWeaponSlot() const = 0;
    virtual void        SetCurrentWeaponSlot(eWeaponSlot weaponSlot) = 0;

    virtual CVector* GetBonePosition(eBone bone, CVector* position) = 0;
    virtual CVector* GetTransformedBonePosition(eBone bone, CVector* position) = 0;

    virtual bool IsDucking() const = 0;
    virtual void SetDucking(bool duck) = 0;
    virtual bool IsInWater() const noexcept = 0;
    virtual int  GetCantBeKnockedOffBike() const = 0;
    virtual void SetCantBeKnockedOffBike(int cantBeKnockedOffBike) = 0;

    virtual void SetBleeding(bool bleeding) = 0;

    virtual bool IsWearingGoggles() const = 0;
    virtual void SetGogglesState(bool isWearingThem) = 0;

    virtual void SetClothesTextureAndModel(const char* texture, const char* model, int textureType) = 0;
    virtual void RebuildPlayer() = 0;

    virtual eFightingStyle GetFightingStyle() const = 0;
    virtual void           SetFightingStyle(eFightingStyle style, std::uint8_t styleExtra) = 0;

    virtual CEntity* GetContactEntity() const noexcept = 0;

    virtual std::uint8_t GetRunState() const = 0;

    virtual CEntity* GetTargetedEntity() const noexcept = 0;
    virtual void     SetTargetedEntity(CEntity* targetEntity) = 0;

    virtual bool GetCanBeShotInVehicle() const = 0;
    virtual bool GetTestForShotInVehicle() const = 0;

    virtual void SetCanBeShotInVehicle(bool shot) = 0;
    virtual void SetTestForShotInVehicle(bool test) = 0;

    virtual std::uint8_t GetOccupiedSeat() const noexcept = 0;
    virtual void SetOccupiedSeat(std::uint8_t seat) noexcept = 0;

    virtual void RemoveBodyPart(std::uint8_t boneID, std::uint8_t direction) = 0;

    virtual void         SetFootBlood(std::uint32_t footBlood) = 0;
    virtual std::uint32_t GetFootBlood() const = 0;

    virtual bool IsOnFire() const = 0;
    virtual void SetOnFire(bool onFire) = 0;

    virtual bool GetStayInSamePlace() const = 0;
    virtual void SetStayInSamePlace(bool stay) = 0;

    virtual void GetVoice(std::int16_t* voiceType, std::int16_t* voiceID) const = 0;
    virtual void GetVoice(const char** voiceType, const char** voice) const = 0;
    virtual void SetVoice(std::int16_t voiceType, std::int16_t voiceID) = 0;
    virtual void SetVoice(const char* voiceType, const char* voice) = 0;
    virtual void ResetVoice() = 0;
    virtual void SetLanding(bool isLanding) = 0;
    virtual void SetUpdateMetricsRequired(bool required) = 0;

    virtual CWeaponStat* GetCurrentWeaponStat() const noexcept = 0;
    virtual float        GetCurrentWeaponRange() const noexcept = 0;
    virtual void         AddWeaponAudioEvent(EPedWeaponAudioEventType audioEventType) = 0;

    virtual int  GetCustomMoveAnim() const noexcept = 0;
    virtual bool IsDoingGangDriveby() const noexcept = 0;

    virtual CPedIKSAInterface*      GetPedIKInterface() = 0;
    virtual void*                   GetPedNodeInterface(std::int32_t nodeId) = 0;
    virtual std::unique_ptr<CPedIK> GetPedIK() = 0;

    virtual void GetAttachedSatchels(std::vector<SSatchelsData> &satchelsList) const = 0;
};
