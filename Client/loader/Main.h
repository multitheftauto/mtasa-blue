/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/Main.h
 *  PURPOSE:     MTA loader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define GTA_EXE_NAME            "gta_sa.exe"
#define PROXY_GTA_EXE_NAME      "proxy_sa.exe"
#define STEAM_GTA_EXE_NAME      "gta-sa.exe"
#define MTA_GTA_KNOWN_FILE_NAME "models\\gta3.img"
#define MTA_GUID                "Global\\{4962AF5F-5D82-412D-9CCA-AB8BB9DBD353}"
#define URI_CONNECT             1
#define MTA_EXE_NAME_RELEASE    "Multi Theft Auto.exe"
#define LOADER_PROXY_DLL_NAME   "mtasa.dll"

#ifdef MTA_DEBUG
    #define MTA_EXE_NAME            "Multi Theft Auto_d.exe"
    #define MTA_DLL_NAME            "core_d.dll"
    #define CHECK_DM_LUA_NAME       "mods\\deathmatch\\lua5.1c_d.dll"
    #define CHECK_DM_CLIENT_NAME    "mods\\deathmatch\\Client_d.dll"
#else
    #define MTA_EXE_NAME            "Multi Theft Auto.exe"
    #define MTA_DLL_NAME            "core.dll"
    #define CHECK_DM_LUA_NAME       "mods\\deathmatch\\lua5.1c.dll"
    #define CHECK_DM_CLIENT_NAME    "mods\\deathmatch\\Client.dll"
#endif
