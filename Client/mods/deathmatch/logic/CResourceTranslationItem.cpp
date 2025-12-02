/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Client/mods/deathmatch/logic/CResourceTranslationItem.cpp
*  PURPOSE:     Resource translation item class
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CResourceTranslationItem.h"
#include "CResource.h"
#include "CResourceTranslationManager.h"
#include "CLogger.h"
#include <filesystem>

CResourceTranslationItem::CResourceTranslationItem(CResource* resource, const char* name, const char* src, uint uiDownloadSize, CChecksum serverChecksum, bool isPrimary)
    : CResourceFile(resource, RESOURCE_FILE_TYPE_TRANSLATION, name, src, uiDownloadSize, serverChecksum, true), m_isPrimary(isPrimary)
{
    m_language = ExtractLanguageFromName();
}


std::string CResourceTranslationItem::ExtractLanguageFromName() const
{
    // Cast away const since we know GetShortName() doesn't modify the object
    std::filesystem::path path(const_cast<CResourceTranslationItem*>(this)->GetShortName());
    return path.stem().string();
}
