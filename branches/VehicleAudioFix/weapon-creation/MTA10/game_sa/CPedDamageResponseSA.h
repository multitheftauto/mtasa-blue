/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedDamageResponseSA.h
*  PURPOSE:     Header file for ped damage response
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PEDDAMAGERESPONSE
#define __CGAMESA_PEDDAMAGERESPONSE

#include <game/CPedDamageResponse.h>

class CPedDamageResponseSAInterface
{
public:
    float       fDamageHealth;
    float       fDamageArmor;
    bool        bUnk;
    bool        bForceDeath;
    bool        bDamageCalculated;
    bool        bUnk3;
};


class CPedDamageResponseSA : public CPedDamageResponse
{
public:
                                        CPedDamageResponseSA        ( CPedDamageResponseSAInterface * pInterface )  { m_pInterface = pInterface; }

    CPedDamageResponseSAInterface *     GetInterface                ( void )    { return m_pInterface; }

    void                                Calculate                   ( CEntity * pEntity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool b_1, bool bSpeak );

private:
    CPedDamageResponseSAInterface *     m_pInterface;
};

#endif