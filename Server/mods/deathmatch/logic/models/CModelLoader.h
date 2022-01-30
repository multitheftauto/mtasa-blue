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
    void unloadDefaultData();

    void loadVehicleHandling(std::string_view strData);
    void loadObjectDefs(const std::filesystem::path path);
    void loadVehicleDefs(const std::filesystem::path path);

private:
    void parseConfigCSV(const std::filesystem::path path, std::function<void(std::vector<char*> lineParams)> parseLineFun);
};
