/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CGarage.h
 *  PURPOSE:     Garage interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CGarageSAInterface;
class CVector;

class CGarage
{
public:
    virtual void SetType(BYTE type) = 0;
    virtual BYTE GetType() const = 0;

    virtual void GetPosition(CVector& vecPosition) const = 0;
    virtual void GetDirection(float& x1, float& y1, float& x2, float& y2) const = 0;
    virtual void GetSize(float& height, float& width, float& depth) const = 0;
    virtual void GetBoundingBox(float& left, float& right, float& front, float& back) const = 0;

    virtual void SetOpen(bool bOpen) = 0;
    virtual bool IsOpen() const = 0;

    virtual CGarageSAInterface* GetInterface() = 0;
};
