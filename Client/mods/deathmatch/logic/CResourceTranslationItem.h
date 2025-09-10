/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Client/mods/deathmatch/logic/CResourceTranslationItem.h
*  PURPOSE:     Resource translation item class
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CResourceFile.h"

class CResourceTranslationItem : public CResourceFile
{
public:
    CResourceTranslationItem(CResource* resource, const char* name, const char* src, uint uiDownloadSize, CChecksum serverChecksum, bool isPrimary = false);
    
    const std::string& GetLanguage() const { return m_language; }
    bool IsPrimary() const noexcept { return m_isPrimary; }

private:
    std::string ExtractLanguageFromName() const;
    std::string m_language;
    bool m_isPrimary;
};
