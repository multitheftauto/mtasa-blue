/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerCamera.h
*  PURPOSE:     Player camera entity class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPlayerCamera_H
#define __CPlayerCamera_H

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
                            CPlayerCamera           ( CPlayer * pPlayer );
                            ~CPlayerCamera          ( void );

    eCameraMode             GetMode                 ( void ) const                  { return m_Mode; }
    void                    SetMode                 ( eCameraMode Mode );

    const CVector&          GetPosition             ( void ) const;
    void                    GetPosition             ( CVector& vecPosition ) const;
    void                    SetPosition             ( const CVector& vecPosition );
    
    void                    GetLookAt               ( CVector& vecLookAt ) const;
    void                    SetLookAt               ( const CVector& vecLookAt );

    void                    SetMatrix               ( const CVector& vecPosition, const CVector& vecLookAt );

    CElement *              GetTarget               ( void ) const                  { return m_pTarget; }
    void                    SetTarget               ( CElement* pElement );

    float                   GetRoll                 ( void ) const                  { return m_fRoll; }
    void                    SetRoll                 ( float fRoll )                 { m_fRoll = fRoll; }
    float                   GetFOV                  ( void ) const                  { return m_fFOV; }
    void                    SetFOV                  ( float fFOV )                  { m_fFOV = fFOV; }

    void                    SetRotation             ( CVector & vecRotation );

    unsigned char           GetInterior             ( void ) const                  { return m_ucInterior; }
    void                    SetInterior             ( unsigned char ucInterior )    { m_ucInterior = ucInterior; }

    CPlayer*                GetPlayer               ( void ) const                  { return m_pPlayer; }

    uchar                   GenerateSyncTimeContext ( void );
    bool                    CanUpdateSync           ( uchar ucRemote );

private:
    CPlayer *               m_pPlayer;
    eCameraMode             m_Mode;
    float                   m_fRotation;
    unsigned char           m_ucInterior;
    CVector                 m_vecPosition;
    CVector                 m_vecLookAt;
    CElement *              m_pTarget;
    float                   m_fRoll;
    float                   m_fFOV;
    uchar                   m_ucSyncTimeContext;
};

#endif
