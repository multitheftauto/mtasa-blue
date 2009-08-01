/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadio.h
*  PURPOSE:     Radio class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#ifndef __CCLIENTRADIO_H
#define __CCLIENTRADIO_H

class CClientManager;

enum eRadioChannel
{
    CHANNEL_POLICE=0,
    CHANNEL_PLAYBACK_FM,
    CHANNEL_K_ROSE,
    CHANNEL_K_DST,
    CHANNEL_BOUNCE_FM,
    CHANNEL_SF_UR,
    CHANNEL_RADIO_LOS_SANTOS,
    CHANNEL_RADIO_X,
    CHANNEL_CSR,
    CHANNEL_K_JAH_WEST,
    CHANNEL_MASTER_SOUNDS,
    CHANNEL_WCTR,
    CHANNEL_USER_TRACK_PLAYER,
    CHANNEL_RADIO_OFF,
};

class CClientRadio
{
public:
                                CClientRadio        ( CClientManager * pManager );
                                ~CClientRadio       ( void );

    void                        DoPulse             ( void );
    void                        Render              ( void );

    bool                        IsPlaying           ( void );    
    void                        Start               ( void );
    void                        Stop                ( void );

    inline eRadioChannel        GetChannel          ( void )    { return m_Channel; }
    void                        SetChannel          ( eRadioChannel channel );
    char *                      GetChannelName      ( void );

    void                        NextChannel         ( void );
    void                        PreviousChannel     ( void );

private:
    CClientManager *            m_pManager;
    eRadioChannel               m_Channel;
    bool                        m_bIsPlaying;
    unsigned long               m_ulChangeTick;
    bool                        m_bPoliceRadio;
};


#endif