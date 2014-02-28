/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Main.h
*  PURPOSE:     MTA loader
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __MAIN_H
#define __MAIN_H

#define MTA_GTAEXE_NAME "gta_sa.exe"
#define MTA_HTAEXE_NAME "proxy_sa.exe"
#define MTA_GTASTEAMEXE_NAME "gta-sa.exe"
#define MTA_GUID "Global\\{4962AF5F-5D82-412D-9CCA-AB8BB9DBD353}"
#define URI_CONNECT 1
#define MTA_EXE_NAME_RELEASE "Multi Theft Auto.exe"

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

#endif