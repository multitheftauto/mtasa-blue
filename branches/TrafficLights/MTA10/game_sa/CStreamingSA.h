/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.h
*  PURPOSE:     Header file for data streamer class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CStreamingSA_H
#define __CStreamingSA_H

#include <game/CStreaming.h>
#include "Common.h"

#define FUNC_CStreaming__RequestModel                0x4087E0
#define FUNC_LoadAllRequestedModels                  0x40EA10
#define FUNC_CStreaming__HasVehicleUpgradeLoaded     0x407820

class CStreamingSA : public CStreaming
{
public:
    void            RequestModel                    ( DWORD dwModelID, DWORD dwFlags );
    void            LoadAllRequestedModels          ( BOOL bOnlyPriorityModels = 0 );
    BOOL            HasModelLoaded                  ( DWORD dwModelID );
    void            RequestAnimations               ( int iAnimationLibraryBlock, DWORD dwFlags );
    BOOL            HaveAnimationsLoaded            ( int iAnimationLibraryBlock );
    bool            HasVehicleUpgradeLoaded         ( int model );
};

#endif