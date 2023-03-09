/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/Install.cpp
 *  PURPOSE:     Handles the installation of updates for MTA
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "Install.h"
#include "Utils.h"
#include "Dialogs.h"
#include "Main.h"
#include <unrar/dll.hpp>
#include <optional>
#include <memory>
#include <filesystem>

static std::string ROLLBACK_NAME = "rollback.step";
static char        ARCHIVE_PASSWORD[] = "mta";

constexpr DWORD OPERATION_RETRY_DELAY_IN_MS = 100;

struct ChecksumFile
{
    std::optional<uint32_t> checksum{};
    SString                 absolutePath{};

    ChecksumFile& operator=(SString&& absolutePath_)
    {
        checksum.reset();
        absolutePath = absolutePath_;
        return *this;
    }

    void ComputeChecksum()
    {
        checksum.reset();

        if (FileExists(absolutePath))
            checksum = ComputeCRC32(absolutePath.c_str());
    }

    [[nodiscard]] auto c_str() const noexcept { return absolutePath.c_str(); }
    [[nodiscard]] bool operator==(const ChecksumFile& other) const noexcept { return checksum && other.checksum && (*checksum == *other.checksum); }
    [[nodiscard]] bool operator!=(const ChecksumFile& other) const noexcept { return !(*this == other); }
};

struct InstallableFile
{
    SString      relativePath{};
    ChecksumFile sourceFile{};
    ChecksumFile backupFile{};
    ChecksumFile targetFile{};
};

struct ArchiveHandleDeleter
{
    void operator()(HANDLE archiveHandle) const noexcept { RARCloseArchive(archiveHandle); }
};

using ArchiveHandle = std::unique_ptr<std::remove_pointer_t<HANDLE>, ArchiveHandleDeleter>;

/**
 * @brief Deletes a directory, its subfolders and the files contained on destruction.
*/
struct DirectoryDeleteScope
{
    std::filesystem::path directoryPath;

    explicit DirectoryDeleteScope(const std::string& directoryPath_) : directoryPath(directoryPath_) {}

    ~DirectoryDeleteScope()
    {
        if (!directoryPath.empty())
        {
            std::error_code ec{};
            std::filesystem::remove_all(directoryPath, ec);
        }
    }

    void Release() { std::exchange(directoryPath, {}); }
};

/**
 * @brief Extracts a single file out of an archive to the provided target directory.
 * @param archivePath Path to the archive
 * @param targetRoot Path to the target directory to append the relative path and extract the file to
 * @param fileName Name of the file to be extracted (case-insensitive)
 * @param password An optional password for the archive
 */
static bool ExtractSingleArchiveFile(const std::string& archivePath, const SString& targetRoot, const SString& fileName, char* password)
{
    if (archivePath.empty())
        return false;

    RAROpenArchiveDataEx archiveData{};
    archiveData.ArcName = const_cast<char*>(archivePath.data());
    archiveData.OpenMode = RAR_OM_EXTRACT;

    ArchiveHandle archiveHandle{RAROpenArchiveEx(&archiveData)};

    if (!archiveHandle)
        return false;

    if (password)
        RARSetPassword(archiveHandle.get(), password);

    SString targetRootDirectory = targetRoot;

    while (true)
    {
        RARHeaderDataEx header{};

        if (int errorCode = RARReadHeaderEx(archiveHandle.get(), &header); errorCode != ERAR_SUCCESS)
            return false;

        const bool extractFile = fileName.CompareI(header.FileName);
        const int  fileOperation = (extractFile ? RAR_EXTRACT : RAR_SKIP);

        if (int errorCode = RARProcessFile(archiveHandle.get(), fileOperation, targetRootDirectory.data(), nullptr); errorCode != ERAR_SUCCESS)
            return false;

        if (extractFile)
            return true;
    }

    return false;
}

