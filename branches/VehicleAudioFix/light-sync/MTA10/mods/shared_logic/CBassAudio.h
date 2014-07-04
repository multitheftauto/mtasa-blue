/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:
*
*****************************************************************************/

#define CUT_OFF 5.0f //Cut off point at which volume is regarded as 0 in the function e^-x

enum eSoundEventType
{
    SOUND_EVENT_NONE,
    SOUND_EVENT_FINISHED_DOWNLOAD,
    SOUND_EVENT_CHANGED_META,
    SOUND_EVENT_STREAM_RESULT,
};

struct SSoundEventInfo
{
    eSoundEventType type;
    SString         strString;
    double          dNumber;
    bool            bBool;
};

struct SSoundThreadVariables
{
    ZERO_ON_NEW
    int                     iRefCount;
    SString                 strURL;
    long                    lFlags;
    DWORD                   pSound;
    bool                    bStreamCreateResult;
    std::list < double >    onClientSoundFinishedDownloadQueue;
    std::list < SString >   onClientSoundChangedMetaQueue;
    CCriticalSection        criticalSection;

    void Release ( void );
};


class CBassAudio
{
public:
    ZERO_ON_NEW
                            CBassAudio              ( bool bStream, const SString& strPath, bool bLoop, bool b3D );
                            ~CBassAudio             ( void );

    bool                    BeginLoadingMedia       ( void );

    void                    SetPaused               ( bool bPaused );
    void                    SetPlayPosition         ( double dPosition );
    double                  GetPlayPosition         ( void );
    double                  GetLength               ( void );
    void                    SetVolume               ( float fVolume, bool bStore = true );
    void                    SetPlaybackSpeed        ( float fSpeed );
    void                    SetPosition             ( const CVector& vecPosition );
    void                    SetVelocity             ( const CVector& vecVelocity );
    void                    SetMinDistance          ( float fDistance );
    void                    SetMaxDistance          ( float fDistance );
    void                    SetFxEffects            ( int* pEnabledEffects, uint iNumElements );
    SString                 GetMetaTags             ( const SString& strFormat );
    bool                    IsFinished              ( void );

    void                    DoPulse                 ( const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt );
    void                    AddQueuedEvent          ( eSoundEventType type, const SString& strString, double dNumber = 0.0, bool bBool = false );
    bool                    GetQueuedEvent          ( SSoundEventInfo& info );
    void                    ParseShoutcastMeta      ( const SString& strMeta );

protected:
    HSTREAM                 ConvertFileToMono       ( const SString& strPath );
    static void             PlayStreamIntern        ( void* arguments );
    void                    CompleteStreamConnect   ( HSTREAM pSound );
    void                    ApplyFxEffects          ( void );
    void                    ServiceVars             ( void );
    void                    SetFinishedCallbacks    ( void );

    void                    Process3D               ( const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt );

public:
    SSoundThreadVariables*  m_pVars;
    bool                    bEndSync;
    bool                    bFreeSync;

private:
    const bool              m_bStream;
    const SString           m_strPath;
    const bool              m_b3D;
    const bool              m_bLoop;

    bool                    m_bPendingPlay;
    DWORD                   m_pSound;

    // Playback state
    bool                    m_bPaused;
    float                   m_fDefaultFrequency;
    float                   m_fVolume;
    float                   m_fMinDistance;
    float                   m_fMaxDistance;
    float                   m_fPlaybackSpeed;
    CVector                 m_vecPosition;
    CVector                 m_vecVelocity;

    int                     m_EnabledEffects[9];
    HFX                     m_FxEffects[9];

    // Info
    SString                 m_strStreamName;
    SString                 m_strStreamTitle;
    std::map < SString, SString > m_ConvertedTagMap;

    std::list < SSoundEventInfo > m_EventQueue;
};
