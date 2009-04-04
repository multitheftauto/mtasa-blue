/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRopesSA.h
*  PURPOSE:     Header file for rope entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRopesSA_H
#define __CRopesSA_H

#include <game/CRopes.h>

#define FUNC_CRopes_CreateRopeForSwatPed    0x558d10

class CRopesSA : public CRopes
{
public:
    int     CreateRopeForSwatPed        ( const CVector & vecPosition );
};

#endif