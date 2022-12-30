/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAnimBlendAssocGroup.h
 *  PURPOSE:     Animation blend association group interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CAnimBlendAssocGroupSAInterface;
class CAnimBlendAssociation;
class CAnimBlendAssociationSAInterface;
class CAnimBlendStaticAssociation;
class CAnimBlock;
struct RpClump;
typedef unsigned char BYTE;
typedef unsigned long AssocGroupId;

enum class eAnimGroup : int
{
    ANIM_GROUP_NONE = -1,
    ANIM_GROUP_DEFAULT = 0,
    ANIM_GROUP_DOOR = 1,
    ANIM_GROUP_BIKES = 2,
    ANIM_GROUP_BIKEV = 3,
    ANIM_GROUP_BIKEH = 4,
    ANIM_GROUP_BIKED = 5,
    ANIM_GROUP_WAYFARER = 6,
    ANIM_GROUP_BMX = 7,
    ANIM_GROUP_MTB = 8,
    ANIM_GROUP_CHOPPA = 9,
    ANIM_GROUP_QUAD = 10,
    ANIM_GROUP_PYTHON = 11,
    ANIM_GROUP_PYTHONBAD = 12,
    ANIM_GROUP_COLT45 = 13,
    ANIM_GROUP_COLT_COP = 14,
    ANIM_GROUP_COLT45PRO = 15,
    ANIM_GROUP_SAWNOFF = 16,
    ANIM_GROUP_SAWNOFFPRO = 17,
    ANIM_GROUP_SILENCED = 18,
    ANIM_GROUP_SHOTGUN = 19,
    ANIM_GROUP_SHOTGUNBAD = 20,
    ANIM_GROUP_BUDDY = 21,
    ANIM_GROUP_BUDDYBAD = 22,
    ANIM_GROUP_UZI = 23,
    ANIM_GROUP_UZIBAD = 24,
    ANIM_GROUP_RIFLE = 25,
    ANIM_GROUP_RIFLEBAD = 26,
    ANIM_GROUP_SNIPER = 27,
    ANIM_GROUP_GRENADE = 28,
    ANIM_GROUP_FLAME = 29,
    ANIM_GROUP_ROCKET = 30,
    ANIM_GROUP_SPRAYCAN = 31,
    ANIM_GROUP_GOGGLES = 32,
    ANIM_GROUP_MELEE_1 = 33,
    ANIM_GROUP_MELEE_2 = 34,
    ANIM_GROUP_MELEE_3 = 35,
    ANIM_GROUP_MELEE_4 = 36,
    ANIM_GROUP_BBBAT_1 = 37,
    ANIM_GROUP_GCLUB_1 = 38,
    ANIM_GROUP_KNIFE_1 = 39,
    ANIM_GROUP_SWORD_1 = 40,
    ANIM_GROUP_DILDO_1 = 41,
    ANIM_GROUP_FLOWERS_1 = 42,
    ANIM_GROUP_CSAW_1 = 43,
    ANIM_GROUP_KICK_STD = 44,
    ANIM_GROUP_PISTLWHP = 45,
    ANIM_GROUP_MEDIC = 46,
    ANIM_GROUP_BEACH = 47,
    ANIM_GROUP_SUNBATHE = 48,
    ANIM_GROUP_PLAYIDLES = 49,
    ANIM_GROUP_RIOT = 50,
    ANIM_GROUP_STRIP = 51,
    ANIM_GROUP_GANGS = 52,
    ANIM_GROUP_ATTRACTORS = 53,
    ANIM_GROUP_PLAYER = 54,
    ANIM_GROUP_FAT = 55,
    ANIM_GROUP_MUSCULAR = 56,
    ANIM_GROUP_PLAYERROCKET = 57,
    ANIM_GROUP_PLAYERROCKETF = 58,
    ANIM_GROUP_PLAYERROCKETM = 59,
    ANIM_GROUP_PLAYER2ARMED = 60,
    ANIM_GROUP_PLAYER2ARMEDF = 61,
    ANIM_GROUP_PLAYER2ARMEDM = 62,
    ANIM_GROUP_PLAYERBBBAT = 63,
    ANIM_GROUP_PLAYERBBBATF = 64,
    ANIM_GROUP_PLAYERBBBATM = 65,
    ANIM_GROUP_PLAYERCSAW = 66,
    ANIM_GROUP_PLAYERCSAWF = 67,
    ANIM_GROUP_PLAYERCSAWM = 68,
    ANIM_GROUP_PLAYERSNEAK = 69,
    ANIM_GROUP_PLAYERJETPACK = 70,
    ANIM_GROUP_SWIM = 71,
    ANIM_GROUP_DRIVEBYS = 72,
    ANIM_GROUP_BIKE_DBZ = 73,
    ANIM_GROUP_COP_DBZ = 74,
    ANIM_GROUP_QUAD_DBZ = 75,
    ANIM_GROUP_FAT_TIRED = 76,
    ANIM_GROUP_HANDSIGNAL = 77,
    ANIM_GROUP_HANDSIGNALL = 78,
    ANIM_GROUP_LHAND = 79,
    ANIM_GROUP_RHAND = 80,
    ANIM_GROUP_CARRY = 81,
    ANIM_GROUP_CARRY05 = 82,
    ANIM_GROUP_CARRY105 = 83,
    ANIM_GROUP_INT_HOUSE = 84,
    ANIM_GROUP_INT_OFFICE = 85,
    ANIM_GROUP_INT_SHOP = 86,
    ANIM_GROUP_STEALTH_KN = 87,
    ANIM_GROUP_STDCARAMIMS = 88,
    ANIM_GROUP_LOWCARAMIMS = 89,
    ANIM_GROUP_TRKCARANIMS = 90,
    ANIM_GROUP_STDBIKEANIMS = 91,
    ANIM_GROUP_SPORTBIKEANIMS = 92,
    ANIM_GROUP_VESPABIKEANIMS = 93,
    ANIM_GROUP_HARLEYBIKEANIMS = 94,
    ANIM_GROUP_DIRTBIKEANIMS = 95,
    ANIM_GROUP_WAYFBIKEANIMS = 96,
    ANIM_GROUP_BMXBIKEANIMS = 97,
    ANIM_GROUP_MTBBIKEANIMS = 98,
    ANIM_GROUP_CHOPPABIKEANIMS = 99,
    ANIM_GROUP_QUADBIKEANIMS = 100,
    ANIM_GROUP_VANCARANIMS = 101,
    ANIM_GROUP_RUSTPLANEANIMS = 102,
    ANIM_GROUP_COACHCARANIMS = 103,
    ANIM_GROUP_BUSCARANIMS = 104,
    ANIM_GROUP_DOZERCARANIMS = 105,
    ANIM_GROUP_KARTCARANIMS = 106,
    ANIM_GROUP_CONVCARANIMS = 107,
    ANIM_GROUP_MTRKCARANIMS = 108,
    ANIM_GROUP_TRAINCARRANIMS = 109,
    ANIM_GROUP_STDTALLCARAMIMS = 110,
    ANIM_GROUP_HOVERCARANIMS = 111,
    ANIM_GROUP_TANKCARANIMS = 112,
    ANIM_GROUP_BFINJCARAMIMS = 113,
    ANIM_GROUP_LEARPLANEANIMS = 114,
    ANIM_GROUP_HARRPLANEANIMS = 115,
    ANIM_GROUP_STDCARUPRIGHT = 116,
    ANIM_GROUP_NVADAPLANEANIMS = 117,
    ANIM_GROUP_MAN = 118,
    ANIM_GROUP_SHUFFLE = 119,
    ANIM_GROUP_OLDMAN = 120,
    ANIM_GROUP_GANG1 = 121,
    ANIM_GROUP_GANG2 = 122,
    ANIM_GROUP_OLDFATMAN = 123,
    ANIM_GROUP_FATMAN = 124,
    ANIM_GROUP_JOGGER = 125,
    ANIM_GROUP_DRUNKMAN = 126,
    ANIM_GROUP_BLINDMAN = 127,
    ANIM_GROUP_SWAT = 128,
    ANIM_GROUP_WOMAN = 129,
    ANIM_GROUP_SHOPPING = 130,
    ANIM_GROUP_BUSYWOMAN = 131,
    ANIM_GROUP_SEXYWOMAN = 132,
    ANIM_GROUP_PRO = 133,
    ANIM_GROUP_OLDWOMAN = 134,
    ANIM_GROUP_FATWOMAN = 135,
    ANIM_GROUP_JOGWOMAN = 136,
    ANIM_GROUP_OLDFATWOMAN = 137,
    ANIM_GROUP_SKATE = 138,
    TOTAL_ANIM_GROUPS = 139
};

