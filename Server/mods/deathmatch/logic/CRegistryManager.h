/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRegistryManager.h
 *  PURPOSE:     Track registry objects
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CRegistry;

class CRegistryManager
{
public:
    ~CRegistryManager();
    void       DoPulse();
    CRegistry* OpenRegistry(const std::string& strFileName);
    bool       CloseRegistry(CRegistry* pRegistry);

private:
    std::vector<CRegistry*> m_RegistryList;
};
