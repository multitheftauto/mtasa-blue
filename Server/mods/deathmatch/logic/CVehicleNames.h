/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CVehicleNames.h
 *  PURPOSE:     GTA vehicle name definitions class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CVehicleNames
{
public:
    static const char*  GetVehicleName(unsigned long ulModel);
    static unsigned int GetVehicleModel(const char* szName);
    static const char* GetVehicleTypeName(unsigned long ulModel);
};
