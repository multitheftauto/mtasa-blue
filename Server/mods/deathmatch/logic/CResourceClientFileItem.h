/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceClientFileItem.h
 *  PURPOSE:     Resource client-side file item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CResourceFile.h"

class CResourceClientFileItem : public CResourceFile
{
public:
    CResourceClientFileItem(class CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes,
                            bool bClientAutoDownload = true);
    ~CResourceClientFileItem();

    bool Start();
    bool Stop();

    bool IsAutoDownload() { return m_bClientAutoDownload; };

private:
    bool m_bClientAutoDownload;
};
