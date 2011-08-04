/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSound.cpp
*  PURPOSE:     Sound entity class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*               arc_
*               Florian Busse <flobu@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>
#include <process.h>
#include <tags.h>
#include <bassmix.h>
#include <basswma.h>

extern CClientGame* g_pClientGame;

CClientSound::CClientSound ( CClientManager* pManager, ElementID ID ) : ClassInit ( this ), CClientEntity ( ID )
{
    m_pManager = pManager;
    m_pSoundManager = pManager->GetSoundManager();
    m_pSound = NULL;

    SetTypeName ( "sound" );

    m_pSoundManager->AddToList ( this );

    RelateDimension ( pManager->GetSoundManager ()->GetDimension () );

    m_strPath = "";
    m_fVolume = 1.0f;
    m_fDefaultFrequency = 44100.0f;
    m_fMinDistance = 5.0f;
    m_fMaxDistance = 20.0f;
    m_fPlaybackSpeed = 1.0f;
    m_usDimension = 0;
    m_b3D = false;
    m_bPaused = false;

    m_strStreamName = "";
    m_strStreamTitle = "";

    for ( int i=0; i<9; i++ )
        m_FxEffects[i] = 0;
}

CClientSound::~CClientSound ( void )
{
    if ( m_pSound )
        BASS_ChannelStop ( m_pSound );

    m_pSoundManager->RemoveFromList ( this );

    // Stream threads:
    //  BASS has been told to stop at this point, so it is assumed that it will not initiate any new threaded callbacks.
    //  However m_pThread could still be active and may still create a sound handle.
    //   So, we terminate m_pThread (could be bad, but worry about that later)
    //   and if a sound handle has been made, we tell BASS to stop that also
    m_VarsCriticalSection.Lock ();
    HANDLE pThread = m_Vars.pThread;
    m_VarsCriticalSection.Unlock ();

    if ( pThread )
        TerminateThread ( pThread, 0 );

    // m_VarsCriticalSection could be in any state now, so we don't use it
    if ( m_Vars.bStreamCreateResult )
        if ( m_Vars.pSound )
            BASS_ChannelStop ( m_Vars.pSound );
}

bool CClientSound::Play ( const SString& strPath, bool bLoop )
{
    long lFlags = BASS_STREAM_AUTOFREE;
    if ( bLoop )
        lFlags = BASS_SAMPLE_LOOP;

    // Try to load the sound file
    if (
        ( m_pSound = BASS_StreamCreateFile ( false, strPath, 0, 0, lFlags ) )
     || ( m_pSound = BASS_MusicLoad ( false, strPath, 0, 0, lFlags, 0) )
        )
    {
        m_strPath = strPath;
        BASS_ChannelPlay ( m_pSound, false );
        BASS_ChannelGetAttribute ( m_pSound, BASS_ATTRIB_FREQ, &m_fDefaultFrequency );
        return true;
    }
    g_pCore->GetConsole()->Printf ( "BASS ERROR %d in Play  path = %s", BASS_ErrorGetCode(), strPath.c_str() );
    return false;
}

HSTREAM CClientSound::ConvertFileToMono(const SString& strPath)
{
    HSTREAM decoder = BASS_StreamCreateFile ( false, strPath, 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_MONO ); // open file for decoding
    if ( !decoder )
        return 0; // failed
    DWORD length = static_cast <DWORD> ( BASS_ChannelGetLength ( decoder, BASS_POS_BYTE ) ); // get the length
    void *data = malloc ( length ); // allocate buffer for decoded data
    BASS_CHANNELINFO ci;
    BASS_ChannelGetInfo ( decoder, &ci ); // get sample format
    if ( ci.chans > 1 ) // not mono, downmix...
    {
        HSTREAM mixer = BASS_Mixer_StreamCreate ( ci.freq, 1, BASS_STREAM_DECODE | BASS_MIXER_END ); // create mono mixer
        BASS_Mixer_StreamAddChannel ( mixer, decoder, BASS_MIXER_DOWNMIX | BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE ); // plug-in the decoder (auto-free with the mixer)
        decoder = mixer; // decode from the mixer
    }
    length = BASS_ChannelGetData ( decoder, data, length ); // decode data
    BASS_StreamFree ( decoder ); // free the decoder/mixer
    HSTREAM stream = BASS_StreamCreate ( ci.freq, 1, BASS_STREAM_AUTOFREE | BASS_SAMPLE_3D, STREAMPROC_PUSH, NULL ); // create stream
    BASS_StreamPutData ( stream, data, length ); // set the stream data
    free ( data ); // free the buffer
    return stream;
}

