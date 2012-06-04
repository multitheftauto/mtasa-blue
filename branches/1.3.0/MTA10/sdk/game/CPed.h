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

#ifndef __CGAME_PED
#define __CGAME_PED

class CTaskManager;

#include "Common.h"
#include "CPhysical.h"
#include "CWeapon.h"
#include "CPedIntelligence.h"
#include "CPedSound.h"

#include <CVector2D.h>

#include <windows.h>

// forward declaration, avoid compile error
class CVehicle;
class CObject;

enum ePedPieceTypes {
    PED_PIECE_UNKNOWN = 0,

    PED_PIECE_TORSO = 3,
    PED_PIECE_ASS,
    PED_PIECE_LEFT_ARM,
    PED_PIECE_RIGHT_ARM,
    PED_PIECE_LEFT_LEG,
    PED_PIECE_RIGHT_LEG,
    PED_PIECE_HEAD
};

enum eBone { 
    BONE_PELVIS1 = 1,
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
    BONE_RIGHTFOOT
};

enum {
    BIKE_KNOCK_OFF_DEFAULT = 0,
    BIKE_KNOCK_OFF_NEVER,
    BIKE_KNOCK_OFF_ALWAYS_NORMAL,
};

enum {
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

enum { PLAYER_PED, CIVILIAN_PED };

class CPed : public virtual CPhysical
{
public:
    virtual                 ~CPed ( void ) {};

    virtual class CPedSAInterface * GetPedInterface ( void ) = 0;

    virtual void            AttachPedToBike(CEntity * entity, CVector * vector, unsigned short sUnk, FLOAT fUnk, FLOAT fUnk2, eWeaponType weaponType)=0;
    virtual void            DetachPedFromEntity ( void )=0;

    virtual bool            CanSeeEntity(CEntity * entity, FLOAT fDistance)=0;
    virtual CVehicle        * GetVehicle()=0;
    virtual void            Respawn (CVector * position,bool bCameraCut)=0;

    virtual void            SetModelIndex       ( unsigned long ulModel ) = 0;
    virtual void            RemoveGeometryRef   ( void ) = 0;

    virtual FLOAT           GetHealth ()=0;
    virtual void            SetHealth ( float fHealth )=0;
    virtual float           GetArmor () = 0;
    virtual void            SetArmor ( float fArmor ) = 0;
    virtual bool            AddProjectile ( eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity )=0;
    virtual CWeapon *       GiveWeapon ( eWeaponType weaponType, unsigned int uiAmmo, eWeaponSkill weaponSkill )=0;
    virtual CWeapon *       GetWeapon ( eWeaponSlot weaponSlot )=0;
    virtual CWeapon *       GetWeapon ( eWeaponType weaponType )=0;
    virtual void            ClearWeapons        ( void ) = 0;
    virtual void            RemoveWeaponModel ( int iModel ) = 0;
    virtual void            ClearWeapon     ( eWeaponType weaponType )=0;

    virtual void            SetIsStanding( bool bStanding )=0;
    virtual DWORD           GetType ()=0;
    virtual CPedIntelligence *  GetPedIntelligence()=0;
    virtual CPedSound *         GetPedSound () = 0;
    virtual DWORD *         GetMemoryValue ( DWORD dwOffset )=0;


    virtual FLOAT           GetCurrentRotation()=0;
    virtual FLOAT           GetTargetRotation()=0;
    virtual void            SetCurrentRotation(FLOAT fRotation)=0;
    virtual void            SetTargetRotation(FLOAT fRotation)=0;
    virtual eWeaponSlot     GetCurrentWeaponSlot()=0;
    virtual void            SetCurrentWeaponSlot ( eWeaponSlot weaponSlot )=0;

    virtual CVector *       GetBonePosition ( eBone bone, CVector * vecPosition )=0;
    virtual CVector *       GetTransformedBonePosition ( eBone bone, CVector * vecPosition )=0;

    virtual bool            IsDucking ( void )=0;
    virtual void            SetDucking ( bool bDuck )=0;
    virtual bool            IsInWater ( void )=0;
    virtual int             GetCantBeKnockedOffBike ( void )=0;
    virtual void            SetCantBeKnockedOffBike ( int iCantBeKnockedOffBike )=0;
    virtual void            QuitEnteringCar ( CVehicle * vehicle, int iSeat, bool bUnknown )=0;

    virtual bool            IsWearingGoggles ( void )=0;
    virtual void            SetGogglesState ( bool bIsWearingThem )=0;

    virtual void            SetClothesTextureAndModel ( const char* szModel, const char* szModelLocationName, int iTexture ) = 0;
    virtual void            RebuildPlayer ( void ) = 0;

    virtual eFightingStyle  GetFightingStyle ( void ) = 0;
    virtual void            SetFightingStyle ( eFightingStyle style, BYTE bStyleExtra ) = 0;

    virtual CEntity*        GetContactEntity ( void ) = 0;

    virtual unsigned char   GetRunState ( void ) = 0;

    virtual CEntity*        GetTargetedEntity ( void ) = 0;
    virtual void            SetTargetedEntity ( CEntity* pEntity ) = 0;

    virtual bool            GetCanBeShotInVehicle       ( void ) = 0;
    virtual bool            GetTestForShotInVehicle     ( void ) = 0;

    virtual void            SetCanBeShotInVehicle       ( bool bShot ) = 0;
    virtual void            SetTestForShotInVehicle     ( bool bTest ) = 0;

    virtual BYTE            GetOccupiedSeat ( void ) = 0;
    virtual void            SetOccupiedSeat ( BYTE seat ) = 0;

    virtual void            RemoveBodyPart ( int i, char c ) = 0;

    virtual void            SetFootBlood ( unsigned int uiFootBlood ) = 0;
    virtual unsigned int    GetFootBlood ( void ) = 0;

    virtual bool            IsOnFire ( void ) = 0;
    virtual void            SetOnFire ( bool bOnFire ) = 0;

    virtual bool            GetStayInSamePlace ( void ) = 0;
    virtual void            SetStayInSamePlace ( bool bStay ) = 0;

    virtual void            GetVoice           ( short* psVoiceType, short* psVoiceID ) = 0;
    virtual void            GetVoice           ( const char** pszVoiceType, const char** pszVoice ) = 0;
    virtual void            SetVoice           ( short sVoiceType, short sVoiceID ) = 0;
    virtual void            SetVoice           ( const char* szVoiceType, const char* szVoice ) = 0;
};

#endif
