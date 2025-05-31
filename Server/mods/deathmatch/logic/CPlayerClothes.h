/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayerClothes.h
 *  PURPOSE:     Player clothes class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPlayerClothes;

#pragma once

#include "CCommon.h"

#define PLAYER_CLOTHING_SLOTS 18
#define TORSO_CLOTHING_MAX 68
#define HAIR_CLOTHING_MAX 33
#define LEGS_CLOTHING_MAX 45
#define SHOES_CLOTHING_MAX 38
#define LEFT_UPPER_ARM_CLOTHING_MAX 3
#define LEFT_LOWER_ARM_CLOTHING_MAX 4
#define RIGHT_UPPER_ARM_CLOTHING_MAX 4
#define RIGHT_LOWER_ARM_CLOTHING_MAX 4
#define BACK_TOP_CLOTHING_MAX 7
#define LEFT_CHEST_CLOTHING_MAX 6
#define RIGHT_CHEST_CLOTHING_MAX 7
#define STOMACH_CLOTHING_MAX 7
#define LOWER_BACK_CLOTHING_MAX 6
#define EXTRA1_CLOTHING_MAX 12
#define EXTRA2_CLOTHING_MAX 12
#define EXTRA3_CLOTHING_MAX 17
#define EXTRA4_CLOTHING_MAX 57
#define SUIT_CLOTHING_MAX 9

class CPed;

struct SPlayerClothingType
{
    const char* szName;
};

struct SPlayerClothing
{
    const char* szTexture;
    const char* szModel;
};

class CPlayerClothes
{
public:
    CPlayerClothes();

    const SPlayerClothing* GetClothing(unsigned char ucType);
    void                   AddClothes(const char* szTexture, const char* szModel, unsigned char ucType);
    bool                   RemoveClothes(unsigned char ucType);

    void RemoveAll();

    void DefaultClothes();

    static bool        HasEmptyClothing(unsigned char ucType);
    static bool        IsEmptyClothing(const SPlayerClothing* pClothing, unsigned char ucType);
    static const char* GetClothingName(unsigned char ucType);

    static const SPlayerClothing* GetClothingGroup(unsigned char ucType);
    static const SPlayerClothing* GetClothing(const char* szTexture, const char* szModel, unsigned char ucType);
    static const int              GetClothingGroupMax(unsigned char ucType);
    static bool                   IsValidClothing(const char* szTexture, const char* szModel, unsigned char ucType);

    SFixedArray<const SPlayerClothing*, PLAYER_CLOTHING_SLOTS> m_Clothes;
};
