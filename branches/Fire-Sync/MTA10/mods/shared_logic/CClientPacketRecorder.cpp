/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPacketRecorder.cpp
*  PURPOSE:     Client packet recorder and "demo" replay class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

unsigned char m_ucNextPacketID = 0;

CClientPacketRecorder::CClientPacketRecorder ( CClientManager* pManager )
{
    // Init
    m_pManager = pManager;
    m_szFilename = NULL;
    m_pfnPacketHandler = NULL;

    m_lRelative = 0;
    m_lFrames = 0;
    m_bPlaying = false;
    m_bRecording = false;
    m_bFrameBased = false;

    m_ulCurrentOffset = 0;
    m_lNextPacketTime = 0;
    m_uiFrameSkip = 1;
}


CClientPacketRecorder::~CClientPacketRecorder ( void )
{
    Stop ();
}


void CClientPacketRecorder::SetPacketHandler ( PPACKETHANDLER pfnPacketHandler )
{
    m_pfnPacketHandler = pfnPacketHandler;
}


void CClientPacketRecorder::StartPlayback ( const char* szInput, bool bFrameBased )
{
    // Delete the filename
    if ( m_szFilename )
    {
        delete m_szFilename;
        m_szFilename = NULL;
    }

    // Did we get a filename?
    if ( szInput )
    {
        // Create a new filename
        m_szFilename = new char [ strlen ( szInput ) + 1 ];
        strcpy ( m_szFilename, szInput );

        // Set the relative time/frame
        if ( bFrameBased )
        {
            m_lRelative = m_lFrames;
        }
        else
        {
            m_lRelative = CClientTime::GetTime ();
        }

        // We're now playing
        m_bPlaying = true;
        m_bFrameBased = bFrameBased;
    }
}


void CClientPacketRecorder::StartRecord ( const char* szOutput, bool bFrameBased )
{
    // Delete the filename
    if ( m_szFilename )
    {
        delete m_szFilename;
        m_szFilename = NULL;
    }

    // Did we get a filename?
    if ( szOutput )
    {
        // Create a new filename
        m_szFilename = new char [ strlen ( szOutput ) + 1 ];
        strcpy ( m_szFilename, szOutput );

        // Empty the file
        FILE* pFile = fopen ( szOutput, "w+" );
        if ( pFile )
        {
            fclose ( pFile );
        }

        // Set the relative time/frame
        if ( bFrameBased )
        {
            m_lRelative = m_lFrames;
        }
        else
        {
            m_lRelative = CClientTime::GetTime ();
        }

        // We're now recording
        m_bRecording = true;
        m_bFrameBased = bFrameBased;
    }
}


void CClientPacketRecorder::Stop ( void )
{
    // Delete the filename
    if ( m_szFilename )
    {
        delete m_szFilename;
        m_szFilename = NULL;
    }

    // Reset
    m_lRelative = 0;
    m_bPlaying = false;
    m_bRecording = false;
    m_bFrameBased = false;
    m_ulCurrentOffset = 0;
    m_lNextPacketTime = 0;
}


bool CClientPacketRecorder::IsPlaying ( void )
{
    return m_bPlaying;
}


bool CClientPacketRecorder::IsRecording ( void )
{
    return m_bRecording;
}


bool CClientPacketRecorder::IsPlayingOrRecording ( void )
{
    return m_bPlaying || m_bRecording;
}


bool CClientPacketRecorder::IsFrameBased ( void )
{
    return m_bFrameBased;
}


void CClientPacketRecorder::SetFrameSkip ( unsigned int uiFrameSkip )
{
    m_uiFrameSkip = uiFrameSkip;
}


void CClientPacketRecorder::RecordPacket ( unsigned char ucPacketID, NetBitStreamInterface& bitStream )
{
    if ( m_bRecording && m_szFilename )
    {
        // Open our file
        FILE* pFile = fopen ( m_szFilename, "ab+" );
        if ( pFile )
        {
            // Write our timestamp
            if ( m_bFrameBased )
            {
                long lFrameStamp = m_lFrames - m_lRelative;
                fwrite ( &lFrameStamp, sizeof ( long ), 1, pFile );
            }
            else
            {
                long lTimeStamp = CClientTime::GetTime () - m_lRelative;
                fwrite ( &lTimeStamp, sizeof ( long ), 1, pFile );
            }
//          fwrite ( &ulTimeStamp, sizeof ( unsigned long), 1, pFile );

            // Write the packet ID
            fputc ( ucPacketID, pFile );

            // Write the size of the bitstream
            unsigned long ulSize = static_cast < unsigned long > ( bitStream.GetNumberOfBytesUsed () );
            fwrite ( &ulSize, sizeof ( unsigned long ), 1, pFile );

            // Write the content of the bitstream to the file
            char c = 0;
            for ( unsigned long i = 0; i < ulSize; i++ )
            {
                bitStream.Read ( c );
                fputc ( c, pFile );
            }

            // Reset the readpointer so the client can use it
            bitStream.ResetReadPointer ();

            // Close the file
            fclose ( pFile );
        }
    }
}


