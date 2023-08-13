#pragma once

struct CColLineSA;
struct CColSphereSA;

class CCollision {
public:
    virtual bool TestLineSphere(const CColLineSA& line, const CColSphereSA& sphere) const = 0;
};
