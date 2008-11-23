/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CGaragesSA.cpp
*  PURPOSE:		Garage manager
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
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
    for ( int i = 0 ; i < MAX_GARAGES ; i++ )
    {
        CGarageSA* pGarage = this->Garages[i];
        // Disable pay and sprays and allow safehouse gargages closing with someone inside
        pGarage->SetType ( 1 );
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