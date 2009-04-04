/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerClothes.h
*  PURPOSE:     Player clothes class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPlayerClothes;

#ifndef __PLAYERCLOTHES_H
#define __PLAYERCLOTHES_H

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
    char szName [ 32 ];
};

struct SPlayerClothing
{
    char* szTexture;
    char* szModel;
};

class CPlayerClothes
{
public:
                                CPlayerClothes              ( void );

    SPlayerClothing*            GetClothing                 ( unsigned char ucType );
    void                        AddClothes                  ( char* szTexture, char* szModel, unsigned char ucType );
    bool                        RemoveClothes               ( unsigned char ucType );
    
    void                        RemoveAll                   ( void );

    void                        DefaultClothes              ( void );

    static bool                 HasEmptyClothing            ( unsigned char ucType );
    static bool                 IsEmptyClothing             ( SPlayerClothing * pClothing, unsigned char ucType );
    static char*                GetClothingName             ( unsigned char ucType );

    static SPlayerClothing*     GetClothingGroup            ( unsigned char ucType );
    static SPlayerClothing*     GetClothing                 ( char * szTexture, char * szModel, unsigned char ucType );
    static const int            GetClothingGroupMax         ( unsigned char ucType );
    static bool                 IsValidClothing             ( char * szTexture, char * szModel, unsigned char ucType );

    SPlayerClothing *           m_Clothes [ PLAYER_CLOTHING_SLOTS ];
};

#endif
