/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleColorConfig.h
 *  PURPOSE:     Vehicle colors loader
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