/**
 * @brief Reads an archive file and optionally extracts the file to the current directory.
 * @param archivePath Path to the archive
 * @param files A list to add the files from the archive to
 * @param extractFiles If set to true, the archive files will be extracted to the current directory
 * @param password An optional password for the archive
 */
static bool ProcessArchive(const std::string& archivePath, std::vector<ManifestFile>& files, bool extractFiles, char* password)
{
    files.clear();

    RAROpenArchiveDataEx archiveData{};
    archiveData.ArcName = const_cast<char*>(archivePath.data());
    archiveData.OpenMode = (extractFiles ? RAR_OM_EXTRACT : RAR_OM_LIST);

    ArchiveHandle archiveHandle{RAROpenArchiveEx(&archiveData)};

    if (!archiveHandle)
        return false;

    if (password)
        RARSetPassword(archiveHandle.get(), password);

    const int fileOperation = (extractFiles ? RAR_EXTRACT : RAR_SKIP);

    for (size_t i = 0; /* no condition */; ++i)
    {
        RARHeaderDataEx header{};

        if (int errorCode = RARReadHeaderEx(archiveHandle.get(), &header); errorCode != ERAR_SUCCESS)
        {
            if (errorCode == ERAR_END_ARCHIVE)
                break;

            AddReportLog(5055, SString("ProcessArchive: Failed to read header (index: %zu, error: %d, archive: '%s')", i, errorCode, archivePath.c_str()));
            return false;
        }

        if (int errorCode = RARProcessFile(archiveHandle.get(), fileOperation, nullptr, nullptr); errorCode != ERAR_SUCCESS)
        {
            AddReportLog(5055, SString("ProcessArchive: Failed to process '%s' (index: %zu, error: %d, archive: %s)", header.FileName, i, errorCode,
                                       archivePath.c_str()));
            return false;
        }

        if (header.Flags & RHDF_DIRECTORY)
            continue;

        ManifestFile file{};
        file.relativePath = header.FileName;
        file.checksum = header.FileCRC;
        files.emplace_back(file);
    }

    OutputDebugLine(SString("ProcessArchive: Located %zu files in the archive '%s'", files.size(), archivePath.c_str()));
    return true;
}

/**
 * @brief Extracts files from an archive to the current directory.
 * @param archivePath Path to the archive
 * @param files A list to add the files from the archive to
 * @param password An optional password for the archive
 */
static bool ExtractArchiveFiles(const std::string& archivePath, std::vector<ManifestFile>& files, char* password)
{
    return ProcessArchive(archivePath, files, /* extractFiles= */ true, password);
}

/**
 * @brief Lists the files from an archive. This function does not extract any file.
 * @param archivePath Path to the archive
 * @param files A list to add the files from the archive to
 * @param password An optional password for the archive
 */
static bool GetArchiveFileList(const std::string& archivePath, std::vector<ManifestFile>& files, char* password)
{
    return ProcessArchive(archivePath, files, /* extractFiles= */ false, password);
}

/**
 * @brief Recursively lists the files from a directory.
 */
static void GetArchiveFilesInDirectory(const SString& directory, std::vector<ManifestFile>& files)
{
    std::vector<SString> fileList;
    FindFilesRecursive(PathJoin(directory, "*"), fileList);

    files.clear();
    files.reserve(fileList.size());

    const size_t relativePathOffset = directory.length() + 1;

    for (const SString& filePath : fileList)
    {
        ManifestFile file{};
        file.relativePath = filePath.substr(relativePathOffset);

        if (file.relativePath != MANIFEST_NAME && file.relativePath != ROLLBACK_NAME)
        {
            file.checksum = ComputeCRC32(filePath.c_str());
            files.emplace_back(file);
        }
    }
}

/**
 * @brief Terminates every process locking a file with the user's consent.
 * @param absolutePath The path to the file to check
 * @param displayName Name of the file in the user prompt
 * @return True if the file is not locked (anymore), false otherwise
 */
