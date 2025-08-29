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

class CResourceTranslationItem : public CResourceFile
{
public:
    CResourceTranslationItem(CResource* resource, const char* name, const char* src, CXMLAttributes* xmlAttributes = nullptr);
    ~CResourceTranslationItem() = default;

    bool Start() override;
    bool Stop() override;

    std::string GetLanguage() const noexcept { return m_language; }
    bool IsPrimary() const noexcept { return m_isPrimary; }

private:
    std::string ExtractLanguageFromName() const;

private:
    std::string m_language;
    bool m_isPrimary;
};
