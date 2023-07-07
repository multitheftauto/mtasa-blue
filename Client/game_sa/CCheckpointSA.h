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

#include <CVector.h>
#include <game/CCheckpoint.h>

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
    float   m_fPulseFraction;
    float   m_fSize;
    float   m_fCameraRange;
    float   m_multiSize;
};

class CCheckpointSA : public CCheckpoint
{
private:
    CCheckpointSAInterface* internalInterface;

public:
    CCheckpointSA(CCheckpointSAInterface* checkpointInterface) { internalInterface = checkpointInterface; };

    CCheckpointSAInterface* GetInterface() { return internalInterface; }

    void               SetPosition(CVector* vecPosition);
    CVector*           GetPosition();
    void               SetPointDirection(CVector* vecPointDirection);
    CVector*           GetPointDirection();
    DWORD              GetType();                      // need enum?
    void               SetType(WORD wType);            // doesn't work propperly (not virtualed)
    bool               IsActive();
    void               Activate();
    DWORD              GetIdentifier();
    void               SetIdentifier(DWORD dwIdentifier);
    SharedUtil::SColor GetColor();
    void               SetColor(const SharedUtil::SColor color);
    void               SetPulsePeriod(WORD wPulsePeriod);
    void               SetRotateRate(short RotateRate);
    float              GetSize();
    void               SetSize(float fSize);
    void               SetCameraRange(float fCameraRange);
    void               SetPulseFraction(float fPulseFraction);            // doesn't work propperly (not virtualed)
    float              GetPulseFraction();
    void               Remove();
};
