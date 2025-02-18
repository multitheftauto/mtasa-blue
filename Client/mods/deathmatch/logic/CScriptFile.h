/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CScriptFile.h
 *  PURPOSE:     Script file class header
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"
#include <stdio.h>
#include <string>

class CResourceFile;

class CScriptFile final : public CClientEntity
{
    DECLARE_CLASS(CScriptFile, CClientEntity)
public:
    enum eMode
    {
        MODE_READ,
        MODE_READWRITE,
        MODE_CREATE,
    };

    CScriptFile(uint uiScriptId, const char* szFilename, unsigned long ulMaxSize, eAccessType accessType);
    ~CScriptFile();

    // Functions required for linking
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    // Functions required by CClientEntity
    eClientEntityType GetType() const { return SCRIPTFILE; };
    void              Unlink(){};

    // Load and unload routines
    bool           Load(CResource* pResourceForFilePath, eMode Mode);
    void           Unload();
    bool           IsLoaded() { return m_pFile != NULL; };
    const SString& GetFilePath() { return m_strFilename; };
    const SString& GetAbsPath() { return m_strAbsPath; };

    // Get the owning resource
    CResource* GetResource();

    // Get the respective resource file (null if not found).

    /**
     * @brief Returns a pointer to CResourceFile if the script file points to one.
     * @return A pointer to CResourceFile on success, null otherwise
    */
    CResourceFile* GetResourceFile() const;

    // Only call functions below this if you're sure that the file is loaded.
    // Or you will crash.
    bool IsEOF();
    long GetPointer();
    long GetSize();

    long SetPointer(unsigned long ulPosition);

    void Flush();
    long Read(unsigned long ulSize, SString& outBuffer);
    long Write(unsigned long ulSize, const char* pData);

    long GetContents(std::string& buffer);

    // Debug info for garbage collected files
    const SLuaDebugInfo& GetLuaDebugInfo() { return m_LuaDebugInfo; };
    void                 SetLuaDebugInfo(const SLuaDebugInfo& luaDebugInfo) { m_LuaDebugInfo = luaDebugInfo; };

private:
    void DoResourceFileCheck();

    CBinaryFileInterface* m_pFile;
    SString               m_strFilename;            // Resource relative
    SString               m_strAbsPath;             // Absolute
    unsigned long         m_ulMaxSize;
    bool                  m_bDoneResourceFileCheck;
    unsigned int          m_uiScriptId;
    CResource*            m_pResource;
    eAccessType           m_accessType;
    SLuaDebugInfo         m_LuaDebugInfo;
};
