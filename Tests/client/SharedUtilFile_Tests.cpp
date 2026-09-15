/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/SharedUtilFile_Tests.cpp
 *  PURPOSE:     Tests for the file loading helpers in Shared/sdk/SharedUtil.File.hpp
 *
 *  These cover the contract that the number of bytes read must come from the open
 *  handle. Directory entry metadata (GetFileAttributesEx) is not guaranteed to be current
 *  on NTFS, so sizing a read from it can silently produce an empty or truncated buffer
 *  and still report success - see issue #5120.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.h>

#include <filesystem>
#include <string>

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace SharedUtil;

namespace
{
    class TempFile
    {
    public:
        explicit TempFile(const char* szSuffix)
        {
            const std::filesystem::path path = std::filesystem::temp_directory_path() / (std::string("mta_filetest_") + szSuffix + ".bin");
            m_strPath = path.string().c_str();
            Remove();
        }

        ~TempFile() { Remove(); }

        const SString& Path() const { return m_strPath; }

        void Write(const std::string& data) const { ASSERT_TRUE(FileSave(m_strPath, data.data(), static_cast<unsigned long>(data.size()), true)); }

        void Remove() const { FileDelete(m_strPath); }

    private:
        SString m_strPath;
    };

    std::string MakePayload(size_t size, char seed)
    {
        std::string data(size, '\0');
        for (size_t i = 0; i < size; ++i)
            data[i] = static_cast<char>(seed + static_cast<char>(i % 61));
        return data;
    }
}  // namespace

///////////////////////////////////////////////////////////////
//
// FileLoad(std::nothrow_t, ...)
//
///////////////////////////////////////////////////////////////

TEST(SharedUtilFile, FileLoadNoThrowReadsWholeFile)
{
    TempFile          file("nothrow_whole");
    const std::string payload = MakePayload(5000, 'a');
    file.Write(payload);

    SString buffer;
    ASSERT_TRUE(FileLoad(std::nothrow, file.Path(), buffer));
    EXPECT_EQ(buffer.size(), payload.size());
    EXPECT_EQ(std::string(buffer.data(), buffer.size()), payload);
}

TEST(SharedUtilFile, FileLoadNoThrowEmptyFileSucceedsWithEmptyBuffer)
{
    TempFile file("nothrow_empty");
    file.Write(std::string());

    SString buffer;
    EXPECT_TRUE(FileLoad(std::nothrow, file.Path(), buffer));
    EXPECT_TRUE(buffer.empty());
}

TEST(SharedUtilFile, FileLoadNoThrowMissingFileFails)
{
    TempFile file("nothrow_missing");

    SString buffer = "leftovers";
    EXPECT_FALSE(FileLoad(std::nothrow, file.Path(), buffer));
    EXPECT_TRUE(buffer.empty());
}

TEST(SharedUtilFile, FileLoadNoThrowHonoursOffsetAndMaxSize)
{
    TempFile          file("nothrow_offset");
    const std::string payload = MakePayload(1000, 'q');
    file.Write(payload);

    SString buffer;
    ASSERT_TRUE(FileLoad(std::nothrow, file.Path(), buffer, 100, 200));
    EXPECT_EQ(buffer.size(), 100u);
    EXPECT_EQ(std::string(buffer.data(), buffer.size()), payload.substr(200, 100));
}

// Deleting and re-creating a file with different contents must not serve the old length
TEST(SharedUtilFile, FileLoadNoThrowSeesRewrittenContents)
{
    TempFile file("nothrow_rewrite");

    for (int i = 1; i <= 8; ++i)
    {
        const std::string payload = MakePayload(static_cast<size_t>(i) * 733, static_cast<char>('a' + i));
        file.Remove();
        file.Write(payload);

        SString buffer;
        ASSERT_TRUE(FileLoad(std::nothrow, file.Path(), buffer)) << "iteration " << i;
        EXPECT_EQ(buffer.size(), payload.size()) << "iteration " << i;
        EXPECT_EQ(std::string(buffer.data(), buffer.size()), payload) << "iteration " << i;
    }
}

#if defined(_WIN32) && defined(MTA_CLIENT)

///////////////////////////////////////////////////////////////
//
// FileLoadWithTimeout / GetFileInfoWithTimeout
//
///////////////////////////////////////////////////////////////

