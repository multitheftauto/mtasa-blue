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
    CGlobalTranslationItem(CResource* resource, const char* src, CXMLAttributes* xmlAttributes);

    std::string GetProviderResourceName() const noexcept { return m_providerResourceName; }
    
    bool Start() override;
    bool Stop() override;

private:
    std::string m_providerResourceName;
};