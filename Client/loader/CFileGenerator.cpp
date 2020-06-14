/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CFileGenerator.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../../vendor/unrar/dll.hpp"

CFileGenerator::CFileGenerator(const SString& strTarget, const SString& strTargetMd5, const std::vector<CFileGenerator::SResetItem>& targetResetList,
                               const SString& strPatchBase, const SString& strPatchDiff)
    : m_strTarget(strTarget), m_strTargetMd5(strTargetMd5), m_targetResetList(targetResetList), m_strPatchBase(strPatchBase), m_strPatchDiff(strPatchDiff)
{
}

//////////////////////////////////////////////////////////
// Return true if target file is incorrect
//////////////////////////////////////////////////////////
bool CFileGenerator::IsGenerationRequired()
{
    return (CheckTarget(m_strTarget) != EResult::Success);
}

//////////////////////////////////////////////////////////
// Check if target file has correct hash
//////////////////////////////////////////////////////////
CFileGenerator::EResult CFileGenerator::CheckTarget(const SString& strTarget)
{
    // Load into a buffer
    CBuffer buffer;
    if (!buffer.LoadFromFile(strTarget))
        return EResult::TargetLoadError;

    // List of pokes to restore
    CBufferWriteStream stream(buffer);
    for (auto& row : m_targetResetList)
    {
        stream.Seek(row.uiFileOffset);
        for (uint i = 0; i < row.uiLength; i++)
        {
            uchar c = row.data[i % row.data.size()];
            stream.Write(c);
        }
    }

    if (GenerateHashHexString(EHashFunctionType::MD5, buffer.GetData(), buffer.GetSize()) != m_strTargetMd5)
        return EResult::TargetHashIncorrect;
    return EResult::Success;
}

//////////////////////////////////////////////////////////
// Generate target file
//////////////////////////////////////////////////////////
CFileGenerator::EResult CFileGenerator::GenerateFile()
{
    // Base + Diff => rar archive containing new target file
    SString strTmpArchive = m_strPatchDiff + ".tmp";
    EResult result = ApplyPatchFile(m_strPatchBase, m_strPatchDiff, strTmpArchive);
    if (result == EResult::Success)
    {
        // Extract new target file
        SString strTmpNewTarget = m_strTarget + ".tmp";
        result = UnrarFile(strTmpArchive, strTmpNewTarget);
        if (result == EResult::Success)
        {
            // Make sure new file has correct hash
            result = CheckTarget(strTmpNewTarget);
            if (result == EResult::Success)
            {
                // Move in to place
                if (MoveFileExW(FromUTF8(strTmpNewTarget), FromUTF8(m_strTarget), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED) != 0)
                {
                    // Make sure final file has correct hash
                    result = CheckTarget(m_strTarget);
                }
                else
                {
                    result = EResult::TargetMoveError;
                }
            }
        }
        FileDelete(strTmpNewTarget);
    }
    FileDelete(strTmpArchive);
    return result;
}

//////////////////////////////////////////////////////////
// Combine PatchBase and PatchDiff to generate output file
//////////////////////////////////////////////////////////
CFileGenerator::EResult CFileGenerator::ApplyPatchFile(const SString& strPatchBase, const SString& strPatchDiff, const SString& strOutputFile)
{
    std::vector<char> baseData;
    std::vector<char> diffData;
    FileLoad(strPatchBase, baseData);
    FileLoad(strPatchDiff, diffData);
    if (baseData.empty() || diffData.empty())
        return EResult::PatchInputError;

    // Reuncompression using future delta system
    uint uiPos = 0;
    for (auto& c : diffData)
    {
        c ^= baseData[uiPos];
        uiPos = (uiPos + 1) % baseData.size();
    }

    if (!FileSave(strOutputFile, &diffData[0], diffData.size()))
        return EResult::PatchOutputError;
    return EResult::Success;
}

//////////////////////////////////////////////////////////
// Extract first file from archive
//////////////////////////////////////////////////////////
CFileGenerator::EResult CFileGenerator::UnrarFile(const SString& strArchive, const SString& strOutputFile)
{
    // Open archive
    RAROpenArchiveData archiveData = {};
    archiveData.ArcName = (char*)*strArchive;
    archiveData.OpenMode = RAR_OM_EXTRACT;
    HANDLE hArcData = RAROpenArchive(&archiveData);
    if (!hArcData)
        return EResult::ArchiveOpenError;

    // Extract first file
    EResult       result = EResult::ArchiveExtractError;
    RARHeaderData headerData = {};
    if (RARReadHeader(hArcData, &headerData) == 0)
    {
        if (RARProcessFile(hArcData, RAR_EXTRACT, nullptr, (char*)*strOutputFile) == 0)
        {
            result = EResult::Success;
        }
    }
    RARCloseArchive(hArcData);
    return result;
}
