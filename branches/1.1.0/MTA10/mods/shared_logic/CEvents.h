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
                                ~CEvents            ( void );

    bool                        AddEvent            ( const char* szName, const char* szArguments, class CLuaMain* pLuaMain, bool bAllowRemoteTrigger );
    void                        RemoveEvent         ( SEvent* pEvent );
    void                        RemoveEvent         ( const char* szName );
    void                        RemoveAllEvents     ( class CLuaMain* pMain );

    inline bool                 Exists              ( const char* szName )  { return Get ( szName ) != NULL; };
    SEvent*                     Get                 ( const char* szName );

    std::list < SEvent* > ::const_iterator
                                IterBegin           ( void )                { return m_Events.begin (); };
    std::list < SEvent* > ::const_iterator
                                IterEnd             ( void )                { return m_Events.end (); };

    void                        PreEventPulse       ( void );
    void                        PostEventPulse      ( void );

    void                        CancelEvent         ( bool bCancelled = true );
    bool                        WasEventCancelled   ( void );

private:
    void                            RemoveAllEvents     ( void );
    std::list < SEvent* >           m_Events;
    std::vector < int >             m_CancelledList;
    bool                            m_bEventCancelled;
    bool                            m_bWasEventCancelled;
};

#endif

