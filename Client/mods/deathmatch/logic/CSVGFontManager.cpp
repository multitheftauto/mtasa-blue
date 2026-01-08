/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CSVGFontManager.h"
#include <lunasvg.h>
#include <ranges>

CSVGFontManager& CSVGFontManager::GetSingleton()
{
    static CSVGFontManager instance;
    return instance;
}

void CSVGFontManager::FontDataDestroyCallback(void* pData)
{
    delete[] static_cast<char*>(pData);
}

bool CSVGFontManager::RegisterFont(const SString& strFontFamily, const SString& strFontPath, CResource* pResource)
{
    if (strFontFamily.empty() || strFontPath.empty() || !pResource)
        return false;

    if (IsFontRegistered(strFontFamily))
        return false;

    SString strAbsPath;
    SString strMetaPath;

    CResource* pFileResource = pResource;
    if (!g_pClientGame->GetResourceManager()->ParseResourcePathInput(strFontPath, pFileResource, &strAbsPath, &strMetaPath))
        return false;

    if (!FileExists(strAbsPath))
        return false;

    // Get file size
    uint64 fileSize = FileSize(strAbsPath);
    if (fileSize == 0 || fileSize > 10 * 1024 * 1024) // max 10MB
        return false;

    // Allocate memory for font data that will be owned by LunaSVG
    size_t dataSize = static_cast<size_t>(fileSize);
    char* pFontData = new (std::nothrow) char[dataSize];
    if (!pFontData)
        return false;

    // Read file directly into our allocated buffer
    FILE* pFile = File::Fopen(strAbsPath, "rb");
    if (!pFile)
    {
        delete[] pFontData;
        return false;
    }

    size_t bytesRead = fread(pFontData, 1, dataSize, pFile);
    fclose(pFile);

    if (bytesRead != dataSize)
    {
        delete[] pFontData;
        return false;
    }

    // Register font with LunaSVG using data API
    // LunaSVG takes ownership of the data and will call our destroy callback when done
    if (!lunasvg_add_font_face_from_data(strFontFamily.c_str(), false, false, pFontData, dataSize, FontDataDestroyCallback, pFontData))
    {
        // Registration failed, clean up the memory ourselves
        delete[] pFontData;
        return false;
    }

    // Store font info for tracking
    SFontInfo fontInfo;
    fontInfo.strOriginalPath = strFontPath;
    fontInfo.strAbsolutePath = strAbsPath;
    fontInfo.pOwnerResource = pResource;

    m_RegisteredFonts[strFontFamily] = std::move(fontInfo);

    return true;
}

bool CSVGFontManager::UnregisterFont(const SString& strFontFamily)
{
    auto it = m_RegisteredFonts.find(strFontFamily);
    if (it == m_RegisteredFonts.end())
        return false;

    // Remove from our tracking
    // Note: The font data memory will be freed by LunaSVG via our destroy callback
    m_RegisteredFonts.erase(it);
    return true;
}

bool CSVGFontManager::IsFontRegistered(const SString& strFontFamily) const
{
    return m_RegisteredFonts.contains(strFontFamily);
}

CResource* CSVGFontManager::GetFontOwnerResource(const SString& strFontFamily) const
{
    auto it = m_RegisteredFonts.find(strFontFamily);
    if (it != m_RegisteredFonts.end())
        return it->second.pOwnerResource;
    return nullptr;
}

void CSVGFontManager::UnregisterResourceFonts(CResource* pResource)
{
    if (!pResource)
        return;

    // Collect fonts to remove
    std::vector<SString> fontsToRemove;
    for (const auto& pair : m_RegisteredFonts)
    {
        if (pair.second.pOwnerResource == pResource)
            fontsToRemove.push_back(pair.first);
    }

    // Remove collected fonts from our tracking
    for (const auto& fontFamily : fontsToRemove)
        m_RegisteredFonts.erase(fontFamily);
}

std::vector<SString> CSVGFontManager::GetRegisteredFonts() const
{
    auto keys = std::views::keys(m_RegisteredFonts);
    return {keys.begin(), keys.end()};
}
