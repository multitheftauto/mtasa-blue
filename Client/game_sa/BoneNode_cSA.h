/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/BoneNode_cSA.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "gamesa_renderware.h"

class BoneNode_cSAInterface
{
public:
    static void QuatToEuler(RtQuat* quat, RwV3d* angles);
    static void EulerToQuat(RwV3d* angles, RtQuat* quat);
};
