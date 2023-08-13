#include "StdInc.h"

#include "CCollisionSA.h"

bool CCollisionSA::TestLineSphere(const CColLineSA& line, const CColSphereSA& sphere) const
{
    return reinterpret_cast<bool(__cdecl*)(const CColLineSA&, const CColSphereSA&)>(0x417470)(line, sphere);
}
