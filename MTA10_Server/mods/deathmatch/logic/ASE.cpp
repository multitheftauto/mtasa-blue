/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/ASE.cpp
*  PURPOSE:     All-Seeing Eye server query protocol handler class
*  DEVELOPERS:  Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern "C"
{
    #include "ASEQuerySDK.h"
}

ASE* ASE::_instance = NULL;

ASE::ASE ( CMainConfig* pMainConfig, CPlayerManager* pPlayerManager, int iPort, const char* lpszServerIP )
{
    _instance = this;

	#ifdef _WIN32 /* WinSock initialization */
			WSADATA WSAData;
			WSAStartup(MAKEWORD(1,1), &WSAData);
	#endif

    m_pMainConfig = pMainConfig;
    m_pPlayerManager = pPlayerManager;

    memset(m_szMapName, 0, sizeof(m_szMapName));
    memset(m_szGameType, 0, sizeof(m_szGameType));

    SetGameType ( "MTA:SA" );
    SetMapName ( "None" );

    // Make sure it gets 0 if it's empty. ASE shouldn't be changing it but it takes
    // a char* for some reason.
    char* szServerIP = const_cast < char* > ( lpszServerIP );
    if ( szServerIP && szServerIP [0] == 0 )
    {
        szServerIP = NULL;
    }

    if ( ASEQuery_initialize ( iPort, 1, szServerIP ) )
    {
        CLogger::LogPrintf ( "All Seeing Eye listing enabled. Port %d (UDP) must be accessible\n           from the internet\n", iPort+123 );
    }
    else
    {
        CLogger::ErrorPrintf ( "ERROR: All Seeing Eye registration failed\n" );
    }
}


ASE::~ASE ( void )
{
    _instance = NULL;
    ClearRules ();
}


void ASE::DoPulse ( void )
{
	ASEQuery_check ();
}

void ASE::SetGameType ( const char * szGameType )
{
    strncpy ( m_szGameType, szGameType, sizeof(m_szGameType) );
    m_szGameType[sizeof(m_szGameType)-1] = 0;
}

void ASE::SetMapName ( const char * szMapName )
{
    strncpy ( m_szMapName, szMapName, sizeof(m_szMapName) );
    m_szMapName[sizeof(m_szMapName)-1] = 0;
}

char* ASE::GetRuleValue ( char* szKey )
{
    list < CASERule* > ::iterator iter = m_Rules.begin ();
    for ( ; iter != m_Rules.end () ; iter++ )
    {
        if ( strcmp ( (*iter)->GetKey (), szKey ) == 0 )
        {
            return (*iter)->GetValue ();
        }
    }
    return NULL;
}


void ASE::SetRuleValue ( char* szKey, char* szValue )
{
    if ( szKey && szKey [ 0 ] )
    {
        list < CASERule* > ::iterator iter = m_Rules.begin ();
        for ( ; iter != m_Rules.end () ; iter++ )
        {
            CASERule* pRule = *iter;
            if ( strcmp ( (*iter)->GetKey (), szKey ) == 0 )
            {
                if ( szValue && szValue [ 0 ] )
                {
                    (*iter)->SetValue ( szValue );
                }
                else
                {
					// Remove from the list
                    delete pRule;
                    m_Rules.erase ( iter );
                }
				// And return
                return;
            }
        }
        m_Rules.push_back ( new CASERule ( szKey, szValue ) );
    }
}


bool ASE::RemoveRuleValue ( char* szKey )
{
    list < CASERule* > ::iterator iter = m_Rules.begin ();
    for ( ; iter != m_Rules.end () ; iter++ )
    {
        CASERule* pRule = *iter;
        if ( strcmp ( pRule->GetKey (), szKey ) == 0 )
        {
            delete pRule;
            m_Rules.erase ( iter );
            return true;
        }
    }
    return false;
}


void ASE::ClearRules ( void )
{
    list < CASERule* > ::iterator iter = m_Rules.begin ();
    for ( ; iter != m_Rules.end () ; iter++ )
    {
        delete *iter;
    }
    m_Rules.clear ();
}


extern void ASEQuery_wantstatus ( void )
{
    CMainConfig* pMainConfig = ASE::GetInstance ()->GetMainConfig ();
    CPlayerManager* pPlayerManager = ASE::GetInstance ()->GetPlayerManager ();

    if ( pMainConfig && pPlayerManager )
    {
        ASE * ase;
	    int iPassword = 0;
        int iPlayers = 0;
        int iMaxPlayers = 0;
        char strGameVer[256] = { '\0' };
        char szName[256] = { '\0' };
        const char * szMapName = NULL;
        const char * szGameType = NULL;

        if ( ase = ASE::GetInstance() )
        {
            szMapName = ase->GetMapName();
            szGameType = ase->GetGameType();
        }

        if ( szMapName == NULL ) 
        {
            szMapName = "NULL";
        }

        if ( szGameType == NULL )
        {
            szGameType = "NULL";
        }

        if ( pMainConfig->HasPassword () )
        {
            iPassword = 1;
        }
        	
	    sprintf ( strGameVer, MTA_VERSION );
	    _snprintf ( szName, 255, "%s", pMainConfig->GetServerName ().c_str () );
		szName[255] = '\0';

	    iPlayers = static_cast < int > ( pPlayerManager->CountJoined () );

        iMaxPlayers = static_cast < int > ( pMainConfig->GetMaxPlayers () );

	    ASEQuery_status ( szName , szGameType, szMapName, strGameVer, iPassword, iPlayers, iMaxPlayers );
    }
}

extern void ASEQuery_wantrules ( void )
{
	//server rules can be added here
	ASEQuery_addrule ( "ListenServer", "no" );
    
    ASE* ase = ASE::GetInstance ();
    list < CASERule* > ::iterator iter = ase->IterBegin ();
    for ( ; iter != ase->IterEnd () ; iter++ )
    {
        ASEQuery_addrule ( (*iter)->GetKey (), (*iter)->GetValue () );
    }
}

extern void ASEQuery_wantplayers ( void )
{
    CPlayerManager* pPlayerManager = ASE::GetInstance ()->GetPlayerManager ();

    if ( pPlayerManager )
    {
        list < CPlayer* > ::const_iterator iter = pPlayerManager->IterBegin ();

        for ( ; iter != pPlayerManager->IterEnd (); iter++ )
        {
            if ( (*iter)->IsJoined () )
            {
                const char* szName = (*iter)->GetNick ();
                char szPing[256] = { '\0' };
			    sprintf ( szPing, "%u", (*iter)->GetPing () );

                if ( szName )
                {
			        ASEQuery_addplayer ( szName , "", "", "0", szPing, "" );
                }
		    }
	    }
    }
}

