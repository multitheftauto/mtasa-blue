#include "StdInc.h"

float CGeneralSA::LimitAngle(float angle)
{
    return ((float(__cdecl*)(float))0x53CB00)(angle);
}

float CGeneralSA::LimitRadianAngle(float angle)
{
    return ((float(__cdecl*)(float))0x53CB50)(angle);
}

float CGeneralSA::GetRadianAngleBetweenPoints(float x1, float y1, float x2, float y2)
{
    return ((float(__cdecl*)(float, float, float, float))0x53CBE0)(x1, y1, x2, y2);
}

float CGeneralSA::GetATanOfXY(float x, float y)
{
    return ((float(__cdecl*)(float, float))0x53CC70)(x, y);
}

unsigned char CGeneralSA::GetNodeHeadingFromVector(float x, float y)
{
    return ((unsigned char(__cdecl*)(float, float))0x53CDC0)(x, y);
}

bool CGeneralSA::SolveQuadratic(float a, float b, float c, float& x1, float& x2)
{
    return ((bool(__cdecl*)(float, float, float, float&, float&))0x53CE30)(a, b, c, x1, x2);
}

float CGeneralSA::GetAngleBetweenPoints(float x1, float y1, float x2, float y2)
{
    return ((float(__cdecl*)(float, float, float, float))0x53CEA0)(x1, y1, x2, y2);
}

unsigned int CGeneralSA::GetRandomNumberInRange(int min, int max)
{
    return ((unsigned int(__cdecl*)(int, int))0x407180)(min, max);
}

float CGeneralSA::GetRandomNumberInRange(float min, float max)
{
    return ((float(__cdecl*)(float, float))0x41BD90)(min, max);
}

void CGeneralSA::CamShakeNoPos(CCamera* camera, float strength)
{
    ((void(__cdecl*)(CCamera*, float))0x50A970)(camera, strength);
}
