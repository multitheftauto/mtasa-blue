/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CFileGenerator.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CFileGenerator
{
public:
    enum class EResult
    {
        Success,
        TargetLoadError,
        TargetHashIncorrect,
        TargetMoveError,
        PatchInputError,
        PatchOutputError,
        ArchiveOpenError,
        ArchiveExtractError,
    };

    struct SResetItem
    {
        uint               uiFileOffset;
        uint               uiLength;
        std::vector<uchar> data;
    };

    CFileGenerator(const SString& strTarget, const SString& strRequiredTargetMd5, const std::vector<SResetItem>& targetResetList, const SString& strPatchBase,
                   const SString& strPatchDiff);
    bool    IsGenerationRequired();
    EResult GenerateFile();
    SString GetCurrentTargetMd5();
    SString GetErrorRecords();
    void    ClearErrorRecords();

protected:
    EResult CheckTarget(const SString& strTarget);
    EResult ApplyPatchFile(const SString& strPatchBase, const SString& strPatchDiff, const SString& strOutputFile);
    EResult UnrarFile(const SString& strArchive, const SString& strOutputFile);
    EResult RecordError(CFileGenerator::EResult code, const SString& strContext = "", const SString& strContext2 = "");

    struct SErrorInfo
    {
        EResult code;
        SString strContext;
        SString strContext2;
    };

    SString                 m_strTarget;
    SString                 m_strRequiredTargetMd5;
    SString                 m_strCurrentTargetMd5;
    std::vector<SResetItem> m_targetResetList;
    SString                 m_strPatchBase;
    SString                 m_strPatchDiff;
    std::vector<SErrorInfo> m_errorInfoList;
};
