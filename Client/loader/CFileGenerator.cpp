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

CFileGenerator::CFileGenerator(const SString& strTarget, const SString& strRequiredTargetMd5, const std::vector<CFileGenerator::SResetItem>& targetResetList,
                               const SString& strPatchBase, const SString& strPatchDiff)
    : m_strTarget(strTarget),
      m_strRequiredTargetMd5(strRequiredTargetMd5),
      m_targetResetList(targetResetList),
      m_strPatchBase(strPatchBase),
      m_strPatchDiff(strPatchDiff)
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
        return RecordError(EResult::TargetLoadError, strTarget);

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

    m_strCurrentTargetMd5 = GenerateHashHexString(EHashFunctionType::MD5, buffer.GetData(), buffer.GetSize());
    if (m_strCurrentTargetMd5 != m_strRequiredTargetMd5)
        return RecordError(EResult::TargetHashIncorrect, strTarget);
    return EResult::Success;
}

//////////////////////////////////////////////////////////
// Return actual MD5 of target file
//////////////////////////////////////////////////////////
SString CFileGenerator::GetCurrentTargetMd5()
{
    return m_strCurrentTargetMd5;
}

//////////////////////////////////////////////////////////
// Generate target file
//////////////////////////////////////////////////////////
CFileGenerator::EResult CFileGenerator::GenerateFile()
{
    ClearErrorRecords();
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
                    result = RecordError(EResult::TargetMoveError, strTmpNewTarget, m_strTarget);
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
    if (baseData.empty())
        return RecordError(EResult::PatchInputError, strPatchBase);
    if (diffData.empty())
        return RecordError(EResult::PatchInputError, strPatchDiff);

    // Reuncompression using future delta system
    uint uiPos = 0;
    for (auto& c : diffData)
    {
        c ^= baseData[uiPos];
        uiPos = (uiPos + 1) % baseData.size();
    }

    if (!FileSave(strOutputFile, &diffData[0], diffData.size()))
        return RecordError(EResult::PatchOutputError, strOutputFile);
    return EResult::Success;
}

//////////////////////////////////////////////////////////
// Extract first file from archive
//////////////////////////////////////////////////////////
CFileGenerator::EResult CFileGenerator::UnrarFile(const SString& strArchive, const SString& strOutputFile)
{
    WString wstrArchive = FromUTF8(strArchive);
    WString wstrOutputFile = FromUTF8(strOutputFile);
    // Open archive
    RAROpenArchiveDataEx archiveData = {};
    archiveData.ArcNameW = (wchar_t*)*wstrArchive;
    archiveData.OpenMode = RAR_OM_EXTRACT;
    HANDLE hArcData = RAROpenArchiveEx(&archiveData);
    if (!hArcData)
        return RecordError(EResult::ArchiveOpenError, strArchive);

    // Extract first file
    EResult       result = EResult::ArchiveExtractError;
    RARHeaderData headerData = {};
    if (RARReadHeader(hArcData, &headerData) == 0)
    {
        if (RARProcessFileW(hArcData, RAR_EXTRACT, nullptr, (wchar_t*)*wstrOutputFile) == 0)
        {
            result = EResult::Success;
        }
    }
    RARCloseArchive(hArcData);

    if (result != EResult::Success)
        return RecordError(result, strArchive, strOutputFile);
    return result;
}

//////////////////////////////////////////////////////////
// Save error info for later
//////////////////////////////////////////////////////////
CFileGenerator::EResult CFileGenerator::RecordError(CFileGenerator::EResult code, const SString& strContext, const SString& strContext2)
{
    m_errorInfoList.push_back({code, strContext, strContext2});
    return code;
}

//////////////////////////////////////////////////////////
// Remove all error records
//////////////////////////////////////////////////////////
void CFileGenerator::ClearErrorRecords()
{
    m_errorInfoList.clear();
}

//////////////////////////////////////////////////////////
// Return all error records as a string
//////////////////////////////////////////////////////////
SString CFileGenerator::GetErrorRecords()
{
    SString strStatus;
    for (const auto& item : m_errorInfoList)
    {
        strStatus += SString("[%d-%s-%s]", item.code, *item.strContext, *item.strContext2);
    }
    ClearErrorRecords();
    return strStatus;
}