bool CClientSound::Play3D ( const SString& strPath, const CVector& vecPosition, bool bLoop )
{
    //long lFlags = BASS_STREAM_AUTOFREE | BASS_SAMPLE_3D | BASS_SAMPLE_MONO;
    long lFlags = BASS_STREAM_AUTOFREE | BASS_SAMPLE_MONO;

    // Try to load the sound file
    if (
        ( m_pSound = BASS_StreamCreateFile ( false, strPath, 0, 0, lFlags ) )
     || ( m_pSound = BASS_MusicLoad ( false, strPath, 0, 0, lFlags, 0) )
     || ( m_pSound = ConvertFileToMono ( strPath ) ) //this interrupts the game, depends on the file size and length
        )
    {
        if ( bLoop && BASS_ChannelFlags ( m_pSound, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP ) == -1 )
            g_pCore->GetConsole()->Printf ( "BASS ERROR %d in Play3D ChannelFlags LOOP  path = %s", BASS_ErrorGetCode(), strPath.c_str() );

        m_b3D = true;
        m_strPath = strPath;
        m_vecPosition = vecPosition;
        m_vecVelocity = CVector( 0, 0, 0 );

        //BASS_3DVECTOR pos ( m_vecPosition.fX, m_vecPosition.fY, m_vecPosition.fZ );
        //BASS_3DVECTOR vel ( m_vecVelocity.fX, m_vecVelocity.fY, m_vecVelocity.fZ );
        //BASS_ChannelSet3DPosition ( m_pSound, &pos, NULL, &vel );
        //BASS_ChannelSet3DAttributes ( m_pSound, BASS_3DMODE_NORMAL, 1.0f, 0.5f, 360, 360, 1.0f );
        BASS_ChannelPlay ( m_pSound, false );
        BASS_ChannelGetAttribute ( m_pSound, BASS_ATTRIB_FREQ, &m_fDefaultFrequency );
        return true;
    }
    g_pCore->GetConsole()->Printf ( "BASS ERROR %d in Play3D  path = %s", BASS_ErrorGetCode(), strPath.c_str() );
    return false;
}

void CClientSound::PlayStream ( const SString& strURL, bool bLoop, bool b3D, const CVector& vecPosition )
{
    m_strPath = strURL;
    m_b3D = b3D;
    m_vecPosition = vecPosition;

    long lFlags = BASS_STREAM_AUTOFREE;
    if ( b3D )
    {
        m_b3D = true;
        m_vecPosition = vecPosition;
        //lFlags |= BASS_SAMPLE_3D | BASS_SAMPLE_MONO;
        lFlags |= BASS_SAMPLE_MONO;
    }
    if ( bLoop )
        lFlags |= BASS_SAMPLE_LOOP;

    thestruct* pArguments = new thestruct;
    pArguments->pClientSound = this;
    pArguments->strURL = strURL;
    pArguments->lFlags = lFlags;

    // Stream the file in a seperate thread to don't interupt the game
    assert ( !m_bUsingVars );
    m_bUsingVars = true;
    m_VarsCriticalSection.Lock ();
    m_Vars.pThread = CreateThread ( NULL, 0, reinterpret_cast <LPTHREAD_START_ROUTINE> ( &CClientSound::PlayStreamIntern ), pArguments, 0, NULL );
    m_VarsCriticalSection.Unlock ();
}

void CClientSound::PlayStreamIntern ( void* arguments )
{
    thestruct* pArgs = static_cast <thestruct*> ( arguments );

    // Try to load the sound file
    HSTREAM pSound = BASS_StreamCreateURL ( pArgs->strURL, 0, pArgs->lFlags, NULL, NULL );

    pArgs->pClientSound->m_VarsCriticalSection.Lock ();
    pArgs->pClientSound->m_Vars.bStreamCreateResult = true;
    pArgs->pClientSound->m_Vars.pSound = pSound;
    pArgs->pClientSound->m_Vars.pThread = NULL;
    pArgs->pClientSound->m_VarsCriticalSection.Unlock ();
    delete arguments;
}

void CALLBACK DownloadSync ( HSYNC handle, DWORD channel, DWORD data, void* user )
{
    CClientSound* pClientSound = static_cast <CClientSound*> ( user );

    pClientSound->m_VarsCriticalSection.Lock ();
    pClientSound->m_Vars.onClientSoundFinishedDownloadQueue.push_back ( pClientSound->GetLength () );
    pClientSound->m_VarsCriticalSection.Unlock ();
}