void CClientPacketRecorder::RecordLocalData ( CClientPlayer* pLocalPlayer )
{
    if ( m_bRecording && m_szFilename )
    {
        // Open our file
        FILE* pFile = fopen ( m_szFilename, "ab+" );
        if ( pFile )
        {
            // Write our timestamp
            if ( m_bFrameBased )
            {
                long lFrameStamp = m_lFrames - m_lRelative;
                fwrite ( &lFrameStamp, sizeof ( long ), 1, pFile );
            }
            else
            {
                long lTimeStamp = CClientTime::GetTime () - m_lRelative;
                fwrite ( &lTimeStamp, sizeof ( long ), 1, pFile );
            }
//          fwrite ( &ulTimeStamp, sizeof ( unsigned long ), 1, pFile );

            // Write the packet ID
            fputc ( 0xFE, pFile );

            // Grab the vehicle
            CClientVehicle* pVehicle = pLocalPlayer->GetOccupiedVehicle ();
            if ( pVehicle )
            {
                // Read out the matrix, movespeed and turnspeed
                CMatrix matVehicle;
                CVector vecMoveSpeed;
                CVector vecTurnSpeed;
                pVehicle->GetMatrix ( matVehicle );
                pVehicle->GetMoveSpeed ( vecMoveSpeed );
                pVehicle->GetTurnSpeed ( vecTurnSpeed );
                float fHealth = pVehicle->GetHealth ();

                // Write it
                fwrite ( &matVehicle.vRight.fX, sizeof ( float ), 1, pFile );
                fwrite ( &matVehicle.vRight.fY, sizeof ( float ), 1, pFile );
                fwrite ( &matVehicle.vRight.fZ, sizeof ( float ), 1, pFile );

                fwrite ( &matVehicle.vFront.fX, sizeof ( float ), 1, pFile );
                fwrite ( &matVehicle.vFront.fY, sizeof ( float ), 1, pFile );
                fwrite ( &matVehicle.vFront.fZ, sizeof ( float ), 1, pFile );

                fwrite ( &matVehicle.vUp.fX, sizeof ( float ), 1, pFile );
                fwrite ( &matVehicle.vUp.fY, sizeof ( float ), 1, pFile );
                fwrite ( &matVehicle.vUp.fZ, sizeof ( float ), 1, pFile );

                fwrite ( &matVehicle.vPos.fX, sizeof ( float ), 1, pFile );
                fwrite ( &matVehicle.vPos.fY, sizeof ( float ), 1, pFile );
                fwrite ( &matVehicle.vPos.fZ, sizeof ( float ), 1, pFile );

                fwrite ( &vecMoveSpeed.fX, sizeof ( float ), 1, pFile );
                fwrite ( &vecMoveSpeed.fY, sizeof ( float ), 1, pFile );
                fwrite ( &vecMoveSpeed.fZ, sizeof ( float ), 1, pFile );

                fwrite ( &vecTurnSpeed.fX, sizeof ( float ), 1, pFile );
                fwrite ( &vecTurnSpeed.fY, sizeof ( float ), 1, pFile );
                fwrite ( &vecTurnSpeed.fZ, sizeof ( float ), 1, pFile );
                fwrite ( &fHealth, sizeof ( float ), 1, pFile );

                // Write the controller state
                CControllerState cs;
                g_pGame->GetPad ()->GetCurrentControllerState ( &cs );
                fwrite ( &cs, sizeof ( CControllerState ), 1, pFile );
            }

            // Close the file
            fclose ( pFile );
        }
    }
}


