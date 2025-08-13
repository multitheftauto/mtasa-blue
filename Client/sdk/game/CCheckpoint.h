/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CCheckpoint.h
 *  PURPOSE:     Checkpoint entity interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CVector;

/**
 * \todo fix SetColor/GetColor, the format is actually BGRA (strange)
 */

enum
{
    CHECKPOINT_TUBE,                 // a long tube with arrow as direction
    CHECKPOINT_ENDTUBE,              // finish flag long tube checkpoint
    CHECKPOINT_EMPTYTUBE,            // just a long tube checkpoint
    CHECKPOINT_TORUS,
    CHECKPOINT_TORUS_NOFADE,
    CHECKPOINT_TORUSROT,
    CHECKPOINT_TORUSTHROUGH,
    CHECKPOINT_TORUS_UPDOWN,
    CHECKPOINT_TORUS_DOWN,
    CHECKPOINT_NUM
};

class CCheckpoint
{
public:
    virtual void               SetPosition(CVector* vecPosition) = 0;
    virtual CVector*           GetPosition() = 0;
    virtual void               SetPointDirection(CVector* vecPointDirection) = 0;
    virtual CVector*           GetPointDirection() = 0;
    virtual DWORD              GetType() = 0;            // need enum?
    virtual bool               IsActive() = 0;
    virtual DWORD              GetIdentifier() = 0;
    virtual SharedUtil::SColor GetColor() = 0;
    virtual void               SetColor(const SharedUtil::SColor color) = 0;
    virtual void               SetPulsePeriod(WORD wPulsePeriod) = 0;
    virtual void               SetRotateRate(short RotateRate) = 0;
    virtual float              GetSize() = 0;
    virtual void               SetSize(float fSize) = 0;
    virtual void               SetCameraRange(float fCameraRange) = 0;
    virtual float              GetPulseFraction() = 0;
    virtual void               Remove() = 0;
    virtual SColor             GetTargetArrowColor() const noexcept = 0;
    virtual float              GetTargetArrowSize() const noexcept = 0;
    virtual void               SetTargetArrowData(const SColor arrowColo, float size) noexcept = 0;
};
