/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CVehicleNames.h
*  PURPOSE:     Vehicle names class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Peter <>
*
*****************************************************************************/

#ifndef __CVEHICLENAMES_H
#define __CVEHICLENAMES_H

class CVehicleNames
{
public:
    static bool                     IsValidModel            ( unsigned long ulModel );

    static const char*              GetVehicleName          ( unsigned long ulModel );
    static unsigned int             GetVehicleModel         ( const char* szName );

	static const char*              GetVehicleTypeName      ( unsigned long ulModel );
};

#endif
