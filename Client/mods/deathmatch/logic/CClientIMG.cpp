/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientIMG.cpp
 *  PURPOSE:     IMG container class
 *
 *****************************************************************************/

#include <StdInc.h>
#include "game/CStreaming.h"

struct tImgHeader
{
    char         szMagic[4];
    unsigned int uiFilesCount;
};

CClientIMG::CClientIMG(class CClientManager* pManager, ElementID ID)
    : ClassInit(this),
      CClientEntity(ID),
      m_pImgManager(pManager->GetIMGManager()),
      m_ucArchiveID(INVALID_ARCHIVE_ID),
      m_LargestFileSizeBlocks(0),
      m_bFolderMode(false),
      m_bTempImgCreated(false)
{
    m_pManager = pManager;
    SetTypeName("img");
    m_pImgManager->AddToList(this);
}

CClientIMG::~CClientIMG()
{
    m_pImgManager->RemoveFromList(this);
    Unlink();
}

void CClientIMG::Unlink()
{
    if (IsStreamed())
        StreamDisable();

    if (m_ifs.is_open() || m_bFolderMode)
        Unload();
}

bool CClientIMG::Load(fs::path filePath)
{
    if (!m_fileInfos.empty())
        return false;

    if (m_ifs.is_open())
        return false;

    if (filePath.empty())
        return false;

    if (!fs::exists(filePath))
        return false;

    m_filePath = filePath;
    m_ifs = std::ifstream(filePath, std::ios::binary);

    // Open the file
    if (m_ifs.fail())
    {
        m_ifs.close();
        return false;
    }

    tImgHeader fileHeader;

    // Read header
    m_ifs.read(reinterpret_cast<char*>(&fileHeader), sizeof(tImgHeader));

    if (m_ifs.fail() || m_ifs.eof() || memcmp(&fileHeader.szMagic, "VER2", 4) != 0)
    {
        m_ifs.close();
        return false;
    }

    // Read content info
    try
    {
        m_fileInfos.resize(fileHeader.uiFilesCount);
    }
    catch (const std::bad_alloc&)
    {
        m_ifs.close();
        return false;
    }

    m_ifs.read(reinterpret_cast<char*>(m_fileInfos.data()), sizeof(tImgFileInfo) * (std::streampos)fileHeader.uiFilesCount);
    if (m_ifs.fail() || m_ifs.eof())
    {
        m_ifs.close();
        return false;
    }

    return true;
}

void CClientIMG::Unload()
{
    m_fileInfos.clear();
    m_fileInfos.shrink_to_fit();
    m_ifs.close();

    if (m_bFolderMode)
    {
        m_folderFilePaths.clear();
        m_folderFilePaths.shrink_to_fit();

        if (m_bTempImgCreated && !m_tempImgPath.empty())
        {
            std::error_code ec;
            fs::remove(m_tempImgPath, ec);
            m_tempImgPath.clear();
        }

        m_bTempImgCreated = false;
        m_bFolderMode = false;
    }

    if (!m_tempImgDirPath.empty())
    {
        std::error_code ec;
        for (const auto& entry : fs::directory_iterator(m_tempImgDirPath, ec))
        {
            if (ec)
                break;

            if (!entry.is_regular_file())
                continue;

            const auto filename = entry.path().filename().string();
            if (filename.find("runtime-image") != std::string::npos)
                fs::remove(entry.path(), ec);
        }
    }
}

bool CClientIMG::GetFile(size_t fileID, std::string& buffer)
{
    const tImgFileInfo* pFileInfo = GetFileInfo(fileID);
    if (!pFileInfo)
        throw std::invalid_argument("Invalid file id");

    const auto toReadBytes = (size_t)pFileInfo->usSize * 2048u;

    try
    {
        buffer.resize(toReadBytes);
    }
    catch (const std::bad_alloc&)
    {
        throw std::invalid_argument("Out of memory");
    }

    if (!m_ifs.is_open())
    {
        m_ifs = std::ifstream(m_filePath, std::ios::binary);
        if (m_ifs.fail())
        {
            m_ifs.close();
            return false;
        }
    }

    m_ifs.clear();
    m_ifs.seekg((std::streampos)pFileInfo->uiOffset * 2048);
    m_ifs.read(buffer.data(), toReadBytes);

    return !m_ifs.fail() && !m_ifs.eof();
}

tImgFileInfo* CClientIMG::GetFileInfo(size_t fileID)
{
    if (fileID >= m_fileInfos.size())
        return nullptr;
    return &m_fileInfos[fileID];
}

