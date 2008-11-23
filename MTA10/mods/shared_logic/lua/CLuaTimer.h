/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaTimer.h
*  PURPOSE:     Lua timer class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLuaTimer;

#ifndef __CLUATIMER_H
#define __CLUATIMER_H

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

#define LUA_TIMER_MIN_INTERVAL		50

class CLuaTimer
{
public:

                            CLuaTimer                   ( void );
                            CLuaTimer                   ( int iLuaFunction, const CLuaArguments& Arguments );
							~CLuaTimer					( void );

	inline int				GetLuaFunction				( void )						{ return m_iLuaFunction; };
	inline void				SetLuaFunction				( int iLuaFunction )			{ m_iLuaFunction = iLuaFunction; };

    inline unsigned long    GetStartTime                ( void ) const                  { return m_ulStartTime; };
    inline void             SetStartTime                ( unsigned long ulStartTime )   { m_ulStartTime = ulStartTime; };

	inline unsigned long    GetDelay					( void ) const                  { return m_ulDelay; };
    inline void             SetDelay					( unsigned long ulDelay )		{ m_ulDelay = ulDelay; };

	inline unsigned int		GetRepeats					( void ) const					{ return m_uiRepeats; };
	inline void				SetRepeats					( unsigned int uiRepeats )		{ m_uiRepeats = uiRepeats; }

    void                    ExecuteTimer                ( class CLuaMain* pLuaMain );

private:
    int						m_iLuaFunction;
    CLuaArguments*          m_pArguments;
    unsigned long           m_ulStartTime;
	unsigned long			m_ulDelay;
	unsigned int			m_uiRepeats;
};

#endif
