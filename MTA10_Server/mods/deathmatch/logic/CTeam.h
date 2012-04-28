/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTeam.h
*  PURPOSE:     Team element class
*  DEVELOPERS:  Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CTeam;

#ifndef __CTEAM_H
#define __CTEAM_H

#include "CElement.h"
#include "CPlayer.h"
#include "CTeamManager.h"

#define MAX_TEAM_NAME 128

class CTeam: public CElement
{
    friend class CTeamManager;
public:    
                            CTeam                   ( CTeamManager* pTeamManager, CElement* pParent, CXMLNode* pNode, const char* szName = NULL, unsigned char ucRed = 0, unsigned char ucGreen = 0, unsigned char ucBlue = 0 );
                            ~CTeam                  ( void );

    void                    Unlink                  ( void );
    bool                    ReadSpecialData         ( void );

    const char*             GetTeamName             ( void )                    { return m_strTeamName; }
    void                    SetTeamName             ( const char* szName );

    void                    AddPlayer               ( CPlayer* pPlayer, bool bChangePlayer = false );
    void                    RemovePlayer            ( CPlayer* pPlayer, bool bChangePlayer = false );
    void                    RemoveAllPlayers        ( void );
    void                    GetPlayers              ( lua_State* luaVM );

    void                    GetColor                ( unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue );
    void                    SetColor                ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );

    inline bool             GetFriendlyFire         ( void )                    { return m_bFriendlyFire; }
    inline void             SetFriendlyFire         ( bool bFriendlyFire )      { m_bFriendlyFire = bFriendlyFire; }

    inline unsigned int     CountPlayers            ( void )                    { return static_cast < unsigned int > ( m_Players.size () ); }

    std::list < CPlayer* > ::const_iterator PlayersBegin ( void )               { return m_Players.begin (); }
    std::list < CPlayer* > ::const_iterator PlayersEnd   ( void )               { return m_Players.end (); }

private:                        
    CTeamManager*           m_pTeamManager;

    SString                 m_strTeamName;
    std::list < CPlayer* >  m_Players;

    unsigned char           m_ucRed;
    unsigned char           m_ucGreen;
    unsigned char           m_ucBlue;

    bool                    m_bFriendlyFire;
};

#endif
