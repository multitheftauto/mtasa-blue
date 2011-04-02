/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBuildingSA.h
*  PURPOSE:     Header file for building physical entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_BUILDING
#define __CGAMESA_BUILDING

#include <game/CBuilding.h>
#include "CEntitySA.h"

#define FUNC_CBuilding_operator_new     0x404090
#define FUNC_CBuilding_Constructor      0x403E00

#define SIZEOF_CBuilding                56

class CBuildingSAInterface : public CEntitySAInterface
{

};

class CBuildingSA : public virtual CBuilding, public virtual CEntitySA
{
private:
    CBuildingSAInterface        * internalInterface;
public:
    CBuildingSA( CBuildingSAInterface * objectInterface );
    CBuildingSA( DWORD dwModel );
    ~CBuildingSA( );
};

#endif
