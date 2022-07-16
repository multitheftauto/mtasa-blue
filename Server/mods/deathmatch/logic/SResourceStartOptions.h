/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/SResourceStartOptions.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

struct SResourceStartOptions
{
    bool bIncludedResources = true;
    bool bConfigs = true;
    bool bMaps = true;
    bool bScripts = true;
    bool bHTML = true;
    bool bClientConfigs = true;
    bool bClientScripts = true;
    bool bClientFiles = true;
};
