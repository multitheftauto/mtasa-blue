/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CZipFile.cpp
 *  PURPOSE:     Zip file element class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CZipFile.h"
#include "CGame.h"
#include "CResourceManager.h"

void CZipFile::Init(const uint uiScriptId, const char* szFilename) noexcept
{
    // Init
    m_iType = CElement::ZIPFILE;
    SetTypeName("zipfile");
    m_uiScriptId = uiScriptId;
    m_strFilename = szFilename ? szFilename : "";
    m_pResource = nullptr;
}

CZipFile::CZipFile(const uint uiScriptId, const char* szFilename, const CZipMaker::Mode mode) : CElement(NULL)
{
    Init(uiScriptId, szFilename);
    m_zipMaker = CZipMaker(szFilename, mode);
}
CZipFile::CZipFile(const uint uiScriptId, const char* szFilename, const char mode) : CElement(NULL)
{
    Init(uiScriptId, szFilename);
    m_zipMaker = CZipMaker(szFilename, mode);
}

CZipFile::~CZipFile() { Close(); }

bool CZipFile::IsValid() const noexcept
{
    return m_zipMaker.IsValid();
}

bool CZipFile::AddFile(const SString& srcPath, const SString& destPath) noexcept
{
    return m_zipMaker.Insert(srcPath, destPath);
}
bool CZipFile::RemoveFile(const SString& srcPath) noexcept
{
    return m_zipMaker.RemoveFile(srcPath);
}

bool CZipFile::ExtractFile(const SString& srcPath, const SString& destDir) noexcept
{
    return m_zipMaker.ExtractFile(srcPath, destDir);
}
bool CZipFile::Extract(const SString& destDir) noexcept
{
    return m_zipMaker.Extract(destDir);
}

bool CZipFile::Close() noexcept
{
    return m_zipMaker.Close();
}
