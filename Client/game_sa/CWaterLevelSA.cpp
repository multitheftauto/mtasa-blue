#include "StdInc.h"

bool CWaterLevelSA::GetWaterLevel(float x, float y, float z, float* pOutWaterLevel, char bUnderWater, CVector* pVector)
{
    auto CWaterLevel_GetWaterLevel = (bool(__cdecl*)(float, float, float, float*, char, CVector*))0x6EB690;
    return CWaterLevel_GetWaterLevel(x, y, z, pOutWaterLevel, bUnderWater, pVector);
}
