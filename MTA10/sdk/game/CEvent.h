/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CEvent.h
*  PURPOSE:     Game event interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_EVENT
#define __CGAME_EVENT

class CEventSAInterface;

class CEvent
{
public:
    virtual CEventSAInterface *         GetInterface        ( void ) = 0;
};

#endif
