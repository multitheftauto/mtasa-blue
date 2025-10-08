/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CCollision.h
 *  PURPOSE:     Interface file for `CCollisionSA`
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

struct CColLineSA;
struct CColSphereSA;

class CCollision {
public:
    virtual bool TestLineSphere(const CColLineSA& line, const CColSphereSA& sphere) const = 0;
};
