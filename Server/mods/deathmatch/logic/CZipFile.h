/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CZipFile.h
 *  PURPOSE:     Zip file element class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"
#include <CZipMaker.h>
#include "lua/LuaCommon.h"
#include <cstdio>
#include <string>

class CZipFile final : public CElement
{
public:
    CZipFile(const uint uiScriptId, const char* szFilename, const CZipMaker::Mode mode = CZipMaker::Mode::READ);
    CZipFile(uint uiScriptId, const char* szFilename, const char mode);
    ~CZipFile();

    bool IsValid() const noexcept;
    bool AddFile(const SString& srcPath, const SString& destPath = "") noexcept;
    bool RemoveFile(const SString& srcPath) noexcept;
    bool ExtractFile(const SString& srcPath, const SString& destDir) noexcept;
    bool Extract(const SString& destDir) noexcept;
    bool Close() noexcept;

    // Functions required by CElement
    void Unlink(){};
protected:
    bool ReadSpecialData(const int iLine) override { return true; }
private:
    void Init(const uint uiScriptId, const char* szFilename) noexcept;

    CResource*    m_pResource;
    CZipMaker     m_zipMaker;
    uint          m_uiScriptId;
    SString       m_strFilename;
    SLuaDebugInfo m_LuaDebugInfo;
};