static bool TerminateFileLockingProcesses(const SString& absolutePath, const SString& displayName)
{
    WString            filePath(absolutePath);
    std::vector<DWORD> processIds = GetProcessListUsingFile(filePath);

    while (true)
    {
        if (processIds.empty())
            return true;

        SString nameList;

        for (DWORD processId : processIds)
        {
            if (!nameList.empty())
                nameList += '\n';

            SString processName = GetProcessFilename(processId);

            if (processName.empty())
                processName = _("Unknown");

            nameList += SString("   %s  [%lu]", processName.c_str(), processId);
        }

        int decision = MessageBoxUTF8(
            nullptr,
            SString(_("The file '%s' is currently locked by %zu processes.\n\nDo you want to terminate the following processes and continue updating?\n\n%s"),
                    displayName.c_str(), processIds.size(), nameList.c_str()),
            "MTA: San Andreas", MB_CANCELTRYCONTINUE | MB_ICONQUESTION | MB_TOPMOST);

        processIds = GetProcessListUsingFile(filePath);

        if (processIds.empty())
            return true;

        if (decision == IDTRYAGAIN)
            continue;

        if (decision != IDCONTINUE)
            return false;

        for (DWORD processId : processIds)
        {
            TerminateProcess(processId, 0);
        }

        processIds = GetProcessListUsingFile(filePath);
    }
}

/**
 * @brief Creates a writable directory for any purpose.
 * @param templateRoot The template path to a directory
 * @return A path to a new directory, empty string on error
*/
static SString CreateWritableDirectory(const SString& templateRoot)
{
    SString fileName = ExtractFilename(templateRoot);

    std::vector<SString> candidates{
        MakeUniquePath(templateRoot),
        PathJoin(GetMTATempPath(), "upcache", fileName),
    };

    const SString probeName = "probe.file";

    for (const SString& candidate : candidates)
    {
        if (!MkDir(candidate))
            continue;

        DirectoryDeleteScope deleteCandidate(candidate);

        const SString probeFile = PathJoin(candidate, probeName);

        if (!FileSave(probeFile, probeName))
            continue;

        if (FileSize(probeFile) != probeName.size())
            continue;

        FileDelete(probeFile);
        deleteCandidate.Release();
        return candidate;
    }

    return {};
}

/**
 * @brief Retrieves the list of files for the update.
 * @param sourceRoot Path to the directory with update files
 * @param archivePath Optional path to the archive with the update files
*/
static auto GetUpdateFiles(const SString& sourceRoot, const SString& archivePath) -> std::vector<ManifestFile>
{
    std::vector<ManifestFile> files{};
    int                       filesSource{};

    if (ParseManifestFile(sourceRoot, files))
    {
        filesSource = 1;
        OutputDebugLine(SString("GetUpdateFiles: Located %zu files in manifest", files.size()));
    }
    else if (!archivePath.empty() && GetArchiveFileList(archivePath, files, ARCHIVE_PASSWORD))
    {
        filesSource = 2;
        OutputDebugLine(SString("GetUpdateFiles: Located %zu files in archive", files.size()));
    }
    else
    {
        filesSource = 3;
        GetArchiveFilesInDirectory(sourceRoot, files);
        OutputDebugLine(SString("GetUpdateFiles: Located %zu files in directory", files.size()));
    }

    if (files.empty())
        AddReportLog(5055, SString("GetUpdateFiles: Found no files (source: %d)", filesSource));

    return files;
}

/**
 * @brief Retrieves the list of files from a backup.
 * @param backupRoot Path to the directory with backup files
 */
