/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTeam.cpp
*  PURPOSE:     Team element class
*  DEVELOPERS:  Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTeam::CTeam ( CTeamManager* pTeamManager, CElement* pParent, CXMLNode* pNode, const char* szName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue ): CElement ( pParent, pNode )
{
    m_pTeamManager = pTeamManager;

    m_iType = CElement::TEAM;
    SetTypeName ( "team" );

    SetTeamName ( szName );
    SetColor ( ucRed, ucGreen, ucBlue );
    SetFriendlyFire ( true );

    m_pTeamManager->AddToList ( this );
}


CTeam::~CTeam ( void )
{
    RemoveAllPlayers ();
    Unlink ();
}


void CTeam::Unlink ( void )
{
    m_pTeamManager->RemoveFromList ( this );
}


bool CTeam::ReadSpecialData ( void )
{
    // Grab the "name"
    char szTemp [ MAX_TEAM_NAME ];
    if ( GetCustomDataString ( "name", szTemp, MAX_TEAM_NAME, true ) )
    {
        SetTeamName ( szTemp );
    }
    else
    {
        CLogger::ErrorPrintf ( "Bad/missing name' attribute in <team> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "color" data
    int iTemp;
    if ( GetCustomDataString ( "color", szTemp, MAX_TEAM_NAME, true ) )
    {
        // Convert it to RGBA
        unsigned char ucAlpha;
        if ( !XMLColorToInt ( szTemp, m_ucRed, m_ucGreen, m_ucBlue, ucAlpha ) )
        {
            CLogger::ErrorPrintf ( "Bad 'color' value specified in <team> (line %u)\n", m_uiLine );
            return false;
        }
    }
    else
    {
        if ( GetCustomDataInt ( "colorR", iTemp, true ) )
            m_ucRed = static_cast < unsigned char > ( iTemp );
        else
            m_ucRed = 255;
        if ( GetCustomDataInt ( "colorG", iTemp, true ) )
            m_ucGreen = static_cast < unsigned char > ( iTemp );
        else
            m_ucGreen = 255;
        if ( GetCustomDataInt ( "colorB", iTemp, true ) )
            m_ucBlue = static_cast < unsigned char > ( iTemp );
        else
            m_ucBlue = 255;
    }

    if ( !GetCustomDataBool ( "friendlyfire", m_bFriendlyFire, true ) )
    {
        m_bFriendlyFire = true;
    }

    if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    return true;
}


void CTeam::SetTeamName ( const char* szName )
{
    if ( szName )
        m_strTeamName.AssignLeft( szName, MAX_TEAM_NAME_LENGTH );
    else
        m_strTeamName = "";
}


void CTeam::AddPlayer ( CPlayer* pPlayer, bool bChangePlayer )
{
    m_Players.push_back ( pPlayer );
    if ( bChangePlayer )
        pPlayer->SetTeam ( this, false );
}


void CTeam::RemovePlayer ( CPlayer* pPlayer, bool bChangePlayer )
{
    m_Players.remove ( pPlayer );
    if ( bChangePlayer )
        pPlayer->SetTeam ( NULL, false );
}


void CTeam::RemoveAllPlayers ( void )
{
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); ++iter )
    {
        (*iter)->SetTeam ( NULL, false );
    }
    m_Players.clear ();
}


void CTeam::GetPlayers ( lua_State* luaVM )
{
    unsigned int uiIndex = 0;
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); ++iter )
    {
        if ( !( *iter )->IsBeingDeleted ( ) )
        {
            lua_pushnumber ( luaVM, ++uiIndex );
            lua_pushelement ( luaVM, *iter );
            lua_settable ( luaVM, -3 );
        }
    }
}


void CTeam::GetColor ( unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue )
{
    ucRed = m_ucRed;
    ucBlue = m_ucBlue;
    ucGreen = m_ucGreen;
}


void CTeam::SetColor ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    m_ucRed = ucRed;
    m_ucBlue = ucBlue;
    m_ucGreen = ucGreen;
}

