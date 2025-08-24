/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayerCamera.h
 *  PURPOSE:     Player camera entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>

class CPlayer;
class CElement;

enum eCameraMode
{
    CAMERAMODE_PLAYER = 0,
    CAMERAMODE_FIXED,
    CAMERAMODE_INVALID,
};

class CPlayerCamera
{
public:
    CPlayerCamera(CPlayer* pPlayer);
    ~CPlayerCamera();

    eCameraMode GetMode() const { return m_Mode; }
    void        SetMode(eCameraMode Mode);

    const CVector& GetPosition() const;
    void           GetPosition(CVector& vecPosition) const;
    void           SetPosition(const CVector& vecPosition);

    void GetLookAt(CVector& vecLookAt) const;
    void SetLookAt(const CVector& vecLookAt);

    void SetMatrix(const CVector& vecPosition, const CVector& vecLookAt);

    CElement* GetTarget() const { return m_pTarget; }
    void      SetTarget(CElement* pElement);

    float GetRoll() const { return m_fRoll; }
    void  SetRoll(float fRoll) { m_fRoll = fRoll; }
    float GetFOV() const { return m_fFOV; }
    void  SetFOV(float fFOV) { m_fFOV = fFOV; }

    void SetRotation(CVector& vecRotation);

    unsigned char GetInterior() const { return m_ucInterior; }
    void          SetInterior(unsigned char ucInterior) { m_ucInterior = ucInterior; }

    CPlayer* GetPlayer() const { return m_pPlayer; }

    uchar GenerateSyncTimeContext();
    bool  CanUpdateSync(uchar ucRemote);

private:
    CPlayer*      m_pPlayer;
    eCameraMode   m_Mode;
    float         m_fRotation;
    unsigned char m_ucInterior;
    CVector       m_vecPosition;
    CVector       m_vecLookAt;
    CElement*     m_pTarget;
    float         m_fRoll;
    float         m_fFOV;
    uchar         m_ucSyncTimeContext;
};