static auto GetBackupFiles(const SString& backupRoot) -> std::vector<ManifestFile>
{
    std::vector<ManifestFile> files{};
    int                       filesSource{};

    if (ParseManifestFile(backupRoot, files))
    {
        filesSource = 1;
        OutputDebugLine(SString("GetBackupFiles: Located %zu files in manifest", files.size()));
    }
    else
    {
        filesSource = 2;
        GetArchiveFilesInDirectory(backupRoot, files);
        OutputDebugLine(SString("GetBackupFiles: Located %zu files in directory", files.size()));
    }

    if (files.empty())
        AddReportLog(5055, SString("GetBackupFiles: Found no files (source: %d)", filesSource));

    return files;
}

/**
 * @brief Returns the content of a file that will be deleted.
 * @param filePath The path to get the content of and delete
*/
static auto GetOneShotFile(const SString& filePath) -> SString
{
    SString fileContent;
    FileLoad(filePath, fileContent);
    FileDelete(filePath);
    return fileContent;
}

/**
 * @brief Executes the rollback for the installed files, by copying the backup to the target.
 * @param files The files to restore
 * @return The amount of non-recoverable files
*/
static auto RunRollback(std::vector<InstallableFile>& files) -> size_t
{
    size_t disasterCounter{};

    for (InstallableFile& file : files)
    {
        if (file.targetFile == file.backupFile)
            continue;

        int  attempts = 0;
        int  checksums = 0;
        bool success = false;
        bool hasBackup = !!file.backupFile.checksum;

        // Try to restore the file with three attempts and make sure the checksum matches.
        while (!success && attempts < 3)
        {
            ++attempts;

            // Delete the file if it doesn't have a backup file.
            if (hasBackup)
            {
                if (success = FileCopy(file.backupFile.absolutePath, file.targetFile.absolutePath))
                {
                    ++checksums;
                    file.targetFile.ComputeChecksum();
                    success = (file.backupFile == file.targetFile);
                }
            }
            else
            {
                if (FileExists(file.targetFile.absolutePath))
                    success = FileDelete(file.targetFile.absolutePath);
                else
                    success = true;
            }

            if (!success)
            {
                if (attempts == 1)
                {
                    // If the first attempt didn't work, check if any process is locking one of the files.
                    TerminateFileLockingProcesses(file.targetFile.absolutePath, file.relativePath);
                    TerminateFileLockingProcesses(file.backupFile.absolutePath, file.relativePath);
                }

                Sleep(OPERATION_RETRY_DELAY_IN_MS);
            }
        }

        if (success)
            continue;

        ++disasterCounter;

        // Report a disaster only up to five times.
        if (disasterCounter <= 5)
        {
            bool exists = FileExists(file.targetFile.absolutePath);
            AddReportLog(5055, SString("RunRollback: Unable to restore backup from '%s' to '%s' (exists: %d, attempts: %d, checksums: %d)",
                                       file.backupFile.c_str(), file.targetFile.c_str(), exists, attempts, checksums));
        }
    }

    if (disasterCounter > 0)
    {
        MessageBoxUTF8(nullptr,
                       SString(_("Your installation may be corrupt now.\n\n%zu out of %zu files could not be restored from the backup.\n\nYou should "
                                 "reinstall Multi Theft Auto from www.multitheftauto.com\nor try running the update with administrator rights."),
                               disasterCounter, files.size()),
                       "MTA: San Andreas", MB_OK | MB_ICONERROR | MB_TOPMOST);

        AddReportLog(5055, SString("RunRollback: Rollback failed for %zu out of %zu files", disasterCounter, files.size()));
        OutputDebugLine(SString("RunRollback: Rollback failed for %zu out of %zu files", disasterCounter, files.size()));
    }
    
    return disasterCounter;
}

