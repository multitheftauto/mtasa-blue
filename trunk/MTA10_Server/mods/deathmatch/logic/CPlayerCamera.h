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

    inline eCameraMode      GetMode                 ( void )                        { return m_Mode; }
    inline void             SetMode                 ( eCameraMode Mode )            { m_Mode = Mode; }
    static void             GetMode                 ( eCameraMode Mode, char* szBuffer, size_t sizeBuffer );
    static eCameraMode      GetMode                 ( const char * szMode );

    void                    GetPosition             ( CVector & vecPosition );
    void                    SetPosition             ( CVector & vecPosition );
    
    void                    GetLookAt               ( CVector & vecLookAt );
    void                    SetLookAt               ( CVector & vecLookAt );

    void                    SetMatrix               ( CVector & vecPosition, CVector & vecLookAt );

    inline CElement *       GetTarget               ( void )                        { return m_pTarget; }
    void                    SetTarget               ( CElement * pElement );

    void                    SetRotation             ( CVector & vecRotation );

    inline unsigned char    GetInterior             ( void )                        { return m_ucInterior; }
    inline void             SetInterior             ( unsigned char ucInterior )    { m_ucInterior = ucInterior; }

private:
    CPlayer *               m_pPlayer;
    eCameraMode             m_Mode;
    float                   m_fRotation;
    unsigned char           m_ucInterior;
    CVector                 m_vecPosition;
    CVector                 m_vecLookAt;
    CElement *              m_pTarget;
};

#endif