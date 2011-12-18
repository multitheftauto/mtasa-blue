/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSound.h
*  PURPOSE:     Sound entity class header
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*               Florian Busse <flobu@gmx.net>
*
*****************************************************************************/

class CClientSound;
class CBassAudio;

#ifndef __CCLIENTSOUND_H
#define __CCLIENTSOUND_H

#include "CClientSoundManager.h"
#include "CClientEntity.h"
#include "CSimulatedPlayPosition.h"

class CClientSound : public CClientEntity
{
    DECLARE_CLASS( CClientSound, CClientEntity )
    friend class CClientSoundManager;

public:

                            CClientSound            ( CClientManager* pManager, ElementID ID );
                            ~CClientSound           ( void );

    virtual CSphere         GetWorldBoundingSphere  ( void );

    eClientEntityType       GetType                 ( void ) const                      { return CCLIENTSOUND; }

    bool                    Play                    ( const SString& strPath, bool bLoop );
    bool                    Play3D                  ( const SString& strPath, bool bLoop );
    void                    PlayStream              ( const SString& strURL, bool bLoop, bool b3D = false );

    void                    SetPaused               ( bool bPaused  );
    bool                    IsPaused                ( void );

    void                    SetPlayPosition         ( double dPosition );
    double                  GetPlayPosition         ( void );

    double                  GetLength               ( bool bAvoidLoad = false );

    void                    SetVolume               ( float fVolume, bool bStore = true );
    float                   GetVolume               ( void );

    void                    SetPlaybackSpeed        ( float fSpeed );
    float                   GetPlaybackSpeed        ( void );

    void                    GetPosition             ( CVector& vecPosition ) const;
    void                    SetPosition             ( const CVector& vecPosition );

    void                    GetVelocity             ( CVector& vecVelocity );
    void                    SetVelocity             ( const CVector& vecVelocity );

    void                    SetMinDistance          ( float fDistance );
    float                   GetMinDistance          ( void );

    void                    SetMaxDistance          ( float fDistance );
    float                   GetMaxDistance          ( void );

    SString                 GetMetaTags             ( const SString& strFormat );

    bool                    SetFxEffect             ( uint uiFxEffect, bool bEnable );
    bool                    IsFxEffectEnabled       ( uint uiFxEffect );

    void                    Unlink                  ( void ) {};

    bool                    IsSoundStopped          ( void )                            { return m_pAudio == NULL; }
    bool                    IsFinished              ( void );

    bool                    IsSound3D               ( void )                            { return m_b3D; }

protected:
    void                    Process3D               ( const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt );
    void                    BeginSimulationOfPlayPosition       ( void );
    void                    EndSimulationOfPlayPositionAndApply ( void );
    void                    DistanceStreamIn        ( void );
    void                    DistanceStreamOut       ( void );
    bool                    Create                  ( void );
    void                    Destroy                 ( void );

private:

    CClientSoundManager*    m_pSoundManager;
    CSimulatedPlayPosition  m_SimulatedPlayPosition;
    CBassAudio*             m_pAudio;

    // Initial state
    bool        m_bStream;
    bool        m_b3D;
    SString     m_strPath;
    bool        m_bLoop;

    // Info
    bool        m_bDoneCreate;
    double      m_dLength;
    std::map < SString, SString >  m_SavedTags;

    // Saved state
    bool        m_bPaused;
    float       m_fVolume;
    float       m_fPlaybackSpeed;
    CVector     m_vecPosition;
    CVector     m_vecVelocity;
    float       m_fMinDistance;
    float       m_fMaxDistance;
    int         m_EnabledEffects[9];
};

#endif