// get stream title from metadata and send it as event
void CALLBACK MetaSync( HSYNC handle, DWORD channel, DWORD data, void *user )
{
    //g_pCore->GetConsole()->Printf ( "BASS STREAM META" );
    CClientSound* pClientSound = static_cast <CClientSound*> ( user );

    pClientSound->m_VarsCriticalSection.Lock ();
    DWORD pSound = pClientSound->m_Vars.pSound;
    pClientSound->m_VarsCriticalSection.Unlock ();

    SString strMeta = BASS_ChannelGetTags( pSound, BASS_TAG_META );
    SString strStreamTitle;
    if ( !strMeta.empty () )// got Shoutcast metadata
    {
        int startPos = strMeta.find("=");
        strStreamTitle = strMeta.substr(startPos + 2,strMeta.find(";") - startPos - 3);
    }
    //else
    //    g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_TAG_META", BASS_ErrorGetCode() );

    /* TESTING ( Found no stream which have those tags )
    szMeta=BASS_ChannelGetTags(m_pSound,BASS_TAG_OGG);
    if (szMeta)// got Icecast/OGG tags
    {
        for (;*szMeta;szMeta+=strlen(szMeta)+1) {
            g_pCore->GetConsole()->Printf ( "BASS_TAG_OGG  %s", szMeta );
        }
    }
    else
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_TAG_OGG", BASS_ErrorGetCode() );

    szMeta=BASS_ChannelGetTags(m_pSound,BASS_TAG_WMA_META);
    if (szMeta) // got a script/mid-stream tag, display it
        g_pCore->GetConsole()->Printf ( "BASS_TAG_WMA_META  %s", szMeta );
    else
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_TAG_WMA_META", BASS_ErrorGetCode() );
    //*/
    //g_pCore->GetConsole()->Printf ( "BASS STREAM META END count = %u", Arguments.Count () );

    if ( !strStreamTitle.empty () )
    {
    	pClientSound->m_VarsCriticalSection.Lock ();
    	pClientSound->m_Vars.onClientSoundChangedMetaQueue.push_back ( strStreamTitle );
    	pClientSound->m_VarsCriticalSection.Unlock ();
	}
}

/* TESTING ( Found no stream which has the real title and author in it )
void CALLBACK WMAChangeSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
    const char* szIcy = BASS_ChannelGetTags ( channel, BASS_TAG_WMA );
    if (szIcy)
        for (;*szIcy;szIcy+=strlen(szIcy)+1)
        {
            g_pCore->GetConsole()->Printf ( "BASS_TAG_WMA CHANGE  %s", szIcy );
        }
    else
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_TAG_WMA CHANGE", BASS_ErrorGetCode() );
}
//*/

