/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientIMG.h
*  PURPOSE:     IMG container class header

*
*****************************************************************************/

#pragma once

#include "CClientEntity.h"
#include <vector>
#include <fstream>
#include <optional>
#include <variant>
#include <string_view>
#include <filesystem>
namespace fs = std::filesystem;

struct tImgFileInfo
{
    std::uint32_t   uiOffset;
    std::uint16_t usSize;
    std::uint16_t usUnpackedSize;
    char           szFileName[24];
};

struct tLinkedModelRestoreInfo
{
    constexpr tLinkedModelRestoreInfo(std::uint32_t uiModelID, std::uint32_t uiOffset, std::uint16_t usSize, std::uint8_t ucStreamID)
        : uiModelID(uiModelID), uiOffset(uiOffset), usSize(usSize), ucStreamID(ucStreamID)
    {
    }

    std::uint32_t   uiModelID;
    std::uint32_t   uiOffset;
    std::uint16_t usSize;
    std::uint8_t  ucStreamID;
};

class CClientIMG : public CClientEntity
{
    DECLARE_CLASS(CClientIMG, CClientEntity)
    friend class CClientIMGManager;

public:
    CClientIMG(class CClientManager* pManager, ElementID ID);
    ~CClientIMG();

    void Unlink(){};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    eClientEntityType GetType() const { return CCLIENTIMG; }
    std::uint8_t     GetArchiveID() const { return m_ucArchiveID; }
    std::uint32_t      GetFilesCount() const { return m_fileInfos.size(); }
    const auto&       GetFileInfos() const noexcept { return m_fileInfos; }
    auto              GetLargestFileSizeBlocks() const { return m_LargestFileSizeBlocks; }

    bool Load(fs::path filePath);
    void Unload();

    tImgFileInfo*         GetFileInfo(size_t fileID);
    std::optional<size_t> GetFileID(std::string_view filename);
    bool                  GetFile(size_t uiFileID, std::string& buffer);

    bool StreamEnable();
    bool StreamDisable();
    bool IsStreamed();

    bool LinkModel(std::uint32_t usModelID, size_t fileID);
    bool UnlinkModel(std::uint32_t usModelID);

private:
    class CClientIMGManager* m_pImgManager;

    std::ifstream             m_ifs;
    fs::path                  m_filePath;
    std::uint8_t             m_ucArchiveID;
    std::vector<tImgFileInfo> m_fileInfos;
    size_t                    m_LargestFileSizeBlocks; // The size of the largest file [in streaming blocks/sectors]

    std::vector<tLinkedModelRestoreInfo> m_restoreInfo;
};
