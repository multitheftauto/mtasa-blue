#include "StdInc.h"
#include "BoneNode_cSA.h"

void BoneNode_cSAInterface::QuatToEuler(RtQuat* quat, RwV3d* angles)
{
    auto BoneNode_c_QuatToEuler = (void(__cdecl*)(RtQuat*, RwV3d*))0x617080;
    BoneNode_c_QuatToEuler(quat, angles);
}

void BoneNode_cSAInterface::EulerToQuat(RwV3d* angles, RtQuat* quat)
{
    auto BoneNode_c_EulerToQuat = (void(__cdecl*)(RwV3d*, RtQuat*))0x6171F0;
    BoneNode_c_EulerToQuat(angles, quat);
}
