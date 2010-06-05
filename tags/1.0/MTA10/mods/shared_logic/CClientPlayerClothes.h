/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPlayerClothes.h
*  PURPOSE:     Player entity clothes manager header
*  DEVELOPERS:  Jax <>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

class CClientPlayerClothes;

#ifndef __CLIENTPLAYERCLOTHES_H
#define __CLIENTPLAYERCLOTHES_H

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
    char szName [ 32 ];
};

struct SPlayerClothing
{
    char* szTexture;
    char* szModel;
};

class CClientPlayerClothes
{
public:
                                CClientPlayerClothes        ( CClientPed* pPed );
                                ~CClientPlayerClothes       ( void );

    SPlayerClothing*            GetClothing                 ( unsigned char ucType );
    void                        AddClothes                  ( char* szTexture, char* szModel, unsigned char ucType, bool bAddToModel = true );
    void                        InternalAddClothes          ( SPlayerClothing * pClothing, unsigned char ucType );
    bool                        RemoveClothes               ( unsigned char ucType, bool bRemoveFromModel = true );
    
    void                        AddAllToModel               ( void );
    void                        RemoveAll                   ( bool bRemoveFromModel = true );

    void                        DefaultClothes              ( bool bAddToModel = true );

    static bool                 HasEmptyClothing            ( unsigned char ucType );
    static bool                 IsEmptyClothing             ( SPlayerClothing * pClothing, unsigned char ucType );
    static char*                GetClothingName             ( unsigned char ucType );

	static SPlayerClothing*     GetClothingGroup            ( unsigned char ucType );
	static const int            GetClothingGroupMax         ( unsigned char ucType );

private:
    static SPlayerClothing*     GetClothing                 ( char * szTexture, char * szModel, unsigned char ucType );

    CClientPed*                 m_pPlayerModel;

    SPlayerClothing *           m_Clothes [ PLAYER_CLOTHING_SLOTS ];
    static SPlayerClothing *    m_GlobalClothes [ PLAYER_CLOTHING_SLOTS ];
    static bool                 m_bStaticInit;
};

#endif

        