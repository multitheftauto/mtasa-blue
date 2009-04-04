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
#include "Platform.h"
#include <cstdio>

using namespace std;

CModManagerImpl::CModManagerImpl ( CServerImpl* pServer )
{
    // Init
    m_pServer = pServer;
    m_pBase = NULL;
    m_szModPath [0] = 0;
    m_szModPath [MAX_PATH - 1] = 0;
    m_szServerPath [0] = 0;
    m_szServerPath [MAX_PATH - 1] = 0;
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


const char* CModManagerImpl::GetAbsolutePath ( const char* szRelative, char* szBuffer, unsigned int uiBufferSize )
{
    szBuffer [uiBufferSize - 1] = 0;
	_snprintf ( szBuffer, uiBufferSize - 1, "%s/%s", m_szModPath, szRelative );
    return szBuffer;
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
    if ( m_szServerPath [0] == 0 )
        return false;

    // Make the string path to the mod library
    char szFilename [MAX_PATH];
    szFilename [MAX_PATH - 1] = 0;

    #ifdef WIN32
        _snprintf ( m_szModPath, MAX_PATH - 1, "%s/mods/%s", m_szServerPath, szModName );

        #ifdef _DEBUG       // hack to make it use the debug version of the mod if we run the debug core
            _snprintf ( szFilename, MAX_PATH - 1, "%s/%s_d.dll", m_szModPath, szModName );
        #else
            _snprintf ( szFilename, MAX_PATH - 1, "%s/%s.dll", m_szModPath, szModName );
        #endif
    #else

		snprintf ( m_szModPath, MAX_PATH - 1, "%s/mods/%s", m_szServerPath, szModName );
		snprintf ( szFilename, MAX_PATH - 1, "%s/%s.so", m_szModPath, szModName );
    #endif

    // Attempt to load it
    if ( !m_Library.Load ( szFilename ) )
    {
        // Failed
        Print ( "\nERROR: Loading mod (%s) failed!\n", szFilename );
        return false;
    }

    // Grab the initialization procedure
    InitServer* pfnInitServer = (InitServer*) ( m_Library.GetProcedureAddress ( "InitServer" ) );
    if ( !pfnInitServer )
	{
        // Unload the library
        m_Library.Unload ();

        // Report the error
        Print ( "\nERROR: Bad file: %s!\n", szFilename );
		return false;
	}

    // Call the InitServer procedure to get the interface
    m_pBase = pfnInitServer ();
    if ( !m_pBase )
    {
        // Unload the library
        m_Library.Unload ();

        // Report the error
        Print ( "\nERROR: Failed initializing '%s'!\n", szFilename );
		return false;
    }

    // Call the initialization procedure in the interface
    m_pBase->ServerInitialize ( m_pServer );

    // Start the mod up
    if ( !m_pBase->ServerStartup ( iArgumentCount, szArguments ) )
    {
        // Unload the mod again
        Unload ();
        return false;
    }

    // Success
    return true;
}


void CModManagerImpl::Unload ( void )
{
    // Got a mod loaded?
    if ( m_pBase )
    {
        // Call the mod's shutdown procedure
        m_pBase->ServerShutdown ();
        m_pBase = NULL;

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
