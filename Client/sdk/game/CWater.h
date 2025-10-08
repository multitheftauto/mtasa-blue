/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWater.h
 *  PURPOSE:     Water interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CVector;

enum EWaterPolyType
{
    WATER_POLY_NONE = 0,
    WATER_POLY_QUAD = 1,
    WATER_POLY_TRIANGLE = 2,
    WATER_POLY_LIST = 3
};

class CWaterVertex
{
public:
    virtual WORD GetID() = 0;
    virtual void GetPosition(CVector& vec) = 0;
    virtual bool SetPosition(const CVector& vec, void* pChangeSource = NULL) = 0;
};

class CWaterPoly
{
public:
    virtual EWaterPolyType GetType() = 0;
    virtual WORD           GetID() = 0;
    virtual int            GetNumVertices() = 0;
    virtual CWaterVertex*  GetVertex(int index) = 0;
    virtual bool           ContainsPoint(float fX, float fY) = 0;
};
