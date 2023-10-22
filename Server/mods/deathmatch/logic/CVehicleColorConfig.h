/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleColorConfig.h
 *  PURPOSE:     Vehicle colors loader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CVehicleColorConfig
{
public:
    bool Load(const char* szFilename);
    bool Generate(const char* szFilename);
    void Reset();
};
