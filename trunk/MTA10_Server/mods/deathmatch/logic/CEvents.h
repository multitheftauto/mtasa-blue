/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CEvents.h
*  PURPOSE:     Event manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CEVENTS_H
#define __CEVENTS_H

#include <string>
#include <list>
#include <vector>

using namespace std;

struct SEvent
{
    class CLuaMain*     pLuaMain;
    std::string         strName;
    std::string         strArguments;
    bool                bAllowRemoteTrigger;
};

class CEvents
{
public:
                                CEvents             ( void );
    inline                      ~CEvents            ( void )                { RemoveAllEvents (); };

    bool                        AddEvent            ( const char* szName, const char* szArguments, class CLuaMain* pLuaMain, bool bAllowRemoteTrigger );
    void                        RemoveEvent         ( SEvent* pEvent );
    void                        RemoveEvent         ( const char* szName );
    void                        RemoveAllEvents     ( class CLuaMain* pMain );

    inline bool                 Exists              ( const char* szName )  { return Get ( szName ) != NULL; };
    SEvent*                     Get                 ( const char* szName );

    list < SEvent* > ::const_iterator   IterBegin   ( void )                { return m_Events.begin (); };
    list < SEvent* > ::const_iterator   IterEnd     ( void )                { return m_Events.end (); };

    void                        PreEventPulse       ( void );
    void                        PostEventPulse      ( void );

    void                        CancelEvent         ( bool bCancelled = true );
	void                        CancelEvent         ( bool bCancelled, char* szReason );
    bool                        WasEventCancelled   ( void );
	const char*					GetLastError		( void );

private:
    void                        RemoveAllEvents     ( void );

    list < SEvent* >            m_Events;

    vector < int >              m_CancelledList;
    bool                        m_bEventCancelled;
    bool                        m_bWasEventCancelled;

	char*						m_szLastError;
};

#endif
