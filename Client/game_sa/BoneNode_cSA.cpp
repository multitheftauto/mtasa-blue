/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/BoneNode_cSA.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "BoneNode_cSA.h"

void BoneNode_cSAInterface::QuatToEuler(RtQuat* quat, RwV3d* angles)
{
    (reinterpret_cast<void(__cdecl*)(RtQuat*, RwV3d*)>(0x617080))(quat, angles);
}

void BoneNode_cSAInterface::EulerToQuat(RwV3d* angles, RtQuat* quat)
{
    (reinterpret_cast<void(__cdecl*)(RwV3d*, RtQuat*)>(0x6171F0))(angles, quat);
}
