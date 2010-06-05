/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CRopes.h
*  PURPOSE:		Rope entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRopes_H
#define __CRopes_H

typedef unsigned long DWORD;
class CVector;

class CRopes
{
public:
    virtual int     CreateRopeForSwatPed        ( const CVector & vecPosition ) = 0;
};

#endif