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
    : ClassInit(this), CClientEntity(ID), m_pImgManager(pManager->GetIMGManager()), m_ucArchiveID(INVALID_ARCHIVE_ID), m_LargestFileSizeBlocks(0)
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

    if (m_ifs.is_open())
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

bool CClientIMG::StreamEnable()
{
    if (m_fileInfos.empty())
        return false;

    if (IsStreamed())
        return false;

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

    g_pClientGame->RestreamWorld(true);
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

    if (pCurrInfo->archiveId == m_ucArchiveID)
        return true;            // Already linked

    m_restoreInfo.emplace_back(uiModelID, pCurrInfo->offsetInBlocks, pCurrInfo->sizeInBlocks, pCurrInfo->archiveId);

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
