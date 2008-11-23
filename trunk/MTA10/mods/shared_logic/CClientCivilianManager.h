/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCivilianManager.h
*  PURPOSE:     Civilian ped manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

class CClientCivilianManager;

#ifndef __CCLIENTCIVILIANMANAGER_H
#define __CCLIENTCIVILIANMANAGER_H

#include "CClientCivilian.h"

#ifdef COMPILE_FOR_VC
    #include <game/CCivilianPed.h>
#else if COMPILE_FOR_SA
    #include <game/CCivilianPed.h>
#endif

#include <list>
using namespace std;

class CClientCivilianManager
{
    friend class CClientManager;
    friend class CClientCivilian;

public:
    CClientCivilian*                Create                  ( int iPedModel, ElementID ID );
	CClientCivilian*			    Create                  ( CCivilianPed* pPed, ElementID ID );
    void                            DeleteAll               ( void );

    inline unsigned int             Count                   ( void )                            { return static_cast < unsigned int > ( m_Civilians.size () ); };
    static CClientCivilian*         Get                     ( ElementID ID );
    static inline CClientCivilian*  Get                     ( CCivilianPed* pPed )              { return reinterpret_cast < CClientCivilian* > ( pPed->GetStoredPointer () ); };

    static bool                     IsValidModel            ( unsigned int ulCivilianModel );

    list < CClientCivilian* > ::iterator             IterGet             ( CClientCivilian* pCivilian );
    list < CClientCivilian* > ::reverse_iterator     IterGetReverse      ( CClientCivilian* pCivilian );
    inline list < CClientCivilian* > ::iterator      IterBegin           ( void )           { return m_Civilians.begin (); };
    list < CClientCivilian* > ::iterator             IterEnd             ( void )           { return m_Civilians.end (); };
    list < CClientCivilian* > ::reverse_iterator     IterReverseBegin    ( void )           { return m_Civilians.rbegin (); };
    list < CClientCivilian* > ::reverse_iterator     IterReverseEnd      ( void )           { return m_Civilians.rend (); };

private:
                                    CClientCivilianManager   ( class CClientManager* pManager );
                                    ~CClientCivilianManager  ( void );

    inline void                     AddToList               ( CClientCivilian* pCivilian )      { m_Civilians.push_back ( pCivilian ); };
    void                            RemoveFromList          ( CClientCivilian* pCivilian );

    class CClientManager*           m_pManager;
    bool                            m_bCanRemoveFromList;
    list < CClientCivilian* >       m_Civilians;
};

#endif
