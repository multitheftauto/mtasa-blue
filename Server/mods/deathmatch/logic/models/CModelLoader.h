/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelLoader.h
 *  PURPOSE:     Model info loader class header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <filesystem>

class CModelLoader
{
public:
    CModelLoader();
    ~CModelLoader();

    void loadDefaultData();

private:
    void loadVehicleHandling(const char* strPath);
    void loadObjectDefs(const char* strPath);
    void loadVehicleDefs(const char* strPath);
    void loadVehicleHandling(const char* strPath);

};
