/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.ClothesReplacing.cpp
 *
 *****************************************************************************/

#include "StdInc.h"
#include <core/CCoreInterface.h>
#include <fstream>
#include "CGameSA.h"
#include "CDirectorySA.h"
#include "gamesa_renderware.h"

extern CGameSA* pGame;

namespace
{
    struct SImgGTAItemInfo
    {
        ushort usNext;
        ushort usPrev;

        ushort uiUnknown1;  // Parent ?
        uchar  uiUnknown2;  // 0x12 when loading, 0x02 when finished loading
        uchar  ucImgId;

        int  iBlockOffset;
        int  iBlockCount;
        uint uiLoadflag;  // 0-not loaded  2-requested  3-loaded  1-processed
    };

    std::unordered_map<ushort, char*>         ms_ReplacementClothesFileDataMap;
    std::unordered_map<ushort, std::uint16_t> ms_OriginalStreamingSizesMap;
    std::unordered_map<std::string, char*>    ms_ClothesFileDataMap;

    bool clothesReplacementChanged = false;

    struct SPlayerImgItem
    {
        uint       uiBlockOffset;
        uint       uiBlockCount;
        const char szName[24];
    };

    struct SPlayerImgItemArray
    {
        SPlayerImgItem* pItems;
        uint            uiArrayMax;
        uint            uiArraySize;
    };

    DWORD FUNC_CStreamingConvertBufferToObject = 0x40C6B0;
    auto  g_clothesDirectory = reinterpret_cast<CDirectorySAInterface*>(0xBC12C0);
    int   iReturnFileId;
    char* pReturnBuffer;