void CClientSound::ThreadCallback ( HSTREAM pSound )
{
    if ( pSound )
    {
        m_pSound = pSound;

        BASS_ChannelGetAttribute ( pSound, BASS_ATTRIB_FREQ, &m_fDefaultFrequency );

        if ( m_b3D )
        {
            /*BASS_3DVECTOR pos ( m_vecPosition.fX, m_vecPosition.fY, m_vecPosition.fZ );
            BASS_3DVECTOR vel ( m_vecVelocity.fX, m_vecVelocity.fY, m_vecVelocity.fZ );
            BASS_ChannelSet3DPosition ( pSound, &pos, NULL, &vel );
            BASS_ChannelSet3DAttributes ( pSound, BASS_3DMODE_NORMAL, 1.0f, 0.5f, 360, 360, 1.0f );*/
        }
        
        BASS_ChannelSetAttribute( pSound, BASS_ATTRIB_VOL, m_fVolume );
        BASS_ChannelSetAttribute ( pSound, BASS_ATTRIB_FREQ, m_fPlaybackSpeed * m_fDefaultFrequency );

        // Set a Callback function for download finished
        BASS_ChannelSetSync ( pSound, BASS_SYNC_DOWNLOAD, 0, &DownloadSync, this );

        /* TESTING
        const char* szIcy = BASS_ChannelGetTags ( m_pSound, BASS_TAG_ICY );
        if (szIcy)
            for (;*szIcy;szIcy+=strlen(szIcy)+1)
            {
                g_pCore->GetConsole()->Printf ( "BASS_TAG_ICY  %s", szIcy );
            }
        else
            g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_TAG_ICY", BASS_ErrorGetCode() );


        szIcy = BASS_ChannelGetTags ( m_pSound, BASS_TAG_HTTP );
        if (szIcy)
            for (;*szIcy;szIcy+=strlen(szIcy)+1)
            {
                g_pCore->GetConsole()->Printf ( "BASS_TAG_HTTP  %s", szIcy );
            }
        else
            g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_TAG_HTTP", BASS_ErrorGetCode() );

        szIcy = BASS_ChannelGetTags ( m_pSound, BASS_TAG_WMA );
        if (szIcy)
            for (;*szIcy;szIcy+=strlen(szIcy)+1)
            {
                g_pCore->GetConsole()->Printf ( "BASS_TAG_WMA  %s", szIcy );
            }
        else
            g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_TAG_WMA", BASS_ErrorGetCode() );
        //*/

        // get the broadcast name
        const char* szIcy;
        if ( 
            ( szIcy = BASS_ChannelGetTags ( m_pSound, BASS_TAG_ICY ) )
         || ( szIcy = BASS_ChannelGetTags ( m_pSound, BASS_TAG_WMA ) )
         || ( szIcy = BASS_ChannelGetTags ( m_pSound, BASS_TAG_HTTP ) )
            )
        {
            for ( ; *szIcy; szIcy += strlen ( szIcy ) + 1 )
            {
                if ( !strnicmp ( szIcy, "icy-name:", 9 ) ) // ICY / HTTP
                {
                    m_strStreamName = szIcy + 9;
                    break;
                }
                else if ( !strnicmp ( szIcy, "title=", 6 ) ) // WMA
                {
                    m_strStreamName = szIcy + 6;
                    break;
                }
                //g_pCore->GetConsole()->Printf ( "BASS STREAM INFO  %s", szIcy );
            }
        }
        // set sync for stream titles
        BASS_ChannelSetSync( pSound, BASS_SYNC_META, 0, &MetaSync, this); // Shoutcast
        //g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_SYNC_META", BASS_ErrorGetCode() );
        //BASS_ChannelSetSync(pSound,BASS_SYNC_OGG_CHANGE,0,&MetaSync,this); // Icecast/OGG
        //g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_SYNC_OGG_CHANGE", BASS_ErrorGetCode() );
        //BASS_ChannelSetSync(pSound,BASS_SYNC_WMA_META,0,&MetaSync,this); // script/mid-stream tags
        //g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_SYNC_WMA_META", BASS_ErrorGetCode() );
        //BASS_ChannelSetSync(pSound,BASS_SYNC_WMA_CHANGE,0,&WMAChangeSync,this); // server-side playlist changes
        //g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_SYNC_WMA_CHANGE", BASS_ErrorGetCode() );

        if ( !m_bPaused )
            BASS_ChannelPlay ( pSound, false );
    }
    else
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PlayStream  b3D = %s  path = %s", BASS_ErrorGetCode(), m_b3D ? "true" : "false", m_strPath.c_str() );

    // Call onClientSoundStream LUA event
    CLuaArguments Arguments;
    Arguments.PushBoolean ( pSound ? true : false );
    Arguments.PushNumber ( GetLength () );
    if ( !m_strStreamName.empty () )
        Arguments.PushString ( m_strStreamName );
    this->CallEvent ( "onClientSoundStream", Arguments, true );
}

void CClientSound::Stop ( void )
{
    if ( m_pSound )
        BASS_ChannelStop ( m_pSound );

    g_pClientGame->GetElementDeleter()->Delete ( this );
}

void CClientSound::SetPaused ( bool bPaused )
{
    m_bPaused = bPaused;

    if ( m_pSound )
    {
        if ( bPaused )
            BASS_ChannelPause ( m_pSound );
        else
            BASS_ChannelPlay ( m_pSound, false );
    }
}

bool CClientSound::IsPaused ( void )
{
    if ( m_pSound )
    {
        return BASS_ChannelIsActive( m_pSound ) == BASS_ACTIVE_PAUSED;
    }
    return false;
}

bool CClientSound::IsFinished ( void )
{
    if ( m_pSound )
    {
        return BASS_ChannelIsActive( m_pSound ) == BASS_ACTIVE_STOPPED;
    }
    return false;
}

void CClientSound::SetPlayPosition ( unsigned int uiPosition )
{
    if ( m_pSound )
    {
        BASS_ChannelSetPosition( m_pSound, BASS_ChannelSeconds2Bytes( m_pSound, uiPosition/1000 ), BASS_POS_BYTE );
    }
}

