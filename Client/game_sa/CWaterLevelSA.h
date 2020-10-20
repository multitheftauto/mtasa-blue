#pragma once

class CVector;
class CWaterLevelSA
{
public:
    static bool GetWaterLevel(float x, float y, float z, float* pOutWaterLevel, char bUnderWater, CVector* pVector);
};
