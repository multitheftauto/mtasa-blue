/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CEvent.h
*  PURPOSE:		Game event interface
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_EVENTGROUP
#define __CGAME_EVENTGROUP

class CEventGroup
{
public:
    virtual CEvent *                    Add                 ( CEvent * pEvent, bool b_1 ) = 0;
};

#endif