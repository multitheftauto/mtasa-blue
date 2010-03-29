/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFireSA.h
*  PURPOSE:     Header file for fire class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_FIRE
#define __CGAMESA_FIRE

#include <game/CFire.h>
#include "Common.h"
#include "CEntitySA.h"

#define FUNC_Extinguish                 0x5393F0        //  ##SA##
#define FUNC_CreateFxSysForStrength     0x539360        //  ##SA##

class FxSystem_c; // we don't actually define this anywhere

class CFireSAInterface : public CFireInterface
{
public:
    BYTE                    bActive:1;
    BYTE                    bCreatedByScript:1;
    BYTE                    bMakesNoise:1;
    BYTE                    bBeingExtinguished : 1;
    BYTE                    bFirstGeneration : 1;
    WORD                    ScriptReferenceIndex;
    CVector                 vecPosition;
    CEntitySAInterface      * entityTarget;
    CEntitySAInterface      * entityCreator;
    DWORD                   nTimeToBurn;
    FLOAT                   Strength;
    signed char             nNumGenerationsAllowed;
    BYTE                    RemovalDist;

    FxSystem_c              * m_fxSysPtr;
};

class CFireSA : public CFire
{
private:
    CFireSAInterface        * internalInterface;
public:
    // constructor
    CFireSA(CFireSAInterface * fireInterface) { this->internalInterface = fireInterface; }

    VOID                    Extinguish (  );
    CVector                 * GetPosition ( );
    VOID                    SetPosition ( CVector & vecPosition );
    VOID                    SetTimeToBurnOut ( DWORD dwTime );
    DWORD                   GetTimeToBurnOut (  );
    CEntity                 * GetCreator (  );
    CEntity                 * GetEntityOnFire (  );
    VOID                    SetTarget ( CEntity * entity );
    BOOL                    IsIgnited (  );
    BOOL                    IsFree (  );
    VOID                    SetSilent ( BOOL bSilent );
    BOOL                    IsBeingExtinguished ();
    VOID                    Ignite( );
    FLOAT                   GetStrength (  );
    VOID                    SetStrength ( FLOAT fStrength );
    VOID                    SetNumGenerationsAllowed ( char generations );
    inline CFireInterface*  GetInterface ( ) { return this->internalInterface; }
};

#endif