/**
 * @brief Completes a pending rollback.
 * @param sourceRoot Path to the directory with install files
 * @param targetRoot Path to the possibly corrupt target directory
*/
static bool CompletePendingRollback(const SString& sourceRoot, const SString& targetRoot)
{
    const SString hintFile = PathJoin(sourceRoot, ROLLBACK_NAME);
    const SString backupRoot = GetOneShotFile(hintFile);

    if (backupRoot.empty())
        return true;

    if (FileExists(hintFile))
    {
        AddReportLog(5055, SString("CompletePendingRollback: Hint file was not deleted: '%s'", hintFile.c_str()));
    }

    if (!DirectoryExists(backupRoot))
    {
        AddReportLog(5055, SString("CompletePendingRollback: Backup directory does not exist: '%s'", backupRoot.c_str()));
        return false;
    }

	// Check if we have to restore any files at all.
    std::vector<ManifestFile> backupFiles = GetBackupFiles(backupRoot);

    if (backupFiles.empty())
        return true;

	std::vector<InstallableFile> files;
    files.reserve(backupFiles.size());

    for (const ManifestFile& manifestFile : backupFiles)
    {
        InstallableFile file{};
        file.relativePath = manifestFile.relativePath;
        file.targetFile = PathJoin(targetRoot, manifestFile.relativePath);
        file.targetFile.ComputeChecksum();
        file.backupFile = PathJoin(backupRoot, manifestFile.relativePath);

        if (manifestFile.checksum)
            file.backupFile.checksum = manifestFile.checksum;

        if (file.targetFile != file.backupFile)
            files.emplace_back(file);
    }

    OutputDebugLine(SString("CompletePendingRollback: Rolling back %zu out of %zu files", files.size(), backupFiles.size()));

    if (files.empty())
    {
        AddReportLog(5055, SString("CompletePendingRollback: Rollback is obsolete (%zu files skipped)", backupFiles.size()));
        return true;
    }

    DirectoryDeleteScope deleteBackupRoot(backupRoot);

    if (size_t disasterCounter = RunRollback(files); disasterCounter > 0)
    {
        // Do not delete the backup directory if we need it for recovery.
        deleteBackupRoot.Release();

        bool hasHint = FileSave(PathJoin(sourceRoot, ROLLBACK_NAME), backupRoot);
        AddReportLog(5055, SString("CompletePendingRollback: %zu out of %zu files may be corrupt after rollback (hint: %d)", disasterCounter, files.size(), hasHint));
        return false;
    }
    
    return true;
}

