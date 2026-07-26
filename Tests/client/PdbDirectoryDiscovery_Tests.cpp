/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/PdbDirectoryDiscovery_Tests.cpp
 *  PURPOSE:     Google Test suite for bounded PDB directory discovery
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include <windows.h>

#include "../../Client/core/PdbDirectoryDiscovery.h"

namespace
{
    class PdbDirectoryDiscoveryTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            const auto uniqueSuffix =
                std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(std::chrono::steady_clock::now().time_since_epoch().count());
            m_root = std::filesystem::temp_directory_path() / (L"mtasa-pdb-directory-discovery-" + uniqueSuffix);
            ASSERT_TRUE(std::filesystem::create_directories(m_root / L"MTA"));
        }

        void TearDown() override
        {
            std::error_code ec;
            std::filesystem::remove_all(m_root, ec);
        }

        void CreateFile(const std::filesystem::path& path)
        {
            ASSERT_TRUE(std::filesystem::create_directories(path.parent_path()) || std::filesystem::is_directory(path.parent_path()));

            std::ofstream file{path};
            ASSERT_TRUE(file.is_open());
            file.put('\0');
        }

        std::filesystem::path m_root;
    };
}

TEST_F(PdbDirectoryDiscoveryTest, FindsPdbsOnlyBesideClientBinaries)
{
    CreateFile(m_root / L"launcher.pdb");
    CreateFile(m_root / L"MTA" / L"core.PDB");
    CreateFile(m_root / L"MTA" / L"nested" / L"ignored.pdb");
    CreateFile(m_root / L"mods" / L"deathmatch" / L"resources" / L"ignored.pdb");

    const auto directories = CrashHandler::Details::FindPdbDirectories(m_root);

    ASSERT_EQ(directories.size(), 2u);
    EXPECT_EQ(directories[0], m_root);
    EXPECT_EQ(directories[1], m_root / L"MTA");
}

TEST_F(PdbDirectoryDiscoveryTest, IgnoresPdbsOutsideClientBinaryDirectories)
{
    CreateFile(m_root / L"MTA" / L"nested" / L"ignored.pdb");
    CreateFile(m_root / L"mods" / L"deathmatch" / L"resources" / L"ignored.pdb");

    EXPECT_TRUE(CrashHandler::Details::FindPdbDirectories(m_root).empty());
}
