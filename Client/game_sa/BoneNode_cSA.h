#pragma once
#include "gamesa_renderware.h"

class BoneNode_cSAInterface
{
public:
    static void QuatToEuler(RtQuat* quat, RwV3d* angles);
    static void EulerToQuat(RwV3d* angles, RtQuat* quat);
};
