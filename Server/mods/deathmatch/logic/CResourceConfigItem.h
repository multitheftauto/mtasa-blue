/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceConfigItem.h
 *  PURPOSE:     Resource server-side (XML) configuration file item class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CResourceFile.h"
#include <list>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

class CResourceConfigItem : public CResourceFile
{
public:
    CResourceConfigItem(class CResource* resource, const char* szShortName, const char* szResourceFileName,
                        const std::list<std::unique_ptr<CXMLAttribute>>& xmlAttributes);
    ~CResourceConfigItem();

    bool                   Start();
    bool                   Stop();
    inline class CXMLFile* GetFile() { return m_pXMLFile; }
    inline class CXMLNode* GetRoot() { return m_pXMLRootNode; }

private:
    class CXMLFile* m_pXMLFile;
    CXMLNode*       m_pXMLRootNode;

    bool m_bInvalid;
};