/**
 * @brief Installs files from the current temporary directory to the Multi Theft Auto installation directory.
 * @return Zero on success, otherwise the step number the installation failed at
*/
static int RunInstall()
{
    const SString sourceRoot = PathConform(GetSystemCurrentDirectory());

    SString archivePath;
    {
        SString archiveDirectory, archiveFileName;
        sourceRoot.Split("\\", &archiveDirectory, &archiveFileName, -1);
        archiveFileName = archiveFileName.SubStr(1).SplitLeft("_tmp_", nullptr, -1);            // Cut archive name out of '_<archiveFileName>_tmp_'

        archivePath = MakeGenericPath(PathJoin(archiveDirectory, archiveFileName));
    }

    if (!FileExists(archivePath))
    {
        AddReportLog(5055, SString("RunInstall: Source archive does not exist: '%s' (source: '%s')", archivePath.c_str(), sourceRoot.c_str()));
        archivePath = "";
    }

    const SString targetRoot = PathConform(GetMTASAPath());

    if (!DirectoryExists(targetRoot))
    {
        AddReportLog(5055, SString("RunInstall: MTA directory does not exist: '%s'", targetRoot.c_str()));
        return 1;
    }

    // Check if we have to complete a pending rollback.
    if (!CompletePendingRollback(sourceRoot, targetRoot))
    {
        AddReportLog(5055, SStringX("RunInstall: Rollback has failed"));
        return 2;
    }

    // Gather a list of files to install.
    std::vector<ManifestFile> archiveFiles = GetUpdateFiles(sourceRoot, archivePath);

    if (archiveFiles.empty())
        return 0;

    // Create a backup directory for disaster recovery.
    const SString backupRoot = CreateWritableDirectory(sourceRoot + "_bak_");

    if (backupRoot.empty())
    {
        AddReportLog(5055, SStringX("RunInstall: Unable to create writable backup directory"));
        return 3;
    }

    DirectoryDeleteScope deleteBackupRoot(backupRoot);

    // Check if we have to install any files at all.
    std::vector<InstallableFile> files;
    files.reserve(archiveFiles.size());

    for (const ManifestFile& manifestFile : archiveFiles)
    {
        InstallableFile file{};
        file.relativePath = manifestFile.relativePath;
        file.backupFile = PathJoin(backupRoot, manifestFile.relativePath);
        file.targetFile = PathJoin(targetRoot, manifestFile.relativePath);
        file.targetFile.ComputeChecksum();
        file.sourceFile = PathJoin(sourceRoot, manifestFile.relativePath);
        file.sourceFile.checksum = manifestFile.checksum;

		if (file.sourceFile != file.targetFile)
            files.emplace_back(file);
    }

    OutputDebugLine(SString("RunInstall: Installing %zu out of %zu files", files.size(), archiveFiles.size()));

    if (files.empty())
        return 0;

    // Check if any executable or library is locked by any process and terminate it with the user's consent.
    for (const InstallableFile& file : files)
    {
        if (file.relativePath.EndsWithI(".exe") || file.relativePath.EndsWithI(".dll"))
        {
            if (!TerminateFileLockingProcesses(file.targetFile.absolutePath, file.relativePath))
                return 4;
        }
    }

    // Create a backup of currently installed files.
    for (InstallableFile& file : files)
    {
        // Skip files without a checksum (they shouldn't exist).
        if (!file.targetFile.checksum)
        {
            if (FileExists(file.targetFile.absolutePath))
            {
                AddReportLog(5055, SString("RunInstall: Target file '%s' exists but has no checksum", file.targetFile.absolutePath.c_str()));
                return 5;
            }

            continue;
        }

        int  attempts = 0;
        int  checksums = 0;
        bool success = false;

        // Try to backup the file with three attempts and make sure the checksum matches.
        while (!success && attempts < 3)
        {
            ++attempts;

            if (success = FileCopy(file.targetFile.absolutePath, file.backupFile.absolutePath))
            {
                ++checksums;
                file.backupFile.ComputeChecksum();
                success = (file.backupFile == file.targetFile);
            }
            
            if (!success)
            {
                if (!FileExists(file.targetFile.absolutePath))
                {
                    AddReportLog(5055, SString("RunInstall: Target file '%s' is missing (attempts: %d)", file.relativePath.c_str(), attempts));
                    break;
                }

                if (attempts == 1)
                {
                    // If the first attempt didn't work, check if any process is locking one of the files.
                    TerminateFileLockingProcesses(file.targetFile.absolutePath, file.relativePath);
                    TerminateFileLockingProcesses(file.backupFile.absolutePath, file.relativePath);
                }

                Sleep(OPERATION_RETRY_DELAY_IN_MS);
            }
        }

        if (!success)
        {
            bool     targetExists = FileExists(file.targetFile.absolutePath);
            uint64_t targetSize = FileSize(file.targetFile.absolutePath);
            bool     backupExists = FileExists(file.backupFile.absolutePath);
            uint64_t backupSize = FileSize(file.backupFile.absolutePath);
            AddReportLog(5055, SString("RunInstall: Unable to create backup of '%s' (attempts: %d, checksums: %d, target: %d [size:%llu hash:%08x], backup: %d "
                                       "[size:%llu hash:%08x])",
                                       file.relativePath.c_str(), attempts, checksums, targetExists, targetSize, file.targetFile.checksum.value_or(0),
                                       backupExists, backupSize, file.backupFile.checksum.value_or(0)));
            return 6;
        }
    }

    // Create manifest for the backup directory just in case we need it for a rollback.
    // This is allowed to fail. In such case the rollback won't be able to delete files introduced in the update.
    {
        std::vector<ManifestFile> backupFiles;
        backupFiles.reserve(files.size());

        for (const InstallableFile& file : files)
        {
            ManifestFile archiveFile{};
            archiveFile.relativePath = file.relativePath;
            archiveFile.checksum = file.targetFile.checksum.value_or(0);
            backupFiles.emplace_back(archiveFile);
        }

        GenerateManifestFile(backupRoot, backupFiles);
    }

    OutputDebugLine(SString("RunInstall: Created a backup for %zu files", files.size()));

    // Install new files.
    bool requiresRollback = false;

    for (InstallableFile& file : files)
    {
        int  attempts = 0;
        int  checksums = 0;
        bool success = false;

        // Try to install the file with three attempts and make sure the checksum matches.
        while (!success && attempts < 3)
        {
            ++attempts;

            if (success = FileCopy(file.sourceFile.absolutePath, file.targetFile.absolutePath))
            {
                ++checksums;
                file.targetFile.ComputeChecksum();
                success = (file.sourceFile == file.targetFile);
            }

            if (!success)
            {
                if (!FileExists(file.sourceFile.absolutePath))
                {
                    AddReportLog(5055, SString("RunInstall: Source file '%s' is missing (attempts: %d)", file.relativePath.c_str(), attempts));
                    break;
                }

                if (attempts == 1)
                {
                    // If the first attempt didn't work, check if any process is locking one of the files.
                    TerminateFileLockingProcesses(file.targetFile.absolutePath, file.relativePath);
                    TerminateFileLockingProcesses(file.sourceFile.absolutePath, file.relativePath);
                }

                Sleep(OPERATION_RETRY_DELAY_IN_MS);
            }
        }

        // If copying from the source directory fails, try to extract the file directly from the archive.
        if (!success)
        {
            if (ExtractSingleArchiveFile(archivePath, targetRoot, file.relativePath, ARCHIVE_PASSWORD))
            {
                ++checksums;
                file.targetFile.ComputeChecksum();
                success = (file.sourceFile == file.targetFile);
            }
        }

        if (!success)
        {
            // Update the target file checksum for rollback, because we don't know if the target file was modified in the install process.
            file.targetFile.ComputeChecksum();

            bool exists = FileExists(file.sourceFile.absolutePath);
            AddReportLog(5055, SString("RunInstall: Unable to install '%s' (exists: %d, attempts: %d, checksums: %d)", file.relativePath.c_str(), exists,
                                       attempts, checksums));
            requiresRollback = true;
            break;
        }
    }

    if (requiresRollback)
        OutputDebugLine(SStringX("RunInstall: Updating files failed"));
    else
        OutputDebugLine(SString("RunInstall: Updated %zu files", files.size()));

    const auto Rollback = [&]()
    {
        if (size_t disasterCounter = RunRollback(files); disasterCounter > 0)
        {
            // Do not delete the backup directory if we need it for recovery.
            deleteBackupRoot.Release();

            bool hasHint = FileSave(PathJoin(sourceRoot, ROLLBACK_NAME), backupRoot);
            AddReportLog(5055, SString("RunInstall: %zu out of %zu files may be corrupt after rollback (hint: %d)", disasterCounter, files.size(), hasHint));
        }
    };

    if (requiresRollback)
    {
        Rollback();
        return 7;
    }

    // Verify installed files.
    for (InstallableFile& file : files)
    {
        file.targetFile.ComputeChecksum();

        bool exists = !!file.targetFile.checksum;
        bool checksum = (file.targetFile == file.sourceFile);

        if (!exists || !checksum)
        {
            AddReportLog(5055, SString("RunInstall: Verification failed for '%s' (exists: %d, checksum: %d)", file.targetFile.c_str(), exists, checksum));
            requiresRollback = true;
            break;
        }
    }

    if (requiresRollback)
    {
        Rollback();
        return 8;
    }

    OutputDebugLine(SString("RunInstall: Installation of %zu files complete", files.size()));
    return 0;
}

