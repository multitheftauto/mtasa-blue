#pragma once

#include "game/CCollision.h"

class CCollisionSA : CCollision {
public:
    bool TestLineSphere(const CColLineSA& line, const CColSphereSA& sphere) override;
};