unsigned int CClientSound::GetPlayPosition ( void )
{
    if ( m_pSound )
    {
        QWORD pos = BASS_ChannelGetPosition( m_pSound, BASS_POS_BYTE );
        if ( pos != -1 )
            return static_cast <unsigned int> ( BASS_ChannelBytes2Seconds( m_pSound, pos ) * 1000 );
    }
    return 0;
}

unsigned int CClientSound::GetLength ( void )
{
    if ( m_pSound )
    {
        QWORD length = BASS_ChannelGetLength( m_pSound, BASS_POS_BYTE );
        if ( length != -1 )
            return static_cast <unsigned int> ( BASS_ChannelBytes2Seconds( m_pSound, length ) * 1000 );
    }
    return 0;
}

float CClientSound::GetVolume ( void )
{
    return m_fVolume;
}

void CClientSound::SetVolume ( float fVolume, bool bStore )
{
    if ( bStore )
        m_fVolume = fVolume;

    if ( m_pSound && !m_b3D && m_bInSameDimension )
        BASS_ChannelSetAttribute( m_pSound, BASS_ATTRIB_VOL, fVolume );
}

float CClientSound::GetPlaybackSpeed ( void )
{
    return m_fPlaybackSpeed;
}

void CClientSound::SetPlaybackSpeed ( float fSpeed )
{
    m_fPlaybackSpeed = fSpeed;

    if ( m_pSound )
        BASS_ChannelSetAttribute ( m_pSound, BASS_ATTRIB_FREQ, fSpeed * m_fDefaultFrequency );
}

void CClientSound::SetPosition ( const CVector& vecPosition )
{
    m_vecPosition = vecPosition;

    if ( m_pSound )
    {
        /*BASS_3DVECTOR pos ( vecPosition.fX, vecPosition.fY, vecPosition.fZ );
        BASS_ChannelSet3DPosition ( m_pSound, &pos, NULL, NULL);*/
    }
}

void CClientSound::GetPosition ( CVector& vecPosition ) const
{
    vecPosition = m_vecPosition;
}

void CClientSound::SetVelocity ( const CVector& vecVelocity )
{
    m_vecVelocity = vecVelocity;

    if ( m_pSound )
    {
        /*BASS_3DVECTOR vel ( vecVelocity.fX, vecVelocity.fY, vecVelocity.fZ );
        BASS_ChannelSet3DPosition ( m_pSound, NULL, NULL, &vel);*/
    }
}

void CClientSound::GetVelocity ( CVector& vecVelocity )
{
    vecVelocity = m_vecVelocity;
}

void CClientSound::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
    RelateDimension ( m_pManager->GetSoundManager ()->GetDimension () );
}

void CClientSound::RelateDimension ( unsigned short usDimension )
{
    if ( usDimension == m_usDimension )
    {
        m_bInSameDimension = true;
        SetVolume ( m_fVolume, false );
    }
    else
    {
        SetVolume ( 0.0f, false );
        m_bInSameDimension = false;
    }
}

void CClientSound::SetMinDistance ( float fDistance )
{
    m_fMinDistance = fDistance;
}

float CClientSound::GetMinDistance ( void )
{
    return m_fMinDistance;
}

void CClientSound::SetMaxDistance ( float fDistance )
{
    m_fMaxDistance = fDistance;
}

float CClientSound::GetMaxDistance ( void )
{
    return m_fMaxDistance;
}

SString CClientSound::GetMetaTags( const SString& strFormat )
{
    SString strMetaTags = "";
    if ( strFormat == "streamName" )
        strMetaTags = m_strStreamName;
    else if ( strFormat == "streamTitle" )
        strMetaTags = m_strStreamTitle;
    else
        strMetaTags = TAGS_Read( m_pSound, strFormat.c_str() );

    return strMetaTags;
}

bool CClientSound::SetFxEffect ( int iFxEffect, bool bEnable )
{
    if ( m_pSound )
    {
        if ( iFxEffect >= 0 )
        {
            if ( bEnable )
            {
                if ( !m_FxEffects[iFxEffect] )
                {
                    m_FxEffects[iFxEffect] = BASS_ChannelSetFX ( m_pSound, iFxEffect, 0 );
                    if ( m_FxEffects[iFxEffect] )
                        return true;
                }
            }
            else
            {
                if ( BASS_ChannelRemoveFX ( m_pSound, m_FxEffects[iFxEffect] ) )
                {
                    m_FxEffects[iFxEffect] = 0;
                    return true;
                }
            }
        }
    }
    return false;
}

