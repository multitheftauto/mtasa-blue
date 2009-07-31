/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CColModel.h
*  PURPOSE:		Collision model entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_COLMODEL
#define __CGAME_COLMODEL

class CColModelSAInterface;

class CColModel
{
public:
    virtual CColModelSAInterface *	 GetInterface    ( void ) = 0;
    virtual void                     Destroy         ( void ) = 0;
};

#endif