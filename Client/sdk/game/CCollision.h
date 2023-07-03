#pragma once

class CColLineSA;
class CColSphereSA;

class CCollision {
public:
    virtual bool TestLineSphere(const CColLineSA& line, const CColSphereSA& sphere) = 0;
};
