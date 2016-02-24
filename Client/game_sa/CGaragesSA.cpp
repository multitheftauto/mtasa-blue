/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CGaragesSA.cpp
*  PURPOSE:     Garage manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CGaragesSA::CGaragesSA ( CGaragesSAInterface* pInterface )
{
    this->internalInterface = pInterface;

    for ( int i = 0 ; i < MAX_GARAGES ; i++ )
    {
        this->Garages[i] = new CGarageSA( (CGarageSAInterface *)(CLASS_CGarages + 216 * i) );
    }
}

CGaragesSA::~CGaragesSA ( )
{
    for ( int i = 0 ; i < MAX_GARAGES ; i++ )
    {
        delete this->Garages[i];
        this->Garages[i] = NULL;
    }
}

void CGaragesSA::Initialize ( )
{
    // Disable pay and sprays and mod shops
    static const int iPayAndSprays [] = { 7, 8, 10, 11, 12, 15, 18, 19, 24, 27, 32, 33, 36, 40, 41, 47 };
    for ( unsigned int i = 0; i < sizeof ( iPayAndSprays ) / sizeof ( int ); ++i )
    {
        this->Garages [ iPayAndSprays [ i ] ]->SetType ( 1 );
    }
}

CGarageSA* CGaragesSA::GetGarage ( DWORD dwID )
{
    if ( dwID >= 0 && dwID < MAX_GARAGES )
    {
        return this->Garages[dwID];
    }
    return NULL;
}
