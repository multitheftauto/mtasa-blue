/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CEvents.h
*  PURPOSE:     Events class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
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

struct SExtinctEvent
{
    char * szName;
    char * szNewName;
};

class CEvents
{
public:
                                CEvents             ( void );
                                ~CEvents            ( void );

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
    bool                        WasEventCancelled   ( void );

    void                        AddExtinctEvent     ( const char * szName, const char * szNewName );
    bool                        IsExtinctEvent      ( const char * szName, char * szNewName, unsigned int uiLength );

private:
    void                        RemoveAllEvents     ( void );
    list < SEvent* >            m_Events;
    vector < int >              m_CancelledList;
    bool                        m_bEventCancelled;
    bool                        m_bWasEventCancelled;
    list < SExtinctEvent * >    m_ExtinctEvents;
};

#endif

