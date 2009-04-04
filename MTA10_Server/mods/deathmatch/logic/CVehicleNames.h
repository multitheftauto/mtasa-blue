/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVehicleNames.h
*  PURPOSE:     GTA vehicle name definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Peter <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVEHICLENAMES_H
#define __CVEHICLENAMES_H

struct SVehicleName
{
    char szName [32];
};

class CVehicleNames
{
public:
    static bool                     IsValidModel            ( unsigned long ulModel );

    static const char*              GetVehicleName          ( unsigned long ulModel );
    static unsigned int             GetVehicleModel         ( const char* szName );

	static const char*              GetVehicleTypeName      ( unsigned long ulModel );
};

#endif
