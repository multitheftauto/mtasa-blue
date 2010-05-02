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
    friend class CElement;
public:
                            CPlayerCamera           ( CPlayer * pPlayer );

    inline eCameraMode      GetMode                 ( void ) const                  { return m_Mode; }
    inline void             SetMode                 ( eCameraMode Mode )            { m_Mode = Mode; }
    static void             GetMode                 ( eCameraMode Mode, char* szBuffer, size_t sizeBuffer );
    static eCameraMode      GetMode                 ( const char * szMode );

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

    inline unsigned char    GetInterior             ( void ) const                  { return m_ucInterior; }
    inline void             SetInterior             ( unsigned char ucInterior )    { m_ucInterior = ucInterior; }

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
};

#endif