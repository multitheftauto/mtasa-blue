/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <map>
#include <vector>

class CResource;

class CSVGFontManager
{
public:
    struct SFontInfo
    {
        SString    strOriginalPath;    // Original resource path for debugging
        SString    strAbsolutePath;    // Absolute path to font file
        CResource* pOwnerResource;     // Resource that registered this font
    };

    static CSVGFontManager& GetSingleton();


    bool RegisterFont(const SString& strFontFamily, const SString& strFontPath, CResource* pResource);
    bool UnregisterFont(const SString& strFontFamily);
    bool IsFontRegistered(const SString& strFontFamily) const;
    void UnregisterResourceFonts(CResource* pResource);
    std::vector<SString> GetRegisteredFonts() const;

private:
    CSVGFontManager() = default;
    ~CSVGFontManager() = default;

    CSVGFontManager(const CSVGFontManager&) = delete;
    CSVGFontManager& operator=(const CSVGFontManager&) = delete;
    
    static void FontDataDestroyCallback(void* pData);

    // Fonts we're currently tracking (owned by resources)
    std::map<SString, SFontInfo> m_RegisteredFonts;
};
