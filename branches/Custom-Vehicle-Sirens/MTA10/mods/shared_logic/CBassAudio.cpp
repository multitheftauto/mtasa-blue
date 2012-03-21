/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:  me'n'caz'n'flobu
*
*****************************************************************************/

#include <StdInc.h>
#include "CBassAudio.h"
#include <process.h>
#include <tags.h>
#include <bassmix.h>
#include <basswma.h>

#define INVALID_FX_HANDLE (-1)  // Hope that BASS doesn't use this as a valid Fx handle

CBassAudio::CBassAudio ( bool bStream, const SString& strPath, bool bLoop, bool b3D )
    : m_bStream ( bStream )
    , m_strPath ( strPath )
    , m_bLoop ( bLoop )
    , m_b3D ( b3D )
{
    m_fVolume = 1.0f;
    m_fDefaultFrequency = 44100.0f;
    m_fMinDistance = 5.0f;
    m_fMaxDistance = 20.0f;
    m_fPlaybackSpeed = 1.0f;
    m_bPaused = false;
}


CBassAudio::~CBassAudio ( void )
{
    if ( m_pSound )
        BASS_ChannelStop ( m_pSound );

    // Stream threads:
    //  BASS has been told to stop at this point, so it is assumed that it will not initiate any new threaded callbacks.
    //  However m_pThread could still be active and may still create a sound handle.
    //   So, we decrement the ref count on the shared variables
    //   If BASS_StreamCreateURL still holds a ref to the shared variables, any sound handle it may create will
    //   get cleaned up when it releases its ref.
    if ( m_pVars )
    {
        m_pVars->Release ();  // Ref for main thread can now be released
        m_pVars = NULL;
    }

#ifdef MTA_DEBUG // OutputDebugLine only works in debug mode!
    if ( m_bStream )
        OutputDebugLine ( "[Bass]        stream destroyed" );
    else
        OutputDebugLine ( "[Bass] sound destroyed" );
#endif
}


