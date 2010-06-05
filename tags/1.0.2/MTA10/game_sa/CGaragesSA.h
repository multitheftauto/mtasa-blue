/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CGaragesSA.h
*  PURPOSE:		Header file for garage manager class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_GARAGES
#define __CGAMESA_GARAGES

#include <game/CGarages.h>
#include "CGarageSA.h"

class CGaragesSAInterface
{
public:
    CGarageSAInterface      Garages [ MAX_GARAGES ];
};

class CGaragesSA : public CGarages
{
public:
                                CGaragesSA      ( CGaragesSAInterface * pInterface );
    virtual                    ~CGaragesSA      ( );

    CGarageSA*                  GetGarage       ( DWORD dwID );

    void                        Initialize      ( );

private:
    CGaragesSAInterface *       internalInterface;
    CGarageSA*                  Garages[MAX_GARAGES];
};

#endif