enum class eAnimID : int
{
    ANIM_ID_WALK = 0,
    ANIM_ID_RUN = 1,
    ANIM_ID_SPRINT = 2,
    ANIM_ID_IDLE = 3,
    ANIM_ID_IDLE_TIRED = 10,
    ANIM_ID_WEAPON_CROUCH = 55,
    ANIM_ID_WEAPON_FIRE = 224,
    ANIM_ID_STEALTH_AIM = 347,
};

class CAnimBlendAssocGroup
{
public:
    virtual CAnimBlendAssociationSAInterface* CopyAnimation(unsigned int AnimID) = 0;
    virtual void                              InitEmptyAssociations(RpClump* pClump) = 0;
    virtual bool                              IsCreated() = 0;
    virtual int                               GetNumAnimations() = 0;
    virtual CAnimBlock*                       GetAnimBlock() = 0;
    virtual CAnimBlendStaticAssociation*      GetAnimation(unsigned int ID) = 0;
    virtual eAnimGroup                        GetGroupID() = 0;
    virtual void                              CreateAssociations(const char* szBlockName) = 0;
    virtual bool                              IsLoaded() = 0;
    virtual void                              SetIDOffset(int iOffset) = 0;
    virtual CAnimBlendAssocGroupSAInterface*  GetInterface() = 0;
};
