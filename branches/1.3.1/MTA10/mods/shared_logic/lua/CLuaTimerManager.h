/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaTimerManager.h
*  PURPOSE:     Lua timer manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLuaTimerManager;

#ifndef __CLUATIMERMANAGER_H
#define __CLUATIMERMANAGER_H

#include "LuaCommon.h"
#include "CLuaTimer.h"
#include <list>

class CLuaTimerManager
{
public:
    inline                      CLuaTimerManager                ( void )                    { m_pPendingDelete = NULL; m_pProcessingTimer = NULL; }
    inline                      ~CLuaTimerManager               ( void )                    { RemoveAllTimers (); };

    void                        DoPulse                         ( CLuaMain* pLuaMain );

    CLuaTimer*                  GetTimerFromScriptID            ( unsigned int uiScriptID );

    CLuaTimer*                  AddTimer                        ( const CLuaFunctionRef& iLuaFunction, CTickCount llTimeDelay, unsigned int uiRepeats, const CLuaArguments& Arguments );
    void                        RemoveTimer                     ( CLuaTimer* pLuaTimer );
    void                        RemoveAllTimers                 ( void );
    unsigned long               GetTimerCount                   ( void ) const              { return m_TimerList.size (); }

    void                        ResetTimer                      ( CLuaTimer* pLuaTimer );

    CFastList < CLuaTimer > ::const_iterator   IterBegin       ( void )                    { return m_TimerList.begin (); }
    CFastList < CLuaTimer > ::const_iterator   IterEnd         ( void )                    { return m_TimerList.end (); }

private:
    CFastList < CLuaTimer >     m_TimerList;
    std::deque < CLuaTimer* >   m_ProcessQueue;
    CLuaTimer*                  m_pPendingDelete;
    CLuaTimer*                  m_pProcessingTimer;
};

#endif
