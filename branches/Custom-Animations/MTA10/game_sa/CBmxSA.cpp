/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBmxSA.cpp
*  PURPOSE:     Bmx bike vehicle entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CBmxSA::CBmxSA( CBmxSAInterface * bmx )
{
    DEBUG_TRACE("CBmxSA::CBmxSA( CBmxSAInterface * bmx )");
    this->m_pInterface = bmx;
}

CBmxSA::CBmxSA( eVehicleTypes dwModelID ):CBikeSA( dwModelID )
{
    DEBUG_TRACE("CBmxSA::CBmxSA( eVehicleTypes dwModelID ):CBikeSA( dwModelID )");
}
