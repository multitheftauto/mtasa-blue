/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CScriptFile.h
 *  PURPOSE:     Script file element class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"
#include "lua/LuaCommon.h"
#include <stdio.h>
#include <string>

class CResourceFile;

class CScriptFile final : public CElement
{
public:
    enum eMode
    {
        MODE_READ,
        MODE_READWRITE,
        MODE_CREATE,
    };

    CScriptFile(uint uiScriptId, const char* szFilename, std::uint32_t ulMaxSize);
    ~CScriptFile();

    // Functions required by CElement
    void Unlink(){};

    // Load and unload routines
    bool           Load(CResource* pResourceForFilePath, eMode Mode);
    void           Unload();
    bool           IsLoaded() { return m_pFile != NULL; };
    const SString& GetFilePath() { return m_strFilename; };

    // Get the owning resource
    CResource* GetResource();

    /**
     * @brief Returns a pointer to CResourceFile if the script file points to one.
     * @return A pointer to CResourceFile on success, null otherwise
     */
    CResourceFile* GetResourceFile() const;

    // Only call functions belw this if you're sure that the file is loaded.
    // Or you will crash.
    bool IsEOF();
    long GetPointer();
    long GetSize();

    long SetPointer(std::uint32_t ulPosition);
    void SetSize(std::uint32_t ulNewSize);

    void Flush();
    long Read(std::uint32_t ulSize, SString& outBuffer);
    long Write(std::uint32_t ulSize, const char* pData);

    long GetContents(std::string& buffer);

    // Debug info for garbage collected files
    const SLuaDebugInfo& GetLuaDebugInfo() { return m_LuaDebugInfo; };
    void                 SetLuaDebugInfo(const SLuaDebugInfo& luaDebugInfo) { m_LuaDebugInfo = luaDebugInfo; };

protected:
    bool ReadSpecialData(const int iLine) override { return true; }

private:
    CResource*    m_pResource;
    FILE*         m_pFile;
    uint          m_uiScriptId;
    SString       m_strFilename;
    std::uint32_t m_ulMaxSize;
    SLuaDebugInfo m_LuaDebugInfo;
};
