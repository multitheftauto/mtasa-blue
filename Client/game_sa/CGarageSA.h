/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CGarageSA.h
 *  PURPOSE:     Header file for garage handling class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/CGarage.h>

class CGarageSAInterface
{
public:
    CVector vecPosition;                           //  0
    float   DirectionA_X, DirectionA_Y;            // 12
    float   DirectionB_X, DirectionB_Y;            // 20
    float   Height, Width, Depth;                  // 28
    float   Left, Right, Front, Back;              // 40
    float   DoorPosition;                          // 56
    DWORD   pad;                                   // 60
    DWORD   pad2;                                  // 64
    char    Name[8];                               // 68
    BYTE    Type;                                  // 76
    BYTE    DoorState;                             // 77
    BYTE    DoorFlags;                             // 78
    BYTE    OriginalType;                          // 79
    BYTE    pad3[132];                             // 80
};

class CGarageSA : public CGarage
{
public:
    CGarageSA(CGarageSAInterface* pInterface);

    void SetType(BYTE type);
    BYTE GetType() const { return this->internalInterface->Type; }

    void GetPosition(CVector& vecPosition) const;
    void GetDirection(float& x1, float& y1, float& x2, float& y2) const;
    void GetSize(float& height, float& width, float& depth) const;
    void GetBoundingBox(float& left, float& right, float& front, float& back) const;

    void SetOpen(bool bOpen);
    bool IsOpen() const;

    CGarageSAInterface* GetInterface() { return this->internalInterface; }

private:
    CGarageSAInterface* internalInterface;
};
