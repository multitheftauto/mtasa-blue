/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDoorSA.h
 *  PURPOSE:     Header file for vehicle door entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CDoor.h>

#define FUNC_GetAngleOpenRatio      0x6f47e0
#define FUNC_IsClosed               0x6f4800
#define FUNC_IsFullyOpen            0x6f4820
#define FUNC_Open                   0x6f4790

class CDoorSAInterface
{
public:
    float m_fOpenAngle;
    float m_fClosedAngle;
    // got 2 8bit vars next so might as well make this 16bit and get more flags
    WORD m_nDirn;
    BYTE m_nAxis;
    BYTE m_nDoorState;
    // simulation variables
    float m_fAngle;
    float m_fPrevAngle;
    float m_fAngVel;
};
static_assert(sizeof(CDoorSAInterface) == 0x18, "Invalid size for CDoorSAInterface");

class CDoorSA : public CDoor
{
private:
    CDoorSAInterface* internalInterface;

public:
    CDoorSA(CDoorSAInterface* doorInterface = 0) { internalInterface = doorInterface; };

    void              SetInterface(CDoorSAInterface* doorInterface) { internalInterface = doorInterface; }
    CDoorSAInterface* GetInterface() { return internalInterface; };

    float      GetAngleOpenRatio();
    bool       IsClosed();
    bool       IsFullyOpen();
    void       Open(float fOpenRatio);
    eDoorState GetDoorState() { return (eDoorState)GetInterface()->m_nDoorState; };
};
