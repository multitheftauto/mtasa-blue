/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceConfigItem.h
 *  PURPOSE:     Header for resource config item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CResource.h"
#include "CDownloadableResource.h"
#include <list>

#ifndef MAX_PATH
#define MAX_PATH        260
#endif

class CResourceConfigItem : public CDownloadableResource
{
public:
    CResourceConfigItem(class CResource* resource, const char* szShortName, const char* szResourceFileName, uint uiDownloadSize, CChecksum serverChecksum);
    ~CResourceConfigItem();

    bool            Start();
    bool            Stop();
    class CXMLFile* GetFile() { return m_pXMLFile; }
    class CXMLNode* GetRoot() { return m_pXMLRootNode; }

private:
    class CXMLFile* m_pXMLFile;
    CXMLNode*       m_pXMLRootNode;
};
