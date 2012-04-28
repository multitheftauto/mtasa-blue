/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTeam.h
*  PURPOSE:     Team entity class header
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

class CClientTeam;

#ifndef __CCLIENTTEAM_H
#define __CCLIENTTEAM_H

#include "CClientEntity.h"
#include "CClientPlayer.h"
#include "CClientTeamManager.h"

class CClientTeam : public CClientEntity
{
    DECLARE_CLASS( CClientTeam, CClientEntity )
    friend class CClientTeamManager;
public:    
                                CClientTeam         ( CClientManager* pManager, ElementID ID, const char* szName = NULL, unsigned char ucRed = 0, unsigned char ucGreen = 0, unsigned char ucBlue = 0 );
                                ~CClientTeam        ( void );

    eClientEntityType           GetType             ( void ) const              { return CCLIENTTEAM; }

    void                        Unlink              ( void );

    inline unsigned char        GetID               ( void )                    { return m_ucID; }

    const char*                 GetTeamName         ( void )                    { return m_strTeamName; }
    void                        SetTeamName         ( const char* szName );

    void                        GetColor            ( unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue );
    void                        SetColor            ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );

    inline bool                 GetFriendlyFire     ( void )                    { return m_bFriendlyFire; }
    inline void                 SetFriendlyFire     ( bool bFriendlyFire )      { m_bFriendlyFire = bFriendlyFire; }

    void                        AddPlayer           ( CClientPlayer* pPlayer, bool bChangePlayer = false );
    void                        RemovePlayer        ( CClientPlayer* pPlayer, bool bChangePlayer = false );
    void                        RemoveAll           ( void );

    bool                        Exists              ( CClientPlayer* pPlayer );

    std::list < CClientPlayer* > ::const_iterator IterBegin ( void )         { return m_List.begin (); }
    std::list < CClientPlayer* > ::const_iterator IterEnd   ( void )         { return m_List.end (); }

    void                        GetPosition         ( CVector& vecPosition ) const  { vecPosition = m_vecPosition; }
    void                        SetPosition         ( const CVector& vecPosition )  { m_vecPosition = vecPosition; }

    inline unsigned int         CountPlayers        ( void )            { return static_cast < unsigned int > ( m_List.size () ); }

protected:                        
    CClientTeamManager*         m_pTeamManager;

    unsigned char               m_ucID;
    SString                     m_strTeamName;

    unsigned char               m_ucRed;
    unsigned char               m_ucGreen;
    unsigned char               m_ucBlue;

    bool                        m_bFriendlyFire;

    std::list < CClientPlayer* > m_List;  

    CVector                     m_vecPosition;
};

#endif