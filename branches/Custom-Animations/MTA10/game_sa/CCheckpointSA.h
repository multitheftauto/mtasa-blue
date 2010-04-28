/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCheckpointSA.h
*  PURPOSE:     Header file for checkpoint entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CHECKPOINT
#define __CGAMESA_CHECKPOINT

#include <game/CCheckpoint.h>
#include <CMatrix_Pad.h>
#include "Common.h"

class CCheckpointSAInterface
{
public:
    WORD        m_nType;
    bool        m_bIsUsed;
    bool        m_rotFlag;
    DWORD       m_nIdentifier;
    DWORD       rwColour;
    WORD        m_nPulsePeriod;
    short       m_nRotateRate;  // deg per frame (in either direction)
    CVector     m_pos;
    CVector     m_pointDir;
    FLOAT       m_fPulseFraction;
    FLOAT       m_fSize;
    FLOAT       m_fCameraRange;
    FLOAT       m_multiSize;    
};

class CCheckpointSA : public CCheckpoint
{
private:
    CCheckpointSAInterface      * internalInterface;
public:
    // constructor
    CCheckpointSA(CCheckpointSAInterface * checkpointInterface) { internalInterface = checkpointInterface; };

    CCheckpointSAInterface  * GetInterface() { return internalInterface; }

    VOID            SetPosition(CVector * vecPosition);
    CVector     * GetPosition();
    VOID            SetPointDirection(CVector * vecPointDirection);
    CVector     * GetPointDirection();
    DWORD           GetType(); // need enum?
    VOID            SetType(WORD wType); // doesn't work propperly (not virtualed)
    BOOL            IsActive();
    VOID            Activate();
    DWORD           GetIdentifier();
    VOID            SetIdentifier(DWORD dwIdentifier);
    SColor          GetColor();
    VOID            SetColor(const SColor color);
    VOID            SetPulsePeriod(WORD wPulsePeriod);
    VOID            SetRotateRate(short RotateRate);
    FLOAT           GetSize();
    VOID            SetSize(FLOAT fSize);
    VOID            SetCameraRange(FLOAT fCameraRange);
    VOID            SetPulseFraction(FLOAT fPulseFraction); // doesn't work propperly (not virtualed)
    FLOAT           GetPulseFraction();
    VOID            Remove();
};

#endif
