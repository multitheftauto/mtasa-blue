/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCheckpointSA.h
 *  PURPOSE:     Header file for checkpoint entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CCheckpoint.h>
#include <CMatrix_Pad.h>
#include "Common.h"

class CCheckpointSAInterface
{
public:
    WORD    m_nType;
    bool    m_bIsUsed;
    bool    m_rotFlag;
    DWORD   m_nIdentifier;
    DWORD   rwColour;
    WORD    m_nPulsePeriod;
    short   m_nRotateRate;            // deg per frame (in either direction)
    CVector m_pos;
    CVector m_pointDir;
    FLOAT   m_fPulseFraction;
    FLOAT   m_fSize;
    FLOAT   m_fCameraRange;
    FLOAT   m_multiSize;
};

class CCheckpointSA : public CCheckpoint
{
private:
    CCheckpointSAInterface* internalInterface;

public:
    // constructor
    CCheckpointSA(CCheckpointSAInterface* checkpointInterface) { internalInterface = checkpointInterface; };

    CCheckpointSAInterface* GetInterface() { return internalInterface; }

    void     SetPosition(CVector* vecPosition);
    CVector* GetPosition();
    void     SetPointDirection(CVector* vecPointDirection);
    CVector* GetPointDirection();
    DWORD    GetType();                      // need enum?
    void     SetType(WORD wType);            // doesn't work propperly (not virtualed)
    BOOL     IsActive();
    void     Activate();
    DWORD    GetIdentifier();
    void     SetIdentifier(DWORD dwIdentifier);
    SColor   GetColor();
    void     SetColor(const SColor color);
    void     SetPulsePeriod(WORD wPulsePeriod);
    void     SetRotateRate(short RotateRate);
    FLOAT    GetSize();
    void     SetSize(FLOAT fSize);
    void     SetCameraRange(FLOAT fCameraRange);
    void     SetPulseFraction(FLOAT fPulseFraction);            // doesn't work propperly (not virtualed)
    FLOAT    GetPulseFraction();
    void     Remove();
};
