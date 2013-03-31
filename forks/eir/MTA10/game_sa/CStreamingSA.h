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

#define DATA_TEXTURE_BLOCK      20000
#define DATA_ANIM_BLOCK         25575

#define FUNC_CStreaming__RequestModel                       0x4087E0
#define FUNC_LoadAllRequestedModels                         0x40EA10

#define FUNC_CStreaming__HasVehicleUpgradeLoaded            0x407820
#define FUNC_RequestVehicleUpgrade                          0x408C70

#define FUNC_CStreaming_RequestAnimations                   0x407120
#define FUNC_CStreaming_RequestSpecialModel                 0x409d10

#define MAX_MODELS                                          28835

class CStreamingSA : public CStreaming
{
public:
                    CStreamingSA                ( void );
                    ~CStreamingSA               ( void );

    void            RequestModel                ( unsigned short id, unsigned int flags );
    void            FreeModel                   ( unsigned short id );
    void            LoadAllRequestedModels      ( bool onlyPriority = false );
    bool            HasModelLoaded              ( unsigned int id );
    bool            IsModelLoading              ( unsigned int id );
    void            WaitForModel                ( unsigned int id );
    void            RequestAnimations           ( int idx, unsigned int flags );
    bool            HaveAnimationsLoaded        ( int idx );
    void            RequestVehicleUpgrade       ( unsigned short model, unsigned int flags );
    bool            HasVehicleUpgradeLoaded     ( int model );
    void            RequestSpecialModel         ( unsigned short model, const char *tex, unsigned int channel );

    void            SetRequestCallback          ( streamingRequestCallback_t callback );
    void            SetLoadCallback             ( streamingLoadCallback_t callback );
    void            SetFreeCallback             ( streamingFreeCallback_t callback );
};

#include "CStreamingSA.init.h"
#include "CStreamingSA.utils.h"

#ifdef RENDERWARE_VIRTUAL_INTERFACES
extern class CRwObjectSA *g_replObjectNative[DATA_TEXTURE_BLOCK];
#endif //RENDERWARE_VIRTUAL_INTERFACES
extern class CColModelSA *g_colReplacement[DATA_TEXTURE_BLOCK];
extern class CColModelSAInterface *g_originalCollision[DATA_TEXTURE_BLOCK];

#endif