void CClientPacketRecorder::ReadLocalData ( FILE* pFile )
{
    // Grab the local player
    CClientPlayer* pPlayer = m_pManager->GetPlayerManager ()->GetLocalPlayer ();
    if ( pPlayer )
    {
        // Grab the vehicle
        CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            // Temp vars
            CMatrix matVehicle;
            CVector vecMoveSpeed;
            CVector vecTurnSpeed;
            float fHealth;

            // Read it out
            fread ( &matVehicle.vRight.fX, sizeof ( float ), 1, pFile );
            fread ( &matVehicle.vRight.fY, sizeof ( float ), 1, pFile );
            fread ( &matVehicle.vRight.fZ, sizeof ( float ), 1, pFile );

            fread ( &matVehicle.vFront.fX, sizeof ( float ), 1, pFile );
            fread ( &matVehicle.vFront.fY, sizeof ( float ), 1, pFile );
            fread ( &matVehicle.vFront.fZ, sizeof ( float ), 1, pFile );

            fread ( &matVehicle.vUp.fX, sizeof ( float ), 1, pFile );
            fread ( &matVehicle.vUp.fY, sizeof ( float ), 1, pFile );
            fread ( &matVehicle.vUp.fZ, sizeof ( float ), 1, pFile );

            fread ( &matVehicle.vPos.fX, sizeof ( float ), 1, pFile );
            fread ( &matVehicle.vPos.fY, sizeof ( float ), 1, pFile );
            fread ( &matVehicle.vPos.fZ, sizeof ( float ), 1, pFile );

            fread ( &vecMoveSpeed.fX, sizeof ( float ), 1, pFile );
            fread ( &vecMoveSpeed.fY, sizeof ( float ), 1, pFile );
            fread ( &vecMoveSpeed.fZ, sizeof ( float ), 1, pFile );

            fread ( &vecTurnSpeed.fX, sizeof ( float ), 1, pFile );
            fread ( &vecTurnSpeed.fY, sizeof ( float ), 1, pFile );
            fread ( &vecTurnSpeed.fZ, sizeof ( float ), 1, pFile );
            fread ( &fHealth, sizeof ( float ), 1, pFile );

            CControllerState cs;
            fread ( &cs, sizeof ( CControllerState ), 1, pFile );

            // Set the data
            pVehicle->SetMatrix ( matVehicle );
            pVehicle->SetMoveSpeed ( vecMoveSpeed );
            pVehicle->SetTurnSpeed ( vecTurnSpeed );
            pVehicle->SetHealth ( fHealth );

            // Set the controller state
            CControllerState csLast;
            g_pGame->GetPad ()->Disable ( true );
            g_pGame->GetPad ()->GetCurrentControllerState ( &csLast );
            g_pGame->GetPad ()->SetLastControllerState ( &csLast );
            g_pGame->GetPad ()->SetCurrentControllerState ( &cs );
        }
    }
}

bool bHack = false;
void CClientPacketRecorder::DoPulse ( void )
{
    // Count the frame
    m_lFrames += m_uiFrameSkip;

    // Are we playing?
    if ( m_bPlaying && m_pfnPacketHandler && m_szFilename )
    {
        // Time to play the next packet?
        long lCurTime = (long)CClientTime::GetTime () - m_lRelative;
        //g_pCore->GetConsole()->Printf("current time:%u\n",lCurTime);
        if(m_bFrameBased) __asm int 3;
        while ( ( m_bFrameBased && ( m_lFrames - m_lRelative >= m_lNextPacketTime ) ) || ( !m_bFrameBased && ( lCurTime >= m_lNextPacketTime ) ) )
        {
            // Load the file
            FILE* pFile = fopen ( m_szFilename, "rb" );
            if ( pFile )
            {
                // Seek to our current offset + the bytes occupied by the time?
                fseek ( pFile, m_ulCurrentOffset, SEEK_SET );

                unsigned long ulTimeStamp;
                fread ( &ulTimeStamp, sizeof ( unsigned long ), 1, pFile );

                // Reached end of file?
                if ( feof ( pFile ) )
                {
                    fclose ( pFile );
                    Stop ();
                    return;
                }

                // Read out the packet id
                unsigned char ucPacketID = fgetc ( pFile );

                // Is it 0xFE (local player data) or 0xFF (local keys)?
                if ( ucPacketID == 0xFE )
                {
                    ReadLocalData ( pFile );
                }
                else
                {
                    // Raise a breakpoint?
                    if ( ucPacketID == 0xFD )
                    {
                        _asm int 3;
                        Stop ();
                        return;
                    }

                    // Read out number of bytes in the packet
                    unsigned long ulSize = 0;
                    fread ( &ulSize, sizeof ( unsigned long ), 1, pFile );

                    // Create a bitstream
                    NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
                    if ( pBitStream )
                    {
                        // Write the filedata to the bitstream
                        for ( unsigned long i = 0; i < ulSize; i++ )
                        {
                            char c = fgetc ( pFile );
                            pBitStream->Write ( c );
                        }

                        // Send it to the packethandler
                        //g_pCore->GetConsole()->Printf("(time: %u, current: %u) %u\n",ulTimeStamp,lCurTime,ucPacketID);
                        m_pfnPacketHandler ( ucPacketID, *pBitStream );

                        // Destroy the bitstream
                        g_pNet->DeallocateNetBitStream ( pBitStream );
                    }
                }

                // Remember the new offset and read out the time for the next packet
                m_ulCurrentOffset = ftell ( pFile );
                fread ( &m_lNextPacketTime, sizeof ( long ), 1, pFile );
                m_ucNextPacketID = fgetc ( pFile );
                //g_pCore->GetConsole()->Printf("next time: %u\n",m_lNextPacketTime);

                // Reached end of file?
                int iTemp = 0;
                fread ( &iTemp, sizeof ( int ), 1, pFile );
                if ( feof ( pFile ) )
                {
                    fclose ( pFile );
                    Stop ();
                    return;
                }

                // Close the file
                fclose ( pFile );
            }
        }
    }
}
