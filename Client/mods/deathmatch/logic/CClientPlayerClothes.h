/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPlayerClothes.h
 *  PURPOSE:     Player entity clothes manager header
 *
 *****************************************************************************/

class CClientPlayerClothes;

#pragma once

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

class CClientPed;

struct SPlayerClothingType
{
    const char* szName;
};

struct SPlayerClothing
{
    std::string texture;
    std::string model;
};

class CClientPlayerClothes
{
public:
    CClientPlayerClothes(CClientPed* pPed);
    ~CClientPlayerClothes();

    const SPlayerClothing* GetClothing(unsigned char ucType);
    void                   AddClothes(const char* szTexture, const char* szModel, unsigned char ucType, bool bAddToModel = true);
    void                   InternalAddClothes(const SPlayerClothing* pClothing, unsigned char ucType);
    bool                   RemoveClothes(unsigned char ucType, bool bRemoveFromModel = true);

    void AddAllToModel();
    void RefreshClothes();

    void RemoveAll(bool bRemoveFromModel = true);

    void DefaultClothes(bool bAddToModel = true);

    static bool        HasEmptyClothing(unsigned char ucType);
    static bool        IsEmptyClothing(const SPlayerClothing* pClothing, unsigned char ucType);
    static const char* GetClothingName(unsigned char ucType);

    static std::vector<const SPlayerClothing*>  GetClothingGroup(unsigned char ucType);
    static bool                                 IsValidModel(unsigned short usModel);
    static bool                                 HasClothesChanged();
    static bool                                 AddClothingModel(const char* texture, const char* model, unsigned char clothingType);
    static bool                                 RemoveClothingModel(const char* texture, const char* model, unsigned char clothingType);

private:
    static const SPlayerClothing* GetClothing(const char* szTexture, const char* szModel, unsigned char ucType);

    CClientPed* m_pPlayerModel;

    SFixedArray<const SPlayerClothing*, PLAYER_CLOTHING_SLOTS>              m_Clothes;
    static bool                                                             m_bHasClothesChanged;
    static SFixedArray<std::vector<SPlayerClothing>, PLAYER_CLOTHING_SLOTS> m_DefaultClothes;
    static SFixedArray<std::list<SPlayerClothing>, PLAYER_CLOTHING_SLOTS>   m_NewClothes;
    static SFixedArray<const SPlayerClothing*, PLAYER_CLOTHING_SLOTS>       m_GlobalClothes;
    static bool                                                             m_bStaticInit;
};
