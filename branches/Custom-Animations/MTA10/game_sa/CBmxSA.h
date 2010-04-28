/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBmxSA.h
*  PURPOSE:     Header file for bmx bike vehicle entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_BMX
#define __CGAMESA_BMX

#include <game/CBmx.h>
#include "CBikeSA.h"

class CBmxSAInterface : public CBikeSAInterface
{
    // fill this
};

class CBmxSA : public virtual CBmx, public virtual CBikeSA
{
private:
public:
                                CBmxSA( CBmxSAInterface * bmx );
                                CBmxSA( eVehicleTypes dwModelID );

    virtual                     ~CBmxSA ( void ) {};
};

#endif
