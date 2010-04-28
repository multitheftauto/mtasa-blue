/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEventGunShotSA.h
*  PURPOSE:     Header file for gunshot event class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CEventGunShotSA_H
#define __CEventGunShotSA_H

#include <game/CEventGunShot.h>

#define FUNC_CEventGunShot_Constructor  0x4ac610
#define FUNC_CEventGunShot_Destructor   0x4ac720

class CEventGunShotSAInterface
{
public:
    char pad [ 100 ];
};

class CEventGunShotSA : public CEventGunShot
{
public:

                                        CEventGunShotSA     ( CEntity * pEntity, CVector & vecOrigin, CVector & vecTarget, bool b_1 );
                                        CEventGunShotSA     ( CEventGunShotSAInterface * pInterface )    { m_pInterface = pInterface; }

    inline CEventGunShotSAInterface *   GetInterface        ( void )        { return m_pInterface; }

    void                                Destroy             ( bool bDestroyInterface );

private:
    CEventGunShotSAInterface *          m_pInterface;
};

#endif
