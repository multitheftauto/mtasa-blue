/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CVehicleNames.h
 *  PURPOSE:     Vehicle names class header
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CVehicleNames
{
public:
    static bool        IsModelTrailer(ushort usModel);
    static const char* GetVehicleName(ushort usModel);
    static uint        GetVehicleModel(const char* szName);
    static const char* GetVehicleTypeName(ushort ulModel);
};
