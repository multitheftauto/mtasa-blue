/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#define CUT_OFF 5.0f            // Cut off point at which volume is regarded as 0 in the function e^-x

enum eSoundEventType
{
    SOUND_EVENT_NONE,
    SOUND_EVENT_FINISHED_DOWNLOAD,
    SOUND_EVENT_CHANGED_META,
    SOUND_EVENT_STREAM_RESULT,
    SOUND_EVENT_BEAT,
};

struct SSoundEventInfo
{
    eSoundEventType type;
    SString         strString;
    double          dNumber;
    bool            bBool;
    SString         strError;
};

struct SSoundThreadVariables
{
    ZERO_ON_NEW
    SString            strURL;
    long               lFlags;
    DWORD              pSound;
    bool               bStreamCreateResult;
    std::list<double>  onClientSoundFinishedDownloadQueue;
    std::list<float>   onClientBPMQueue;
    std::list<double>  onClientBeatQueue;
    std::list<SString> onClientSoundChangedMetaQueue;
    CCriticalSection   criticalSection;
};

class CBassAudio
{
public:
    ZERO_ON_NEW
    CBassAudio(bool bStream, const SString& strPath, bool bLoop, bool bThrottle, bool b3D);
    CBassAudio(void* pBuffer, unsigned int uiBufferLength, bool bLoop, bool b3D);
    ~CBassAudio();
    void Destroy();

    bool BeginLoadingMedia();

    void   SetPaused(bool bPaused);
    bool   SetPlayPosition(double dPosition);
    double GetPlayPosition();
    double GetLength();
    double GetBufferLength();
    void   SetVolume(float fVolume);
    void   SetPlaybackSpeed(float fSpeed);
    void   SetPosition(const CVector& vecPosition);
    void   SetVelocity(const CVector& vecVelocity);
    void   SetMinDistance(float fDistance);
    void   SetMaxDistance(float fDistance);
    void   SetTempoValues(float fSampleRate, float fTempo, float fPitch, bool bReverse);
    void   GetTempoValues(float& fSampleRate, float& fTempo, float& fPitch, bool& bReverse)
    {
        fSampleRate = m_fSampleRate;
        fTempo = m_fTempo;
        fPitch = m_fPitch;
        bReverse = m_bReversed;
    };
    void    SetReversed(bool bReversed) { m_bReversed = bReversed; };
    bool    GetPanEnabled() { return m_bPan; };
    void    SetPanEnabled(bool bPan) { m_bPan = bPan; };
    void    SetFxEffects(int* pEnabledEffects, uint iNumElements);
    BOOL    SetFxParameters(uint iFxEffect, void* params);
    BOOL    GetFxParameters(uint iFxEffect, void* params);
    SString GetMetaTags(const SString& strFormat);
    uint    GetReachedEndCount();
    bool    IsFreed();
    float   GetPan();
    void    SetPan(float fPan);
    bool    SetLooped(bool bLoop);

    void   DoPulse(const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt);
    void   AddQueuedEvent(eSoundEventType type, const SString& strString, double dNumber = 0.0, bool bBool = false, const SString& strError = "");
    bool   GetQueuedEvent(SSoundEventInfo& info);
    void   ParseShoutcastMeta(const SString& strMeta);
    float* GetFFTData(int iLength);
    float* GetWaveData(int iLength);
    DWORD  GetLevelData();
    float  GetSoundBPM();
    void   SetSoundBPM(float fBPM) { m_fBPM = fBPM; }

    // Retrieves the error message for the most recent BASS function call in the current thread.
    static const char* ErrorGetMessage();

    // Retrieves the error code for the most recent BASS function call in the current thread.
    static int ErrorGetCode();

protected:
    HSTREAM             ConvertFileToMono(const SString& strPath);
    static DWORD WINAPI PlayStreamIntern(LPVOID argument);
    void                CompleteStreamConnect(HSTREAM pSound);
    void                ApplyFxEffects();
    void                ServiceVars();
    void                SetFinishedCallbacks();

    void Process3D(const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt);

public:
    SSoundThreadVariables* m_pVars;
    uint                   uiEndSyncCount;
    bool                   bFreeSync;

private:
    const bool    m_bStream;
    const SString m_strPath;
    const bool    m_b3D;
    const bool    m_bThrottle;
    void*         m_pBuffer;
    unsigned int  m_uiBufferLength;

    bool  m_bPendingPlay;
    DWORD m_pSound;

    // Playback state
    bool    m_bLoop;
    bool    m_bPaused;
    bool    m_bReversed;
    bool    m_bPan;
    float   m_fDefaultFrequency;
    float   m_fVolume;
    float   m_fMinDistance;
    float   m_fMaxDistance;
    float   m_fPlaybackSpeed;
    float   m_fPitch;
    float   m_fTempo;
    float   m_fSampleRate;
    CVector m_vecPosition;
    CVector m_vecVelocity;

    SFixedArray<int, 9> m_EnabledEffects;
    SFixedArray<HFX, 9> m_FxEffects;

    // Info
    SString                    m_strStreamName;
    SString                    m_strStreamTitle;
    std::map<SString, SString> m_ConvertedTagMap;

    std::list<SSoundEventInfo> m_EventQueue;
    float                      m_fBPM;

    void* m_uiCallbackId;
    HSYNC m_hSyncDownload;
    HSYNC m_hSyncEnd;
    HSYNC m_hSyncFree;
    HSYNC m_hSyncMeta;
};
