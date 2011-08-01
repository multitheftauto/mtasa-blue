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

#ifndef __CCLIENTSOUND_H
#define __CCLIENTSOUND_H

#include "CClientSoundManager.h"
#include "CClientEntity.h"

//#define MAX_SOUND_DISTANCE 100
#define CUT_OFF 5.0f //Cut off point at which volume is regarded as 0 in the function e^-x

typedef struct
{
    CClientSound* pClientSound;
    SString strURL;
    long lFlags;
} thestruct;

class CClientSound : public CClientEntity
{
    DECLARE_CLASS( CClientSound, CClientEntity )
    friend class CClientSoundManager;

public:

                            CClientSound            ( CClientManager* pManager, ElementID ID );
                            ~CClientSound           ( void );

    eClientEntityType       GetType                 ( void ) const                      { return CCLIENTSOUND; }

    bool                    Play                    ( const SString& strPath, bool bLoop );
    bool                    Play3D                  ( const SString& strPath, const CVector& vecPosition, bool bLoop );

    HSTREAM                 ConvertFileToMono       ( const SString& strPath );

    void                    PlayStream              ( const SString& strURL, bool bLoop, bool b3D = false, const CVector& vecPosition = CVector () );

    static void             PlayStreamIntern        ( void* arguments );

    void                    GetMeta                  ( void );

    void                    ThreadCallback          ( HSTREAM pSound );

    void                    Stop                    ( void );

    void                    SetPaused               ( bool bPaused );
    bool                    IsPaused                ( void );

    bool                    IsFinished              ( void );

    void                    SetPlayPosition         ( unsigned int uiPosition );
    unsigned int            GetPlayPosition         ( void );

    unsigned int            GetLength               ( void );

    void                    SetVolume               ( float fVolume, bool bStore = true );
    float                   GetVolume               ( void );

    void                    SetPlaybackSpeed        ( float fSpeed );
    float                   GetPlaybackSpeed        ( void );

    void                    GetPosition             ( CVector& vecPosition ) const;
    void                    SetPosition             ( const CVector& vecPosition );

    void                    GetVelocity             ( CVector& vecVelocity );
    void                    SetVelocity             ( const CVector& vecVelocity );

    void                    SetDimension            ( unsigned short usDimension );
    void                    RelateDimension         ( unsigned short usDimension );

    void                    SetMinDistance          ( float fDistance );
    float                   GetMinDistance          ( void );

    void                    SetMaxDistance          ( float fDistance );
    float                   GetMaxDistance          ( void );

    void                    ShowShoutcastMetaTags   ( void );
    SString                 GetMetaTags             ( const SString& strFormat );

    bool                    SetFxEffect             ( int iFxEffect, bool bEnable );
    bool                    IsFxEffectEnabled       ( int iFxEffect );

    void                    Unlink                  ( void ) {};

protected:

    DWORD                   GetSound                ( void )                            { return m_pSound; };
    void                    Process3D               ( CVector vecPosition, CVector vecLookAt );

private:

    CClientSoundManager*    m_pSoundManager;

    DWORD                   m_pSound;

    bool                    m_b3D;
    bool                    m_bInSameDimension;
    bool                    m_bPaused;
    float                   m_fDefaultFrequency;
    float                   m_fVolume;
    float                   m_fMinDistance;
    float                   m_fMaxDistance;
    float                   m_fPlaybackSpeed;
    CVector                 m_vecPosition;
    CVector                 m_vecVelocity;

    HFX                     m_FxEffects[9];

    HANDLE                  m_pThread;

    SString                 m_strPath;

    SString                 m_strStreamName;
    SString                 m_strStreamTitle;
};

#endif