    size_t GetSizeInBlocks(size_t size)
    {
        auto blockDiv = std::div(size, 2048);
        return (blockDiv.quot + (blockDiv.rem ? 1 : 0));
    }
}  // namespace

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ClothesAddReplacement
//
// Add replacement txd/dff for a clothing component
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ClothesAddReplacement(char* pFileData, size_t fileSize, ushort usFileId)
{
    if (!pFileData)
        return;

    const size_t streamingSizeBlocks = GetSizeInBlocks(fileSize);
    if (streamingSizeBlocks > 0xFFFF)
        return;

    if (pFileData != MapFindRef(ms_ReplacementClothesFileDataMap, usFileId))
    {
        MapSet(ms_ReplacementClothesFileDataMap, usFileId, pFileData);
        MapSet(ms_OriginalStreamingSizesMap, usFileId, g_clothesDirectory->GetModelStreamingSize(usFileId));
        g_clothesDirectory->SetModelStreamingSize(usFileId, static_cast<std::uint16_t>(streamingSizeBlocks));

        clothesReplacementChanged = true;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ClothesRemoveReplacement
//
// Remove replacement txd/dff for a clothing component
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ClothesRemoveReplacement(char* pFileData)
{
    if (!pFileData)
        return;

    for (auto iter = ms_ReplacementClothesFileDataMap.begin(); iter != ms_ReplacementClothesFileDataMap.end();)
    {
        if (iter->second == pFileData)
        {
            auto it = ms_OriginalStreamingSizesMap.find(iter->first);

            if (it != ms_OriginalStreamingSizesMap.end())
            {
                std::uint16_t originalStreamingSize = it->second;
                g_clothesDirectory->SetModelStreamingSize(iter->first, originalStreamingSize);
            }

            iter = ms_ReplacementClothesFileDataMap.erase(iter);
            clothesReplacementChanged = true;
        }
        else
            ++iter;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::HasClothesReplacementChanged
//
// Returns true (once) if clothes textures need regenerating
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::HasClothesReplacementChanged()
{
    bool bResult = clothesReplacementChanged;
    clothesReplacementChanged = false;
    return bResult;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ClothesAddFile
//
// Add a file to the clothes directory
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::ClothesAddFile(const char* fileData, std::size_t fileSize, const char* fileName)
{
    if (!fileData || !fileName)
        return false;

    if (MapFind(ms_ClothesFileDataMap, fileName))
        return false;

    DirectoryInfoSA entry{};
    const size_t    streamingSizeBlocks = GetSizeInBlocks(fileSize);
    if (streamingSizeBlocks > 0xFFFF)
        return false;
    entry.m_streamingSize = static_cast<std::uint16_t>(streamingSizeBlocks);

    std::size_t nameSize = sizeof(entry.m_name) - 1;
    std::strncpy(entry.m_name, fileName, nameSize);
    entry.m_name[nameSize] = '\0';

    if (!g_clothesDirectory->AddEntry(entry))
        return false;

    MapSet(ms_ClothesFileDataMap, fileName, const_cast<char*>(fileData));
    clothesReplacementChanged = true;

    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ClothesRemoveFile
//
// Remove a file from the clothes directory
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::ClothesRemoveFile(char* fileData)
{
    if (!fileData)
        return false;

    bool removed = false;

    for (auto iter = ms_ClothesFileDataMap.begin(); iter != ms_ClothesFileDataMap.end();)
    {
        if (iter->second == fileData)
        {
            if (!g_clothesDirectory->RemoveEntry(iter->first.c_str()))
                return false;

            iter = ms_ClothesFileDataMap.erase(iter);
            clothesReplacementChanged = true;
            removed = true;
        }
        else
            ++iter;
    }

    return removed;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::HasClothesFile
//
// Check if clothe file exits
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::HasClothesFile(const char* fileName) const noexcept
{
    return fileName && MapFind(ms_ClothesFileDataMap, fileName);
}

namespace
{
    // player.img is an IMG archive (version 2): the "VER2" magic and the number of entries, followed by one entry per file.
    // Offsets and sizes are counted in blocks, and the file id MTA uses for clothes is the index of the entry.
    constexpr std::size_t IMG_BLOCK_SIZE = 2048;
    constexpr std::size_t IMG_HEADER_SIZE = 8;  // "VER2" + std::uint32_t entry count
    constexpr std::size_t IMG_ENTRY_SIZE = 32;  // std::uint32_t offset, std::uint16_t streaming size, std::uint16_t size in archive, char name[24]

    constexpr std::uint32_t RW_ID_TEXDICTIONARY = 0x16;
    constexpr std::size_t   RW_CHUNK_HEADER_SIZE = 12;  // chunk type, chunk size (without this header), library version

    // The size of the whole RenderWare chunk that starts with this header
    std::size_t GetChunkTotalSize(const char* chunkHeader)
    {
        std::uint32_t chunkSize = 0;
        std::memcpy(&chunkSize, chunkHeader + sizeof(std::uint32_t), sizeof(chunkSize));
        return static_cast<std::size_t>(chunkSize) + RW_CHUNK_HEADER_SIZE;
    }

    // Get the bytes of a clothes TXD, the same ones the game would load for this file.
    // A replacement added by a script wins, otherwise the original file is read straight from player.img.
    // The game only keeps these files around while it builds the textures of a ped, so there is no TXD to look them up in.
    bool ReadClothesTxdFile(ushort usFileId, std::string& outData)
    {
        if (const char* replacementFileData = MapFindRef(ms_ReplacementClothesFileDataMap, usFileId))
        {
            // The replacement is a full TXD file. The streaming size of the file is rounded up to blocks, so the chunk header gives the exact size
            const std::size_t maxSize = g_clothesDirectory->GetModelStreamingSize(usFileId) * IMG_BLOCK_SIZE;
            if (maxSize < RW_CHUNK_HEADER_SIZE)
                return false;

            outData.assign(replacementFileData, std::min(GetChunkTotalSize(replacementFileData), maxSize));
            return true;
        }

        // The game opens the archive with this same relative path
        std::ifstream file("models\\player.img", std::ios::binary);

        char archiveHeader[IMG_HEADER_SIZE];
        if (!file.read(archiveHeader, sizeof(archiveHeader)) || std::memcmp(archiveHeader, "VER2", 4) != 0)
            return false;

        std::uint32_t entryCount = 0;
        std::memcpy(&entryCount, archiveHeader + 4, sizeof(entryCount));
        if (usFileId >= entryCount)
            return false;

        char entry[IMG_ENTRY_SIZE];
        file.seekg(IMG_HEADER_SIZE + static_cast<std::size_t>(usFileId) * IMG_ENTRY_SIZE);
        if (!file.read(entry, sizeof(entry)))
            return false;

        std::uint32_t offsetInBlocks = 0;
        std::uint16_t streamingSizeInBlocks = 0;
        std::uint16_t sizeInArchiveInBlocks = 0;
        std::memcpy(&offsetInBlocks, entry, sizeof(offsetInBlocks));
        std::memcpy(&streamingSizeInBlocks, entry + 4, sizeof(streamingSizeInBlocks));
        std::memcpy(&sizeInArchiveInBlocks, entry + 6, sizeof(sizeInArchiveInBlocks));

        const std::size_t maxSize = std::max(streamingSizeInBlocks, sizeInArchiveInBlocks) * IMG_BLOCK_SIZE;
        if (maxSize < RW_CHUNK_HEADER_SIZE)
            return false;

        char chunkHeader[RW_CHUNK_HEADER_SIZE];
        file.seekg(static_cast<std::uint64_t>(offsetInBlocks) * IMG_BLOCK_SIZE);
        if (!file.read(chunkHeader, sizeof(chunkHeader)))
            return false;

        std::uint32_t chunkType = 0;
        std::memcpy(&chunkType, chunkHeader, sizeof(chunkType));
        if (chunkType != RW_ID_TEXDICTIONARY)
            return false;

        const std::size_t totalSize = std::min(GetChunkTotalSize(chunkHeader), maxSize);
        outData.resize(totalSize);
        std::memcpy(outData.data(), chunkHeader, sizeof(chunkHeader));
        return totalSize == RW_CHUNK_HEADER_SIZE || static_cast<bool>(file.read(outData.data() + RW_CHUNK_HEADER_SIZE, totalSize - RW_CHUNK_HEADER_SIZE));
    }

    // Read a clothes TXD into a texture dictionary that the caller has to destroy
    RwTexDictionary* ReadClothesTxd(ushort usFileId)
    {
        std::string fileData;
        if (!ReadClothesTxdFile(usFileId, fileData))
            return nullptr;

        RwBuffer streamBuffer;
        streamBuffer.ptr = fileData.data();
        streamBuffer.size = fileData.size();
        RwStream* stream = RwStreamOpen(STREAM_TYPE_BUFFER, STREAM_MODE_READ, &streamBuffer);
        if (!stream)
            return nullptr;

        // Unlike CRenderWareSA::ReadTXD this dictionary is only used here, so it does not have to be registered as a script added TXD
        RwTexDictionary* pTxd = nullptr;
        if (RwStreamFindChunk(stream, RW_ID_TEXDICTIONARY, nullptr, nullptr))
            pTxd = RwTexDictionaryGtaStreamRead(stream);

        RwStreamClose(stream, nullptr);
        return pTxd;
    }
}  // namespace

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetClothesTextureNames
//
// Get the names of the textures in a clothes TXD of player.img
// Returns false if usFileId is not a TXD of player.img
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::GetClothesTextureNames(std::vector<SString>& outNameList, ushort usFileId)
{
    outNameList.clear();

    RwTexDictionary* pTxd = ReadClothesTxd(usFileId);
    if (!pTxd)
        return false;

    std::vector<RwTexture*> textureList;
    GetTxdTextures(textureList, pTxd);

    for (const RwTexture* pTexture : textureList)
        outNameList.push_back(pTexture->name);

    RwTexDictionaryDestroy(pTxd);
    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetClothesTextures
//
// Get the textures of a clothes TXD of player.img
// If any texture names are specified in vTextureNames, only the textures matching one of them are returned
// Returns false if usFileId is not a TXD of player.img
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::GetClothesTextures(std::vector<std::tuple<std::string, CPixels>>& outTextureList, ushort usFileId,
                                       const std::vector<SString>& vTextureNames)
{
    outTextureList.clear();

    RwTexDictionary* pTxd = ReadClothesTxd(usFileId);
    if (!pTxd)
        return false;

    std::vector<RwTexture*> textureList;
    GetTxdTextures(textureList, pTxd);

    CollectTexturePixels(outTextureList, textureList, vTextureNames);

    RwTexDictionaryDestroy(pTxd);
    return true;
}

////////////////////////////////////////////////////////////////
//
// CStreaming_RequestModel_Mid
//
// If request is for a file inside player.img (imgId 5)
// then maybe switch to replacement txd/dff file data
//
////////////////////////////////////////////////////////////////
__declspec(noinline) bool _cdecl OnCStreaming_RequestModel_Mid(int flags, SImgGTAItemInfo* pImgGTAInfo)
{
    // Check is player.img
    if (pImgGTAInfo->ucImgId != 5)
        return false;

    // Early out if no clothes textures to replace with
    if (ms_ReplacementClothesFileDataMap.empty() && ms_ClothesFileDataMap.empty())
        return false;

    static std::map<std::uint32_t, int>  blockOffsetToFileIdMap;
    std::map<std::uint32_t, std::string> blockOffsetToFileNameMap;

    if (blockOffsetToFileIdMap.empty() || ms_ClothesFileDataMap.size() > 0)
    {
        // Check is player.img dir has been loaded by GTA
        SPlayerImgItemArray* pItemArray = (SPlayerImgItemArray*)0x00BC12C0;
        std::uint32_t        defaultArraySize = 542;
        std::uint32_t        maxArraySize = defaultArraySize + ms_ClothesFileDataMap.size();

        if (!pItemArray->pItems || pItemArray->uiArraySize != maxArraySize)
            return false;

        for (std::uint32_t i = 0; i < pItemArray->uiArraySize; i++)
        {
            SPlayerImgItem* pImgItem = &pItemArray->pItems[i];

            if (i < defaultArraySize)
                MapSet(blockOffsetToFileIdMap, pImgItem->uiBlockOffset, i);

            MapSet(blockOffsetToFileNameMap, pImgItem->uiBlockOffset, pImgItem->szName);
        }
    }

    char* replacementFileData = nullptr;
    int*  playerImgFileId = MapFind(blockOffsetToFileIdMap, pImgGTAInfo->iBlockOffset);

    if (playerImgFileId)
        replacementFileData = MapFindRef(ms_ReplacementClothesFileDataMap, *playerImgFileId);

    if (!replacementFileData)
    {
        std::string* fileName = MapFind(blockOffsetToFileNameMap, pImgGTAInfo->iBlockOffset);

        if (fileName)
            replacementFileData = MapFindRef(ms_ClothesFileDataMap, *fileName);
    }

    if (!replacementFileData)
        return false;

    // If bLoadingBigModel is set, try to get it unset
#define VAR_CStreaming_bLoadingBigModel 0x08E4A58
    BYTE& bLoadingBigModel = *(BYTE*)VAR_CStreaming_bLoadingBigModel;
    if (bLoadingBigModel)
    {
        pGame->GetStreaming()->LoadAllRequestedModels(true);
        if (bLoadingBigModel)
            pGame->GetStreaming()->LoadAllRequestedModels(false);
        assert(!bLoadingBigModel);
    }

    // Set results
    iReturnFileId = ((char*)pImgGTAInfo - (char*)CStreaming__ms_aInfoForModel) / 20;
    pReturnBuffer = replacementFileData;

    // Update flags
    pImgGTAInfo->uiLoadflag = 3;

    // Remove priorty flag, as not counted in ms_numPriorityRequests
    pImgGTAInfo->uiUnknown2 &= ~0x10;

    return true;
}

// Hook info
#define HOOKPOS_CStreaming_RequestModel_Mid  0x040895A
#define HOOKSIZE_CStreaming_RequestModel_Mid 5
DWORD                         RETURN_CStreaming_RequestModel_MidA = 0x0408960;
DWORD                         RETURN_CStreaming_RequestModel_MidB = 0x0408990;
static void __declspec(naked) HOOK_CStreaming_RequestModel_Mid()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    esi
        push    ebx
        call    OnCStreaming_RequestModel_Mid
        add     esp, 4*2
        cmp     al, 0
        jnz     skip

        // Continue with standard code
        popad
        mov     eax, ds:0x08E4C58
        push    eax
        jmp     RETURN_CStreaming_RequestModel_MidA

        // Handle load here
        skip:
        mov     eax, 0
        push    eax
        mov     eax, iReturnFileId
        push    eax
        mov     eax, pReturnBuffer
        push    eax
        call    FUNC_CStreamingConvertBufferToObject
        add     esp, 4*3

        popad
        jmp     RETURN_CStreaming_RequestModel_MidB
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CRenderWareSA::StaticSetClothesReplacingHooks()
{
    EZHookInstall(CStreaming_RequestModel_Mid);
}