//
// This will return false for non streams if the file is not correct
//
bool CBassAudio::BeginLoadingMedia ( void )
{
    assert ( !m_pSound && !m_bPendingPlay );

    // Calc the flags
    long lFlags = BASS_STREAM_AUTOFREE | BASS_SAMPLE_SOFTWARE;   
#if 0   // Everything sounds better in ste-reo
    if ( m_b3D )
        lFlags |= BASS_SAMPLE_MONO;
#endif
    if ( m_bLoop )
        lFlags |= BASS_SAMPLE_LOOP;

    if ( m_bStream )
    {
        //
        // For streams, begin the connect sequence
        //
        assert ( !m_pVars );
        m_pVars = new SSoundThreadVariables ();
        m_pVars->iRefCount = 2;     // One for here, one for BASS_StreamCreateURL
        m_pVars->strURL = m_strPath;
        m_pVars->lFlags = lFlags;
        CreateThread ( NULL, 0, reinterpret_cast <LPTHREAD_START_ROUTINE> ( &CBassAudio::PlayStreamIntern ), m_pVars, 0, NULL );
        m_bPendingPlay = true;
        OutputDebugLine ( "[Bass]        stream connect started" );
    }
    else
    {
        //
        // For non streams, try to load the sound file
        //
        m_pSound = BASS_StreamCreateFile ( false, m_strPath, 0, 0, lFlags );
        if ( !m_pSound )
            m_pSound = BASS_MusicLoad ( false, m_strPath, 0, 0, lFlags, 0 );  // Try again
        if ( !m_pSound && m_b3D )
            m_pSound = ConvertFileToMono ( m_strPath );                       // Last try if 3D

        // Failed to load ?
        if ( !m_pSound )
        {
            g_pCore->GetConsole()->Printf ( "BASS ERROR %d in LoadMedia  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), *m_strPath, m_b3D, m_bLoop );
            return false;
        }

        // Validation of some sort
        if ( m_bLoop && BASS_ChannelFlags ( m_pSound, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP ) == -1 )
            g_pCore->GetConsole()->Printf ( "BASS ERROR %d in LoadMedia ChannelFlags LOOP  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), *m_strPath, m_b3D, m_bLoop );

        BASS_ChannelGetAttribute ( m_pSound, BASS_ATTRIB_FREQ, &m_fDefaultFrequency );
        m_bPendingPlay = true;
        SetFinishedCallbacks ();
        OutputDebugLine ( "[Bass] sound loaded" );
    }

    return true;
}

//
// Util use in BeginLoadingMedia
//
HSTREAM CBassAudio::ConvertFileToMono(const SString& strPath)
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
    HSTREAM stream = BASS_StreamCreate ( ci.freq, 1, BASS_STREAM_AUTOFREE, STREAMPROC_PUSH, NULL ); // create stream
    BASS_StreamPutData ( stream, data, length ); // set the stream data
    free ( data ); // free the buffer
    return stream;
}


//
// Thread callbacks
//

void CALLBACK DownloadSync ( HSYNC handle, DWORD channel, DWORD data, void* user )
{
    CBassAudio* pBassAudio = static_cast <CBassAudio*> ( user );

    pBassAudio->m_pVars->criticalSection.Lock ();
    pBassAudio->m_pVars->onClientSoundFinishedDownloadQueue.push_back ( pBassAudio->GetLength () );
    pBassAudio->m_pVars->criticalSection.Unlock ();
}

// get stream title from metadata and send it as event
void CALLBACK MetaSync( HSYNC handle, DWORD channel, DWORD data, void *user )
{
    CBassAudio* pBassAudio = static_cast <CBassAudio*> ( user );

    pBassAudio->m_pVars->criticalSection.Lock ();
    DWORD pSound = pBassAudio->m_pVars->pSound;
    pBassAudio->m_pVars->criticalSection.Unlock ();

    SString strMeta = BASS_ChannelGetTags( pSound, BASS_TAG_META );

    if ( !strMeta.empty () )
    {
    	pBassAudio->m_pVars->criticalSection.Lock ();
    	pBassAudio->m_pVars->onClientSoundChangedMetaQueue.push_back ( strMeta );
    	pBassAudio->m_pVars->criticalSection.Unlock ();
	}
}

void CBassAudio::PlayStreamIntern ( void* arguments )
{
    SSoundThreadVariables* pArgs = static_cast <SSoundThreadVariables*> ( arguments );

    // Try to load the sound file
    HSTREAM pSound = BASS_StreamCreateURL ( pArgs->strURL, 0, pArgs->lFlags, NULL, NULL );

    pArgs->criticalSection.Lock ();
    pArgs->bStreamCreateResult = true;
    pArgs->pSound = pSound;
    pArgs->criticalSection.Unlock ();
    pArgs->Release ();  // Ref for BASS_StreamCreateURL can now be released
}

//
// Called from the main thread during DoPulse
//
void CBassAudio::CompleteStreamConnect ( HSTREAM pSound )
{
    if ( pSound )
    {
        m_pSound = pSound;

        BASS_ChannelGetAttribute ( pSound, BASS_ATTRIB_FREQ, &m_fDefaultFrequency );
        BASS_ChannelSetAttribute ( pSound, BASS_ATTRIB_FREQ, m_fPlaybackSpeed * m_fDefaultFrequency );
        if ( !m_b3D )
            BASS_ChannelSetAttribute( pSound, BASS_ATTRIB_VOL, m_fVolume );
        ApplyFxEffects ();

        // Set a Callback function for download finished or connection closed prematurely
        BASS_ChannelSetSync ( pSound, BASS_SYNC_DOWNLOAD, 0, &DownloadSync, this );
        SetFinishedCallbacks ();

        // get the broadcast name
        const char* szIcy;
        if ( 
            ( szIcy = BASS_ChannelGetTags ( pSound, BASS_TAG_ICY ) )
         || ( szIcy = BASS_ChannelGetTags ( pSound, BASS_TAG_WMA ) )
         || ( szIcy = BASS_ChannelGetTags ( pSound, BASS_TAG_HTTP ) )
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
    }
    else
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PlayStream  b3D = %s  path = %s", BASS_ErrorGetCode(), m_b3D ? "true" : "false", m_strPath.c_str() );

    OutputDebugLine ( "[Bass]        stream connect complete" );

    AddQueuedEvent ( SOUND_EVENT_STREAM_RESULT, m_strStreamName, GetLength (), pSound ? true : false );
}


//
// Finish detection
//
void CALLBACK EndSync ( HSYNC handle, DWORD channel, DWORD data, void* user )
{
    CBassAudio* pBassAudio = static_cast <CBassAudio*> ( user );
    pBassAudio->bEndSync = true;
}

void CALLBACK FreeSync ( HSYNC handle, DWORD channel, DWORD data, void* user )
{
    CBassAudio* pBassAudio = static_cast <CBassAudio*> ( user );
    pBassAudio->bFreeSync = true;
}


void CBassAudio::SetFinishedCallbacks ( void )
{
    BASS_ChannelSetSync ( m_pSound, BASS_SYNC_END, 0, &EndSync, this );
    BASS_ChannelSetSync ( m_pSound, BASS_SYNC_FREE, 0, &FreeSync, this );
}


//
// CBassAudio::IsFinished
//
bool CBassAudio::IsFinished ( void )
{
    // Sound is determined finished if BASS has freed the sound handle
    if ( bFreeSync )
        return true;
    return false;
}


//
//
// Lake of sets
//
//
void CBassAudio::SetPaused ( bool bPaused )
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

// Non-streams only
void CBassAudio::SetPlayPosition ( double dPosition )
{
    // Only relevant for non-streams, which are always ready if valid
    if ( m_pSound )
    {
        // Make sure position is in range
        QWORD bytePosition = BASS_ChannelSeconds2Bytes( m_pSound, dPosition );
        QWORD byteLength = BASS_ChannelGetLength( m_pSound, BASS_POS_BYTE );
        BASS_ChannelSetPosition( m_pSound, Clamp < QWORD > ( 0, bytePosition, byteLength - 1 ), BASS_POS_BYTE );
    }
}

// Non-streams only
double CBassAudio::GetPlayPosition ( void )
{
    // Only relevant for non-streams, which are always ready if valid
    if ( m_pSound )
    {
        QWORD pos = BASS_ChannelGetPosition( m_pSound, BASS_POS_BYTE );
        if ( pos != -1 )
            return BASS_ChannelBytes2Seconds( m_pSound, pos );
    }
    return 0;
}

// Non-streams only
double CBassAudio::GetLength ( void )
{
    // Only relevant for non-streams, which are always ready if valid
    if ( m_pSound )
    {
        QWORD length = BASS_ChannelGetLength( m_pSound, BASS_POS_BYTE );
        if ( length != -1 )
            return BASS_ChannelBytes2Seconds( m_pSound, length );
    }
    return 0;
}

// Streams only
SString CBassAudio::GetMetaTags( const SString& strFormat )
{
    SString strMetaTags;
    if ( strFormat == "streamName" )
        strMetaTags = m_strStreamName;
    else
    if ( strFormat == "streamTitle" )
        strMetaTags = m_strStreamTitle;
    else
    if ( m_pSound )
    {
        strMetaTags = TAGS_Read( m_pSound, strFormat );

        if ( strMetaTags == "" )
        {
            // Try using data from from shoutcast meta
            SString* pstrResult = MapFind ( m_ConvertedTagMap, strFormat );
            if ( pstrResult )
                strMetaTags = *pstrResult;
        }

    }
    return strMetaTags;
}

void CBassAudio::SetVolume ( float fVolume, bool bStore )
{
    m_fVolume = fVolume;

    if ( m_pSound && !m_b3D )
        BASS_ChannelSetAttribute( m_pSound, BASS_ATTRIB_VOL, fVolume );
}

void CBassAudio::SetPlaybackSpeed ( float fSpeed )
{
    m_fPlaybackSpeed = fSpeed;

    if ( m_pSound )
        BASS_ChannelSetAttribute ( m_pSound, BASS_ATTRIB_FREQ, fSpeed * m_fDefaultFrequency );
}

void CBassAudio::SetPosition ( const CVector& vecPosition )
{
    m_vecPosition = vecPosition;
}

void CBassAudio::SetVelocity ( const CVector& vecVelocity )
{
    m_vecVelocity = vecVelocity;
}

void CBassAudio::SetMinDistance ( float fDistance )
{
    m_fMinDistance = fDistance;
}

void CBassAudio::SetMaxDistance ( float fDistance )
{
    m_fMaxDistance = fDistance;
}


//
// FxEffects
//
void CBassAudio::SetFxEffects ( int* pEnabledEffects, uint iNumElements )
{
    // Update m_EnabledEffects array
    for ( uint i = 0 ; i < NUMELMS(m_EnabledEffects) ; i++ )
        m_EnabledEffects[i] = i < iNumElements ? pEnabledEffects[i] : 0;

    // Apply if active
    if ( m_pSound )
        ApplyFxEffects ();
}

//
// Copy state stored in m_EnabledEffects to actual BASS sound
//
void CBassAudio::ApplyFxEffects ( void )
{
    for ( uint i = 0 ; i < NUMELMS(m_FxEffects) && NUMELMS(m_EnabledEffects) ; i++ )
    {
        if ( m_EnabledEffects[i] && !m_FxEffects[i] )
        {
            // Switch on
            m_FxEffects[i] = BASS_ChannelSetFX ( m_pSound, i, 0 );
            if ( !m_FxEffects[i] )
                m_FxEffects[i] = INVALID_FX_HANDLE;
        }
        else
        if ( !m_EnabledEffects[i] && m_FxEffects[i] )
        {
            // Switch off
            if ( m_FxEffects[i] != INVALID_FX_HANDLE )
                BASS_ChannelRemoveFX ( m_pSound, m_FxEffects[i] );
            m_FxEffects[i] = 0;
        }
    }
}

//
// Must be call every frame
//
void CBassAudio::DoPulse ( const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt )
{
    // If the sound is a stream, handle results from other threads
    if ( m_bStream )
        if ( m_pVars )
            ServiceVars ();

    // If the sound isn't ready, we stop here
    if ( !m_pSound )
        return;

    // Update 3D attenuation and panning
    if ( m_b3D )
        Process3D ( vecPlayerPosition, vecCameraPosition, vecLookAt );

    // Apply any pending play request
    if ( m_bPendingPlay )
    {
        m_bPendingPlay = false;
        if ( !m_bPaused )
            BASS_ChannelPlay ( m_pSound, false );
    }
}


void CBassAudio::Process3D ( const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt )
{
    assert ( m_b3D && m_pSound );

    // Limit panning when getting close to the min distance
    float fDistance = DistanceBetweenPoints3D ( vecCameraPosition, m_vecPosition );
    float fPanSharpness = UnlerpClamped ( m_fMinDistance, fDistance, m_fMinDistance * 2 );
    float fPanLimit = Lerp ( 0.35f, fPanSharpness, 1.0f );

    // Pan
    CVector vecLook = vecLookAt - vecCameraPosition;
    CVector vecSound = m_vecPosition - vecCameraPosition;
    vecLook.fZ = vecSound.fZ = 0.0f;
    vecLook.Normalize ();
    vecSound.Normalize ();

    vecLook.CrossProduct ( &vecSound );
    // The length of the cross product (which is simply fZ in this case)
    // is equal to the sine of the angle between the vectors
    float fPan = Clamp ( -fPanLimit, -vecLook.fZ, fPanLimit );
    
    BASS_ChannelSetAttribute( m_pSound, BASS_ATTRIB_PAN, fPan );

    // Volume
    float fDistDiff = m_fMaxDistance - m_fMinDistance;

    //Transform e^-x to suit our sound
    float fVolume;
    if ( fDistance <= m_fMinDistance )
        fVolume = 1.0f;
    else if ( fDistance >= m_fMaxDistance )
        fVolume = 0.0f;
    else
        fVolume = exp ( - ( fDistance - m_fMinDistance ) * ( CUT_OFF / fDistDiff ) );

    BASS_ChannelSetAttribute( m_pSound, BASS_ATTRIB_VOL, fVolume * m_fVolume );
}


//
// Handle stored data from other threads
//
void CBassAudio::ServiceVars ( void )
{
    // Temp
    DWORD pSound = 0;
    bool bStreamCreateResult = false;
    std::list < double > onClientSoundFinishedDownloadQueue;
    std::list < SString > onClientSoundChangedMetaQueue;

    // Lock vars
    m_pVars->criticalSection.Lock ();

    // Copy vars to temp
    pSound = m_pVars->pSound;
    bStreamCreateResult = m_pVars->bStreamCreateResult;
    onClientSoundFinishedDownloadQueue = m_pVars->onClientSoundFinishedDownloadQueue;
    onClientSoundChangedMetaQueue = m_pVars->onClientSoundChangedMetaQueue;

    // Clear vars
    m_pVars->bStreamCreateResult = false;
    m_pVars->onClientSoundFinishedDownloadQueue.clear ();
    m_pVars->onClientSoundChangedMetaQueue.clear ();

    // Unlock vars
    m_pVars->criticalSection.Unlock ();

    // Process temp
    if ( bStreamCreateResult )
        CompleteStreamConnect ( pSound );

    // Handle onClientSoundFinishedDownload queue
    while ( !onClientSoundFinishedDownloadQueue.empty () )
    {
        AddQueuedEvent ( SOUND_EVENT_FINISHED_DOWNLOAD, "", onClientSoundFinishedDownloadQueue.front () );
        onClientSoundFinishedDownloadQueue.pop_front ();
    }

    // Handle onClientSoundChangedMeta queue
    while ( !onClientSoundChangedMetaQueue.empty () )
    {
        SString strMeta = onClientSoundChangedMetaQueue.front ();
        ParseShoutcastMeta ( strMeta );
        AddQueuedEvent ( SOUND_EVENT_CHANGED_META, m_strStreamTitle );
        onClientSoundChangedMetaQueue.pop_front ();
    }
}

//
// Extract and map data from a shoutcast meta string
//
void CBassAudio::ParseShoutcastMeta ( const SString& strMeta )
{
    // Get title
    int startPos = strMeta.find ( "=" );
    SString strStreamTitle = strMeta.SubStr ( startPos + 2, strMeta.find ( ";" ) - startPos - 3 );

    if ( !strStreamTitle.empty () )
        m_strStreamTitle = strStreamTitle;

    // Get url
    startPos = strMeta.find ( "=" , startPos + 1 );
    SString strStreamUrl = strMeta.SubStr ( startPos + 2, strMeta.find ( ";", startPos ) - startPos - 3 );

    // Extract info from url
    CArgMap shoutcastInfo;
    shoutcastInfo.SetEscapeCharacter ( '%' );
    shoutcastInfo.SetFromString ( strStreamUrl );

    // Convert from shoutcast identifiers to map of tags
    static const char* convList[] = {
                        // Mapable
                        "%ARTI", "artist",
                        "%TITL", "title",
                        "%ALBM", "album",

                        // Mapable, but possibly don't exist
                        "%GNRE", "genre",
                        "%YEAR", "year",
                        "%CMNT", "comment",
                        "%TRCK", "track",
                        "%COMP", "composer",
                        "%COPY", "copyright",
                        "%SUBT", "subtitle",
                        "%AART", "albumartist",

                        // Not mapabale
                        "%DURATION", "duration",
                        "%SONGTYPE", "songtype",
                        "%OVERLAY", "overlay",
                        "%BUYCD", "buycd",
                        "%WEBSITE", "website",
                        "%PICTURE", "picture",
                   };

    std::vector < SString > shoutcastKeyList;
    shoutcastInfo.GetKeys ( shoutcastKeyList );

    // For each shoutcast pair
    for ( std::vector < SString >::iterator iter = shoutcastKeyList.begin () ; iter != shoutcastKeyList.end () ; ++iter )
    {
        const SString& strKey = *iter;
        SString strValue = shoutcastInfo.Get ( strKey );

        // Find %TAG match
        for ( uint i = 0 ; i < NUMELMS( convList ) - 1 ; i += 2 )
        {
            if ( strKey == convList[ i + 1 ] )
            {
                MapSet ( m_ConvertedTagMap, convList[ i ], strValue );
                break;
            }
        }
    }
}


//
// Add queued event from
//
void CBassAudio::AddQueuedEvent ( eSoundEventType type, const SString& strString, double dNumber, bool bBool )
{
    SSoundEventInfo info;
    info.type = type;
    info.strString = strString;
    info.dNumber = dNumber;
    info.bBool = bBool;
    m_EventQueue.push_back ( info );
}


//
// Get next queued event
//
bool CBassAudio::GetQueuedEvent ( SSoundEventInfo& info )
{
    if ( m_EventQueue.empty () )
        return false;

    info = m_EventQueue.front ();
    m_EventQueue.pop_front ();
    return true;
}


///////////////////////////////////////////////////////
//
// SSoundThreadVariables::Release
//
// This gets called when BASS_StreamCreateURL has completed or when CBassAudio is destroyed
//
///////////////////////////////////////////////////////
void SSoundThreadVariables::Release ( void )
{
    criticalSection.Lock ();
    assert ( iRefCount > 0 );
    bool bLastRef = --iRefCount == 0;
    criticalSection.Unlock ();

    if ( !bLastRef )
        return;

    // Cleanup any pSound created by BASS_StreamCreateURL that has not been handled
    if ( bStreamCreateResult )
        if ( pSound )
            BASS_ChannelStop ( pSound );

    delete this;
}