std::optional<size_t> CClientIMG::GetFileID(std::string_view filename)
{
    const auto it =
        std::find_if(m_fileInfos.begin(), m_fileInfos.end(), [filename](const auto& fileInfo) { return filename.compare(fileInfo.szFileName) == 0; });

    if (it == m_fileInfos.end())
        return std::nullopt;
    return std::distance(m_fileInfos.begin(), it);
}

bool CClientIMG::IsStreamed()
{
    return m_ucArchiveID != INVALID_ARCHIVE_ID;
}

bool CClientIMG::LoadFolder(fs::path folderPath, fs::path tempDirPath)
{
    if (!m_fileInfos.empty())
        return false;

    if (folderPath.empty())
        return false;

    if (!fs::is_directory(folderPath))
        return false;

    m_tempImgDirPath = std::move(tempDirPath);

    if (!m_tempImgDirPath.empty())
    {
        std::error_code ec;
        fs::create_directories(m_tempImgDirPath, ec);
        if (ec)
            return false;

        for (const auto& entry : fs::directory_iterator(m_tempImgDirPath, ec))
        {
            if (ec)
                break;

            if (!entry.is_regular_file())
                continue;

            const auto filename = entry.path().filename().string();
            if (filename.find("runtime-image") != std::string::npos)
                fs::remove(entry.path(), ec);
        }
    }

    // Collect regular files and sort alphabetically for deterministic ordering
    std::vector<fs::directory_entry> entries;
    for (const auto& entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
            entries.push_back(entry);
    }
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) { return a.path().filename() < b.path().filename(); });

    CResourceManager* pResourceManager = m_pManager ? m_pManager->GetResourceManager() : nullptr;

    unsigned int uiCurrentOffset = 0;
    for (const auto& entry : entries)
    {
        const auto& filePath = entry.path();
        const auto  filename = filePath.filename().string();

        // tImgFileInfo::szFileName is 24 bytes (23 usable chars + null terminator)
        if (filename.size() > 23)
            continue;

        if (pResourceManager)
        {
            SString strFilePath = SharedUtil::ToUTF8(filePath.wstring());
            SString strConformed = PathConform(strFilePath).ToLower();

            // Include only files known to the resource download manager
            if (!pResourceManager->GetDownloadableResourceFile(strConformed))
                continue;
        }

        std::error_code ec;
        const auto      fileSize = entry.file_size(ec);
        if (ec || fileSize == 0)
            continue;

        const auto sizeInBlocks = static_cast<unsigned short>((fileSize + 2047) / 2048);

        tImgFileInfo fileInfo{};
        fileInfo.uiOffset = uiCurrentOffset;
        fileInfo.usSize = sizeInBlocks;
        fileInfo.usUnpackedSize = 0;
        std::strncpy(fileInfo.szFileName, filename.c_str(), 23);
        fileInfo.szFileName[23] = '\0';

        m_fileInfos.push_back(fileInfo);
        m_folderFilePaths.push_back(filePath);

        uiCurrentOffset += sizeInBlocks;
    }

    if (m_fileInfos.empty())
        return false;

    m_bFolderMode = true;
    return true;
}

bool CClientIMG::BuildTempIMG()
{
    if (m_tempImgDirPath.empty())
        return false;

    std::error_code ec;
    fs::create_directories(m_tempImgDirPath, ec);
    if (ec)
        return false;

    const unsigned long long base = static_cast<unsigned long long>(GetTickCount64_());
    const unsigned long long pid = static_cast<unsigned long long>(GetCurrentProcessId());
    const std::wstring       fileName = L"runtime-image-" + std::to_wstring(base) + L"-" + std::to_wstring(pid) + L".tmp";
    m_tempImgPath = m_tempImgDirPath / fileName;

    ec.clear();
    if (fs::exists(m_tempImgPath, ec) || ec)
        return false;

    std::ofstream ofs(m_tempImgPath, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open())
    {
        fs::remove(m_tempImgPath);
        return false;
    }

    std::string       readBuf;
    std::vector<char> zeroPad;

    for (size_t i = 0; i < m_folderFilePaths.size(); i++)
    {
        const auto& filePath = m_folderFilePaths[i];
        const auto& fileInfo = m_fileInfos[i];

        std::error_code ec;
        const auto      fileSize = fs::file_size(filePath, ec);
        if (ec)
        {
            ofs.close();
            fs::remove(m_tempImgPath);
            return false;
        }

        try
        {
            readBuf.resize(fileSize);
        }
        catch (const std::bad_alloc&)
        {
            ofs.close();
            fs::remove(m_tempImgPath);
            return false;
        }

        std::ifstream ifs(filePath, std::ios::binary);
        if (!ifs.is_open())
        {
            ofs.close();
            fs::remove(m_tempImgPath);
            return false;
        }

        ifs.read(readBuf.data(), fileSize);
        if (ifs.fail())
        {
            ofs.close();
            fs::remove(m_tempImgPath);
            return false;
        }

        ofs.write(readBuf.data(), fileSize);

        const size_t paddedSize = static_cast<size_t>(fileInfo.usSize) * 2048;
        const size_t paddingNeeded = paddedSize - fileSize;
        if (paddingNeeded > 0)
        {
            zeroPad.assign(paddingNeeded, '\0');
            ofs.write(zeroPad.data(), paddingNeeded);
        }

        if (ofs.fail())
        {
            ofs.close();
            fs::remove(m_tempImgPath);
            return false;
        }
    }

    ofs.close();
    m_filePath = m_tempImgPath;
    m_bTempImgCreated = true;
    return true;
}

