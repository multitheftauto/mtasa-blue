/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CModManagerImpl.cpp
*  PURPOSE:     Mod manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Oli <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CModManagerImpl.h"
#include "MTAPlatform.h"
#include <cstdio>

using namespace std;

CModManagerImpl::CModManagerImpl ( CServerImpl* pServer )
{
    // Init
    m_pServer = pServer;
    m_pBase = NULL;
}


CModManagerImpl::~CModManagerImpl ( void )
{
    // Make sure the mod is unloaded
    Unload ();
}


bool CModManagerImpl::RequestLoad ( const char* szModName )
{
    // TODO
    return false;
}


SString CModManagerImpl::GetAbsolutePath ( const char* szRelative )
{
    return SString ( "%s/%s", m_strModPath.c_str (), szRelative );
}

bool CModManagerImpl::IsModLoaded ( void )
{
    return m_pBase != NULL;
}


CServerBase* CModManagerImpl::GetCurrentMod ( void )
{
    return m_pBase;
}


bool CModManagerImpl::Load ( const char* szModName, int iArgumentCount, char* szArguments [] )
{
    // Fail if no server path is specified
    if ( m_strServerPath == "" )
        return false;

    // Make the string path to the mod library
    m_strModPath = SString ( "%s/mods/%s", m_strServerPath.c_str (), szModName );

    #if defined( WIN32 ) && defined( _DEBUG )
        SString strFilename ( "%s/%s_d%s", m_strModPath.c_str (), szModName, MTA_LIB_EXTENSION );
    #else
        SString strFilename ( "%s/%s%s", m_strModPath.c_str (), szModName, MTA_LIB_EXTENSION );
    #endif

    // Attempt to load it
    if ( !m_Library.Load ( strFilename ) )
    {
        // Failed
        Print ( "\nERROR: Loading mod (%s) failed!\n", strFilename.c_str () );
        return false;
    }

    // Grab the initialization procedure
    InitServer* pfnInitServer = (InitServer*) ( m_Library.GetProcedureAddress ( "InitServer" ) );
    if ( !pfnInitServer )
    {
        // Unload the library
        m_Library.Unload ();

        // Report the error
        Print ( "\nERROR: Bad file: %s!\n", strFilename.c_str () );
        return false;
    }

    // Call the InitServer procedure to get the interface
    m_pBase = pfnInitServer ();
    if ( !m_pBase )
    {
        // Unload the library
        m_Library.Unload ();

        // Report the error
        Print ( "\nERROR: Failed initializing '%s'!\n", strFilename.c_str () );
        return false;
    }

    // Call the initialization procedure in the interface
    m_pBase->ServerInitialize ( m_pServer );

    // Start the mod up
    if ( !m_pBase->ServerStartup ( iArgumentCount, szArguments ) )
    {
        // Unload the mod again
        Unload ( true );
        return false;
    }

    // Success
    return true;
}


void CModManagerImpl::Unload ( bool bKeyPressBeforeTerm )
{
    // Got a mod loaded?
    if ( m_pBase )
    {
        // Call the mod's shutdown procedure
        m_pBase->ServerShutdown ();
        m_pBase = NULL;

#ifdef WIN32
        // Exit crash test
        if ( m_pServer->HasConsole() )
        {
            if ( bKeyPressBeforeTerm )
            {
                Print ( "Press Q to shut down the server!\n" );
                WaitForKey ( 'q' );
            }
            TerminateProcess( GetCurrentProcess(), 0 );
        }
#endif
        // Unload the library
        m_Library.Unload ();
    }
}


void CModManagerImpl::DoPulse ( void )
{
    // Got a mod loaded?
    if ( m_pBase )
    {
        // Pulse the mod
        m_pBase->DoPulse ();
    }
}


bool CModManagerImpl::IsFinished ( void )
{
    if ( m_pBase )
    {
        return m_pBase->IsFinished ();
    }

    return true;
}

bool CModManagerImpl::PendingWorkToDo ( int& iSleepMs )
{
    if ( m_pBase )
    {
        return m_pBase->PendingWorkToDo ( iSleepMs );
    }

    return false;
}


void CModManagerImpl::GetTag ( char *szInfoTag, int iInfoTag )
{
    if ( m_pBase )
    {
        m_pBase->GetTag ( szInfoTag, iInfoTag );
    }
}


void CModManagerImpl::HandleInput ( const char* szCommand )
{
    if ( m_pBase )
    {
        m_pBase->HandleInput ( (char*)szCommand );
    }
}
