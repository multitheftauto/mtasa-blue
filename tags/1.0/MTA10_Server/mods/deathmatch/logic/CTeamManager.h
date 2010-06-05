/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTeamManager.h
*  PURPOSE:     Team element manager class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CTeamManager;

#ifndef __CTEAMMANAGER_H
#define __CTEAMMANAGER_H

#include <list>
#include "CTeam.h"

class CTeamManager
{
public:
                            CTeamManager        ( void );
                            ~CTeamManager       ( void );
    
    inline void             AddToList           ( CTeam* pTeam )    { m_List.push_back ( pTeam ); }
    void                    RemoveFromList      ( CTeam* pTeam );
    void                    RemoveAll           ( void );

    CTeam*                  GetTeam             ( const char* szName );
    bool                    Exists              ( CTeam* pTeam );
    CTeam*                  Create              ( CElement* pParent, CXMLNode* pNode, char* szName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );
    CTeam*                  CreateFromXML       ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents );

    std::list < CTeam* > ::const_iterator IterBegin  ( void )            { return m_List.begin (); }
    std::list < CTeam* > ::const_iterator IterEnd    ( void )            { return m_List.end (); }

private:
    std::list < CTeam* >    m_List;
    bool                    m_bRemoveFromList;
};

#endif