bool InstallFiles(bool showProgressWindow)
{
    if (showProgressWindow)
        StartPseudoProgress(g_hInstance, "MTA: San Andreas", _("Installing update..."));

    bool success = false;

    if (int failureStep = RunInstall(); failureStep > 0)
    {
        AddReportLog(5055, SString("InstallFiles: Installation ended on step %d", failureStep));
        OutputDebugLine(SString("InstallFiles: Installation ended on step %d", failureStep));
    }
    else
    {
        // Make sure the correct service is created and started.
        success = CheckService(CHECK_SERVICE_POST_UPDATE);
    }

    StopPseudoProgress();
    return success;
}

bool ExtractFiles(const std::string& archivePath, bool withManifest)
{
    if (archivePath.empty() || !FileExists(archivePath))
        return false;

    std::vector<ManifestFile> files;

    if (!ExtractArchiveFiles(archivePath, files, ARCHIVE_PASSWORD))
    {
        AddReportLog(5055, SString("ExtractFiles: Failed to extract '%s'", archivePath.c_str()));
        return false;
    }

    if (files.empty())
    {
        AddReportLog(5055, SStringX("ExtractFiles: File extraction yielded zero files"));
        return false;
    }

    if (withManifest && !GenerateManifestFile(".", files))
    {
        // This is not a fatal error. We have fallback methods to work without a manifest.
        AddReportLog(5055, SStringX("ExtractFiles: Failed to generate manifest"));
    }

    return true;
}