bool CClientSound::IsFxEffectEnabled ( int iFxEffect )
{
    return m_FxEffects[iFxEffect] ? true : false;
}

void CClientSound::Process3D ( CVector vecPosition, CVector vecCameraPosition, CVector vecLookAt )
{
    // Handle results from other threads
    if ( m_bUsingVars )
        ServiceVars ();

    // If the sound isn't 3D, we don't need to process it
    if ( !m_b3D )
        return;

    // Update our position and velocity if we're attached
    CClientEntity* pAttachedToEntity = GetAttachedTo ();
    if ( pAttachedToEntity )
    {
        DoAttaching ();
        CVector vecVelocity;
        if ( CStaticFunctionDefinitions::GetElementVelocity ( *pAttachedToEntity, vecVelocity ) )
            SetVelocity ( vecVelocity );
    }

    // If the sound isn't playing, just having attaching handled is all that's needed here
    if ( !m_pSound )
        return;

    // Initialize fVolume
    float fVolume = 1.0;

    if ( !m_bInSameDimension )
        // We don't need any fancy calculations if the sound is not in our dimension - just mute it already
        fVolume = 0.0f;
    else
    {
        // Pan
        CVector vecLook = vecLookAt - vecCameraPosition;
        CVector vecSound = m_vecPosition - vecCameraPosition;
        vecLook.fZ = vecSound.fZ = 0.0f;
        vecLook.Normalize ();
        vecSound.Normalize ();

        vecLook.CrossProduct ( &vecSound );
        // The length of the cross product (which is simply fZ in this case)
        // is equal to the sine of the angle between the vectors
        float fPan = Clamp ( -1.0f, -vecLook.fZ , 1.0f );
        
        BASS_ChannelSetAttribute( m_pSound, BASS_ATTRIB_PAN, fPan );

        // Volume
        float fDistance = DistanceBetweenPoints3D ( vecPosition, m_vecPosition );
        float fDistDiff = m_fMaxDistance - m_fMinDistance;

        //Transform e^-x to suit our sound
        if ( fDistance <= m_fMinDistance )
            fVolume = 1.0f;
        else if ( fDistance >= m_fMaxDistance )
        {
            fVolume = 0.0f;
            SetPaused( true ); // Actually pause it here ( hopefully BASS stops streaming radio streams! )
            return;
        }
        else
            fVolume = exp ( - ( fDistance - m_fMinDistance ) * ( CUT_OFF / fDistDiff ) );

        if ( IsPaused() )
            SetPaused( false );
    }

    BASS_ChannelSetAttribute( m_pSound, BASS_ATTRIB_VOL, fVolume * m_fVolume );
}


//
// Handle stored data from other threads
//
void CClientSound::ServiceVars ( void )
{
    // Temp
    DWORD pSound = 0;
    bool bStreamCreateResult = false;
    std::list < uint > onClientSoundFinishedDownloadQueue;
    std::list < SString > onClientSoundChangedMetaQueue;

    // Lock vars
    m_VarsCriticalSection.Lock ();

    // Copy vars to temp
    pSound = m_Vars.pSound;
    bStreamCreateResult = m_Vars.bStreamCreateResult;
    onClientSoundFinishedDownloadQueue = m_Vars.onClientSoundFinishedDownloadQueue;
    onClientSoundChangedMetaQueue = m_Vars.onClientSoundChangedMetaQueue;

    // Clear vars
    m_Vars.bStreamCreateResult = false;
    m_Vars.onClientSoundFinishedDownloadQueue.clear ();
    m_Vars.onClientSoundChangedMetaQueue.clear ();

    // Unlock vars
    m_VarsCriticalSection.Unlock ();

    // Process temp
    if ( bStreamCreateResult )
        ThreadCallback ( pSound );

    // Handle onClientSoundFinishedDownload queue
    while ( !onClientSoundFinishedDownloadQueue.empty () )
    {
        CLuaArguments Arguments;
        Arguments.PushNumber ( onClientSoundFinishedDownloadQueue.front () );
        onClientSoundFinishedDownloadQueue.pop_front ();
        CallEvent ( "onClientSoundFinishedDownload", Arguments, true );
    }

    // Handle onClientSoundChangedMeta queue
    while ( !onClientSoundChangedMetaQueue.empty () )
    {
        CLuaArguments Arguments;
        Arguments.PushString ( onClientSoundChangedMetaQueue.front () );
        onClientSoundChangedMetaQueue.pop_front ();
        CallEvent ( "onClientSoundChangedMeta", Arguments, true );
    }
}
