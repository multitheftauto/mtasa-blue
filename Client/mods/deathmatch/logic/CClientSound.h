/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientSound.h
 *  PURPOSE:     Sound entity class header
 *
 *****************************************************************************/

class CClientSound;
class CBassAudio;

#pragma once

#include "CClientSoundManager.h"
#include "CClientEntity.h"
#include "CSimulatedPlayPosition.h"

class CClientSound final : public CClientEntity
{
    DECLARE_CLASS(CClientSound, CClientEntity)
    friend class CClientSoundManager;

public:
    CClientSound(CClientManager* pManager, ElementID ID);
    ~CClientSound();

    virtual CSphere GetWorldBoundingSphere();

    eClientEntityType GetType() const { return CCLIENTSOUND; }

    bool Play(const SString& strPath, bool bLoop);
    bool Play(void* pMemory, unsigned int uiLength, bool bLoop);
    bool Play3D(const SString& strPath, bool bLoop);
    bool Play3D(void* pMemory, unsigned int uiLength, bool bLoop);
    void PlayStream(const SString& strURL, bool bLoop, bool bThrottle, bool b3D = false);

    void SetPaused(bool bPaused);
    bool IsPaused();

    bool SetLooped(bool bLoop);
    bool IsLooped() const;

    bool   SetPlayPosition(double dPosition);
    double GetPlayPosition();

    double GetLength(bool bAvoidLoad = false);
    double GetBufferLength();

    void  SetVolume(float fVolume, bool bStore = true);
    float GetVolume();

    void  SetPlaybackSpeed(float fSpeed);
    float GetPlaybackSpeed();

    void GetPosition(CVector& vecPosition) const;
    void SetPosition(const CVector& vecPosition);

    void GetVelocity(CVector& vecVelocity);
    void SetVelocity(const CVector& vecVelocity);

    void  SetMinDistance(float fDistance);
    float GetMinDistance();

    void  SetMaxDistance(float fDistance);
    float GetMaxDistance();

    void   ApplyFXModifications(float fSampleRate, float fTempo, float fPitch, bool bReversed);
    void   GetFXModifications(float& fSampleRate, float& fTempo, float& fPitch, bool& bReversed);
    float* GetFFTData(int iLength);
    float* GetWaveData(int iLength);
    bool   IsPanEnabled();
    bool   SetPanEnabled(bool bPan);
    DWORD  GetLevelData();
    float  GetSoundBPM();

    SString GetMetaTags(const SString& strFormat);

    bool SetPan(float fPan);
    bool GetPan(float& fPan);

    bool SetFxEffect(uint uiFxEffect, bool bEnable);
    bool IsFxEffectEnabled(uint uiFxEffect);

    void Unlink(){};

    bool IsSoundStopped() { return m_pAudio == NULL; }
    bool IsFinished();

    bool IsSound3D() { return m_b3D; }
    bool IsSoundStream() { return m_bStream; }

protected:
    void Process3D(const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt);
    void BeginSimulationOfPlayPosition();
    void EndSimulationOfPlayPositionAndApply();
    void DistanceStreamIn();
    void DistanceStreamOut();
    bool Create();
    void Destroy();

private:
    CClientSoundManager*   m_pSoundManager;
    CSimulatedPlayPosition m_SimulatedPlayPosition;
    CBassAudio*            m_pAudio;

    // Initial state
    bool         m_bStream;
    bool         m_b3D;
    SString      m_strPath;
    bool         m_bLoop;
    bool         m_bThrottle;
    void*        m_pBuffer;
    unsigned int m_uiBufferLength;

    // Info
    bool                       m_bDoneCreate;
    double                     m_dLength;
    std::map<SString, SString> m_SavedTags;

    // Playback altering stuff
    float m_fPitch;
    float m_fTempo;
    float m_fSampleRate;
    bool  m_bReversed;
    bool  m_bPan;
    float m_fPan;

    // Saved state
    bool                m_bPaused;
    float               m_fVolume;
    float               m_fPlaybackSpeed;
    CVector             m_vecPosition;
    CVector             m_vecVelocity;
    float               m_fMinDistance;
    float               m_fMaxDistance;
    SFixedArray<int, 9> m_EnabledEffects;
    uint                m_uiFrameNumberCreated;
};