SString CheckOnRestartCommand()
{
    const SString strMTASAPath = GetMTASAPath();

    SetCurrentDirectory(strMTASAPath);
    SetDllDirectory(strMTASAPath);

    SString strOperation, strFile, strParameters, strDirectory, strShowCmd;
    if (GetOnRestartCommand(strOperation, strFile, strParameters, strDirectory, strShowCmd))
    {
        if (strOperation == "files" || strOperation == "silent")
        {
            //
            // Update
            //

            if (strFile.empty() || !FileExists(strFile))
                return "FileMissing";

            // Make temp path name and go there
            SString strArchivePath, strArchiveName;
            strFile.Split("\\", &strArchivePath, &strArchiveName, -1);

            const SString sourceRoot = MakeUniquePath(strArchivePath + "\\_" + strArchiveName + "_tmp_");

            if (!MkDir(sourceRoot))
                return "FileError1";

            DirectoryDeleteScope deleteSourceRoot(sourceRoot);

            if (!SetCurrentDirectory(sourceRoot))
                return "FileError2";

            // Start progress bar
            if (!strParameters.Contains("hideprogress"))
                StartPseudoProgress(g_hInstance, "MTA: San Andreas", _("Extracting files..."));

            // Try to extract the files
            bool success = ExtractFiles(strFile, /* withManifest */ true);
            
            if (!success)
            {
                // If extract failed and update file is an .exe or .msu, try to run it
                if (ExtractExtension(strFile).EqualsI("exe") || ExtractExtension(strFile).EqualsI("msu"))
                    success = ShellExecuteBlocking("open", strFile, strParameters.SplitRight("###"));
            }

            // Stop progress bar
            StopPseudoProgress();

            if (!success)
                return "FileError3";

            deleteSourceRoot.Release();

            // If a new "Multi Theft Auto.exe" exists, let that complete the install
            if (FileExists(MTA_EXE_NAME_RELEASE))
                return "install from far " + strOperation + " " + strParameters;

            // Otherwise use the current exe to install
            return "install from near " + strOperation + " " + strParameters;
        }
        else
        {
            AddReportLog(5052, SString("CheckOnRestartCommand: Unknown restart command %s", strOperation.c_str()));
        }
    }

    return "no update";
}
