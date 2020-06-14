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

    CFileGenerator(const SString& strTarget, const SString& strTargetMd5, const std::vector<SResetItem>& targetResetList, const SString& strPatchBase,
                   const SString& strPatchDiff);
    bool           IsGenerationRequired();
    EResult        GenerateFile();

protected:
    EResult        CheckTarget(const SString& strTarget);
    static EResult ApplyPatchFile(const SString& strPatchBase, const SString& strPatchDiff, const SString& strOutputFile);
    static EResult UnrarFile(const SString& strArchive, const SString& strOutputFile);

    SString                 m_strTarget;
    SString                 m_strTargetMd5;
    std::vector<SResetItem> m_targetResetList;
    SString                 m_strPatchBase;
    SString                 m_strPatchDiff;
};