bool CClientIMG::StreamEnable()
{
    if (m_fileInfos.empty())
        return false;

    if (IsStreamed())
        return false;

    if (m_bFolderMode && !m_bTempImgCreated)
    {
        if (!BuildTempIMG())
            return false;
    }

    if (m_LargestFileSizeBlocks == 0)
    {
        for (const auto& fileInfo : m_fileInfos)
            m_LargestFileSizeBlocks = std::max(m_LargestFileSizeBlocks, (size_t)fileInfo.usSize);
    }

    m_ucArchiveID = g_pGame->GetStreaming()->AddArchive(m_filePath.wstring().c_str());

    if (IsStreamed())
    {
        m_pImgManager->UpdateStreamerBufferSize();
        return true;
    }
    return false;
}

bool CClientIMG::StreamDisable()
{
    if (!IsStreamed())
        return false;

    // Unlink all models
    for (const auto& v : m_restoreInfo)
    {
        g_pGame->GetStreaming()->SetStreamingInfo(v.uiModelID, v.ucStreamID, v.uiOffset, v.usSize);
    }
    m_restoreInfo.clear();
    m_restoreInfo.shrink_to_fit();

    // Remove archive from streaming
    g_pGame->GetStreaming()->RemoveArchive(m_ucArchiveID);
    m_ucArchiveID = INVALID_ARCHIVE_ID;

    m_pImgManager->UpdateStreamerBufferSize();

    // During session shutdown (CClientManager being destroyed), element destruction
    // order is arbitrary. Skip restreaming because earlier element cleanup may have
    // already freed TXD pool slots, and ReinitStreaming would flush pending
    // streaming channels that reference those freed parent slots.
    if (!m_pManager || !m_pManager->IsBeingDeleted())
        g_pClientGame->RestreamWorld();

    return true;
}

bool CClientIMG::LinkModel(unsigned int uiModelID, size_t uiFileID)
{
    if (!IsStreamed())
        return false;

    tImgFileInfo* pFileInfo = GetFileInfo(uiFileID);
    if (!pFileInfo)
        return false;

    CStreamingInfo* pCurrInfo = g_pGame->GetStreaming()->GetStreamingInfo(uiModelID);
    if (!pCurrInfo)
        return false;

    if (pCurrInfo->archiveId == m_ucArchiveID)
        return true;  // Already linked

    m_restoreInfo.emplace_back(uiModelID, pCurrInfo->offsetInBlocks, pCurrInfo->sizeInBlocks, pCurrInfo->archiveId);

    // Internally stream out the vehicle before calling CStreamingSA::RemoveModel
    // otherwise a crash will occur if the player is inside a vehicle that gets unloaded by the streamer
    if (CClientVehicleManager::IsValidModel(uiModelID))
        g_pClientGame->GetVehicleManager()->RestreamVehicles(static_cast<unsigned short>(uiModelID));

    g_pGame->GetStreaming()->SetStreamingInfo(uiModelID, m_ucArchiveID, pFileInfo->uiOffset, pFileInfo->usSize);

    return true;
}

bool CClientIMG::UnlinkModel(unsigned int uiModelID)
{
    const auto it =
        std::find_if(m_restoreInfo.begin(), m_restoreInfo.end(), [uiModelID](const auto& restoreInfo) { return restoreInfo.uiModelID == uiModelID; });

    if (it == m_restoreInfo.end())
        return false;

    g_pGame->GetStreaming()->SetStreamingInfo(uiModelID, it->ucStreamID, it->uiOffset, it->usSize);

    m_restoreInfo.erase(it);

    return true;
}
