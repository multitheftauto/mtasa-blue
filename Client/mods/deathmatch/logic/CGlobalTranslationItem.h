/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CResourceFile.h"
#include <string>

class CGlobalTranslationItem : public CResourceFile
{
public:
    CGlobalTranslationItem(CResource* resource, const char* src, uint uiDownloadSize, CChecksum serverChecksum);

    std::string GetProviderResourceName() const noexcept { return m_providerResourceName; }
    
    bool Start();
    bool Stop();

private:
    std::string m_providerResourceName;
};