TEST(SharedUtilFile, FileLoadWithTimeoutReadsWholeFile)
{
    TempFile          file("timeout_whole");
    const std::string payload = MakePayload(5000, 'a');
    file.Write(payload);

    SString       buffer;
    SFileReadInfo info;
    ASSERT_TRUE(FileLoadWithTimeout(file.Path(), buffer, 2000, &info));
    EXPECT_EQ(buffer.size(), payload.size());
    EXPECT_EQ(std::string(buffer.data(), buffer.size()), payload);
    EXPECT_EQ(info.size, payload.size());
    EXPECT_NE(info.mtime, 0u);
}

TEST(SharedUtilFile, FileLoadWithTimeoutEmptyFileSucceedsWithEmptyBuffer)
{
    TempFile file("timeout_empty");
    file.Write(std::string());

    SString       buffer;
    SFileReadInfo info;
    EXPECT_TRUE(FileLoadWithTimeout(file.Path(), buffer, 2000, &info));
    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(info.size, 0u);
}

TEST(SharedUtilFile, FileLoadWithTimeoutMissingFileFails)
{
    TempFile file("timeout_missing");

    SString buffer = "leftovers";
    EXPECT_FALSE(FileLoadWithTimeout(file.Path(), buffer, 2000));
    EXPECT_TRUE(buffer.empty());
}

TEST(SharedUtilFile, FileLoadWithTimeoutSeesRewrittenContents)
{
    TempFile file("timeout_rewrite");

    for (int i = 1; i <= 8; ++i)
    {
        const std::string payload = MakePayload(static_cast<size_t>(i) * 733, static_cast<char>('a' + i));
        file.Remove();
        file.Write(payload);

        SString       buffer;
        SFileReadInfo info;
        ASSERT_TRUE(FileLoadWithTimeout(file.Path(), buffer, 2000, &info)) << "iteration " << i;
        EXPECT_EQ(buffer.size(), payload.size()) << "iteration " << i;
        EXPECT_EQ(std::string(buffer.data(), buffer.size()), payload) << "iteration " << i;
        EXPECT_EQ(info.size, payload.size()) << "iteration " << i;
    }
}

TEST(SharedUtilFile, GetFileInfoWithTimeoutReportsHandleSize)
{
    TempFile          file("timeout_info");
    const std::string payload = MakePayload(1234, 'z');
    file.Write(payload);

    SFileReadInfo info;
    ASSERT_TRUE(GetFileInfoWithTimeout(file.Path(), info, 2000));
    EXPECT_EQ(info.size, payload.size());
    EXPECT_NE(info.mtime, 0u);

    file.Remove();
    SFileReadInfo missingInfo;
    EXPECT_FALSE(GetFileInfoWithTimeout(file.Path(), missingInfo, 2000));
    EXPECT_EQ(missingInfo.size, 0u);
}

// A writer holding the file open is the state in which the directory entry is most likely to
// disagree with the file, since it is refreshed on close or flush rather than on every write
TEST(SharedUtilFile, ReadsFileWithUnflushedWriterHandleOpen)
{
    TempFile          file("timeout_unflushed");
    const std::string payload = MakePayload(64 * 1024, 'k');

    const WString wide = FromUTF8(file.Path());
    HANDLE        writer = CreateFileW(wide.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    ASSERT_NE(writer, INVALID_HANDLE_VALUE);

    DWORD written = 0;
    ASSERT_TRUE(WriteFile(writer, payload.data(), static_cast<DWORD>(payload.size()), &written, nullptr));
    ASSERT_EQ(written, payload.size());

    SString       buffer;
    SFileReadInfo info;
    const bool    loaded = FileLoadWithTimeout(file.Path(), buffer, 2000, &info);

    SString    noThrowBuffer;
    const bool noThrowLoaded = FileLoad(std::nothrow, file.Path(), noThrowBuffer);

    CloseHandle(writer);

    ASSERT_TRUE(loaded);
    EXPECT_EQ(info.size, payload.size());
    EXPECT_EQ(std::string(buffer.data(), buffer.size()), payload);

    ASSERT_TRUE(noThrowLoaded);
    EXPECT_EQ(std::string(noThrowBuffer.data(), noThrowBuffer.size()), payload);
}

#endif
