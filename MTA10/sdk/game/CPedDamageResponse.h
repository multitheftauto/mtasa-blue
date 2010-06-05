/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CPedDamageResponse.h
*  PURPOSE:		ped damage response interface
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PEDDAMAGERESPONSE
#define __CGAME_PEDDAMAGERESPONSE

class CPedDamageResponseSAInterface;

class CPedDamageResponse
{
public:
    virtual CPedDamageResponseSAInterface *     GetInterface                ( void ) = 0;
    virtual void                                Calculate                   ( CEntity * pEntity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool b_1, bool bSpeak ) = 0;
};

#endif