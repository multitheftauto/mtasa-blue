/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTXD.cpp
 *  PURPOSE:     TXD manager class
 *
 *****************************************************************************/

#include <StdInc.h>

CClientTXD::CClientTXD(class CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_pManager = pManager;
    SetTypeName("txd");
}

CClientTXD::~CClientTXD()
{
    if (!g_pGame || !g_pGame->GetRenderWare())
    {
        // RenderWare already destroyed (SA is unpredictable)
        return;
    }

    const auto usedTxdIdsSnapshot = m_ReplacementTextures.usedInTxdIds;

    g_pGame->GetRenderWare()->ModelInfoTXDRemoveTextures(&m_ReplacementTextures);

    // Restream affected models
    std::vector<unsigned short> restreamModelIds;
    restreamModelIds.reserve(m_ReplacementTextures.usedInModelIds.size());
    for (unsigned short modelId : m_ReplacementTextures.usedInModelIds)
        restreamModelIds.push_back(modelId);

    for (unsigned short modelId : restreamModelIds)
    {
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(modelId, true);
        if (!pModelInfo || !pModelInfo->IsValid())
            continue;

        // Only restream if model uses a TXD that was affected
        if (!usedTxdIdsSnapshot.empty() && usedTxdIdsSnapshot.count(pModelInfo->GetTextureDictionaryID()) == 0)
            continue;
        Restream(modelId);
    }

    // Remove us from all the clothes replacement doo dah
    g_pGame->GetRenderWare()->ClothesRemoveReplacement(m_FileData.data());

    // Remove us from all the clothes
    g_pGame->GetRenderWare()->ClothesRemoveFile(m_FileData.data());
}

bool CClientTXD::Load(bool isRaw, SString input, bool enableFiltering)
{
    if (input.empty())
        return false;

    m_bIsRawData = isRaw;
    m_bFilteringEnabled = enableFiltering;

    if (isRaw)
    {
        return LoadFromBuffer(std::move(input));
    }
    else
    {
        return LoadFromFile(std::move(input));
    }
}

bool CClientTXD::AddClothingTexture(const std::string& modelName)
{
    m_strLastError.clear();

    if (modelName.empty())
    {
        m_strLastError = "Model name is empty";
        return false;
    }

    if (m_FileData.empty() && m_bIsRawData)
    {
        m_strLastError = "Raw data buffer unavailable (texture was first used for non-clothes model)";
        return false;
    }

    if (m_FileData.empty())
    {
        SString strUseFilename;
        if (!GetFilenameToUse(strUseFilename))
            return false;
        if (!FileLoad(std::nothrow, strUseFilename, m_FileData))
        {
            m_strLastError = SString("Failed to read file: %s", ExtractFilename(strUseFilename).c_str());
            return false;
        }
    }

    if (!g_pGame->GetRenderWare()->ClothesAddFile(m_FileData.data(), m_FileData.size(), modelName.c_str()))
    {
        m_strLastError = SString("Failed to add clothing texture: %s", modelName.c_str());
        return false;
    }

    return true;
}

bool CClientTXD::Import(unsigned short usModelID)
{
    m_strLastError.clear();

    if (usModelID >= CLOTHES_TEX_ID_FIRST && usModelID <= CLOTHES_TEX_ID_LAST)
    {
        if (m_FileData.empty() && m_bIsRawData)
        {
            m_strLastError = "Raw data buffer unavailable (texture was first used for non-clothes model)";
            return false;
        }

        // If using for clothes only, unload 'replacing model textures' stuff to save memory
        if (!m_ReplacementTextures.textures.empty() && m_ReplacementTextures.usedInModelIds.empty())
        {
            g_pGame->GetRenderWare()->ModelInfoTXDRemoveTextures(&m_ReplacementTextures);
            m_ReplacementTextures = SReplacementTextures();
        }
        // Load txd file data if not already done
        if (m_FileData.empty())
        {
            SString strUseFilename;

            if (!GetFilenameToUse(strUseFilename))
            {
                if (m_strLastError.empty())
                    m_strLastError = SString("Cannot access file: %s", ExtractFilename(m_strFilename).c_str());
                return false;
            }

            if (!FileLoad(std::nothrow, strUseFilename, m_FileData))
            {
                m_strLastError = SString("Failed to read file: %s", ExtractFilename(strUseFilename).c_str());
                return false;
            }
        }
        m_bUsingFileDataForClothes = true;
        // Note: ClothesAddReplacement uses the pointer from m_FileData, so don't touch m_FileData until matching ClothesRemove call
        g_pGame->GetRenderWare()->ClothesAddReplacement(m_FileData.data(), m_FileData.size(), usModelID - CLOTHES_MODEL_ID_FIRST);
        return true;
    }
    else
    {
        // Ensure loaded for replacing model textures
        if (m_ReplacementTextures.textures.empty())
        {
            if (!m_bIsRawData)
            {
                SString strUseFilename;
                if (!GetFilenameToUse(strUseFilename))
                {
                    if (m_strLastError.empty())
                        m_strLastError = SString("Cannot access file for model %d: %s", usModelID, ExtractFilename(m_strFilename).c_str());
                    return false;
                }

                if (!g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, strUseFilename, SString(), m_bFilteringEnabled, &m_strLastError))
                {
                    if (m_strLastError.empty())
                        m_strLastError = SString("Failed to load textures for model %d: %s", usModelID, ExtractFilename(strUseFilename).c_str());
                    return false;
                }
            }
            else
            {
                if (!g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, SString(), m_FileData, m_bFilteringEnabled, &m_strLastError))
                {
                    if (m_strLastError.empty())
                        m_strLastError = SString("Failed to load textures for model %d from buffer", usModelID);
                    return false;
                }
            }
        }

        // If raw data and not used as clothes textures yet, then free raw data buffer to save RAM
        if (m_bIsRawData && !m_bUsingFileDataForClothes)
        {
            // This means the texture can't be used for clothes now
            SString().swap(m_FileData);
        }

        if (g_pGame->GetRenderWare()->ModelInfoTXDAddTextures(&m_ReplacementTextures, usModelID))
        {
            Restream(usModelID);
            return true;
        }

        if (m_strLastError.empty())
            m_strLastError = SString("Cannot apply textures to model %d (already applied or invalid model)", usModelID);
    }

    return false;
}

bool CClientTXD::IsImportableModel(unsigned short usModelID)
{
    // Currently we work on vehicles and objects
    return CClientObjectManager::IsValidModel(usModelID) || CClientVehicleManager::IsValidModel(usModelID) || CClientPlayerManager::IsValidModel(usModelID) ||
           (usModelID >= CLOTHES_TEX_ID_FIRST && usModelID <= CLOTHES_TEX_ID_LAST);
}

bool CClientTXD::LoadFromFile(SString filePath)
{
    m_strFilename = std::move(filePath);
    m_strLastError.clear();

    SString strUseFilename;

    if (!GetFilenameToUse(strUseFilename))
    {
        if (m_strLastError.empty())
            m_strLastError = SString("Invalid or inaccessible file: %s", ExtractFilename(m_strFilename).c_str());
        return false;
    }

    const bool ok = g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, strUseFilename, SString(), m_bFilteringEnabled, &m_strLastError);
    if (!ok && m_strLastError.empty())
        m_strLastError = SString("Failed to load TXD: %s", ExtractFilename(m_strFilename).c_str());

    return ok;
}

bool CClientTXD::LoadFromBuffer(SString buffer)
{
    m_strLastError.clear();

    if (!g_pCore->GetNetwork()->CheckFile("txd", "", buffer.data(), buffer.size()))
    {
        m_strLastError = SString("TXD data rejected as invalid (%u bytes)", static_cast<unsigned>(buffer.size()));
        return false;
    }

    m_FileData = std::move(buffer);

    const bool ok = g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, SString(), m_FileData, m_bFilteringEnabled, &m_strLastError);
    if (!ok && m_strLastError.empty())
        m_strLastError = SString("Failed to load TXD from buffer (%u bytes)", static_cast<unsigned>(m_FileData.size()));

    return ok;
}

void CClientTXD::Restream(unsigned short usModelID)
{
    if (CClientVehicleManager::IsValidModel(usModelID))
    {
        m_pManager->GetVehicleManager()->RestreamVehicles(usModelID);
    }
    else if (CClientObjectManager::IsValidModel(usModelID))
    {
        if (CClientPedManager::IsValidWeaponModel(usModelID))
        {
            m_pManager->GetPedManager()->RestreamWeapon(usModelID);
            m_pManager->GetPickupManager()->RestreamPickups(usModelID);
        }
        m_pManager->GetObjectManager()->RestreamObjects(usModelID);
        g_pGame->GetModelInfo(usModelID)->RestreamIPL();
    }
    else if (CClientPlayerManager::IsValidModel(usModelID))
    {
        m_pManager->GetPedManager()->RestreamPeds(usModelID);
    }
}

bool CClientTXD::GetFilenameToUse(SString& strOutFilename)
{
    if (!FileExists(m_strFilename))
    {
        m_strLastError = SString("File not found: %s", ExtractFilename(m_strFilename).c_str());
        return false;
    }

    CDownloadableResource* pResFile = nullptr;
    bool                   bChecksumAlreadyValidated = false;

    static const CChecksum  zeroChecksum;

    CChecksum               serverChecksum;
    bool                   bServerHasChecksum = false;
    long long              cachedFileSize = -1;

    if (g_pClientGame)
    {
        if (auto* pResMgr = g_pClientGame->GetResourceManager())
        {
            SString strLookup = PathConform(m_strFilename).ToLower();
            pResFile = pResMgr->GetDownloadableResourceFile(strLookup);

            if (pResFile)
            {
                serverChecksum = pResFile->GetServerChecksum();
                bServerHasChecksum = (serverChecksum != zeroChecksum);
            }
        }
    }

    if (pResFile && pResFile->IsAutoDownload() && !pResFile->IsDownloaded())
    {
        const long long expectedSize = static_cast<long long>(pResFile->GetDownloadSize());

        if (expectedSize > 0)
        {
            cachedFileSize = static_cast<long long>(FileSize(m_strFilename));
            if (cachedFileSize < 0)
            {
                m_strLastError = SString("Cannot read file: %s (download may be in progress)", ExtractFilename(m_strFilename).c_str());
                return false;
            }
            if (cachedFileSize != expectedSize)
            {
                m_strLastError = SString("Download incomplete: %s (got %lld of %lld bytes)", ExtractFilename(m_strFilename).c_str(), cachedFileSize, expectedSize);
                return false;
            }
        }

        if (bServerHasChecksum)
        {
            const CChecksum clientChecksum = CChecksum::GenerateChecksumFromFileUnsafe(m_strFilename);
            if (clientChecksum == zeroChecksum)
            {
                m_strLastError = SString("Download incomplete: %s (checksum unavailable)", ExtractFilename(m_strFilename).c_str());
                return false;
            }

            if (clientChecksum == serverChecksum)
            {
                pResFile->SetDownloaded();
                bChecksumAlreadyValidated = true;
            }
            else
            {
                m_strLastError = SString("Download incomplete: %s (checksum mismatch during transfer)", ExtractFilename(m_strFilename).c_str());
                return false;
            }
        }
        else
        {
            m_strLastError = SString("Download incomplete: %s (awaiting server verification)", ExtractFilename(m_strFilename).c_str());
            return false;
        }
    }

    if (g_pClientGame && g_pClientGame->GetResourceManager())
        g_pClientGame->GetResourceManager()->ValidateResourceFile(m_strFilename, nullptr, 0);

    // Validate downloaded resources
    if (pResFile)
    {
        const long long expectedSize = static_cast<long long>(pResFile->GetDownloadSize());
        
        // Cache file size for reuse (also used in RightSizeTxd logging)
        if (cachedFileSize < 0)
            cachedFileSize = static_cast<long long>(FileSize(m_strFilename));

        if (cachedFileSize < 0)
        {
            m_strLastError = SString("Cannot read file: %s", ExtractFilename(m_strFilename).c_str());
            return false;
        }

        if (expectedSize > 0 && cachedFileSize != expectedSize)
        {
            m_strLastError = SString("Size mismatch: %s (expected %lld, got %lld bytes)", ExtractFilename(m_strFilename).c_str(), expectedSize, cachedFileSize);
            return false;
        }

        // Only validate checksum if server provided it AND we haven't already validated it
        // (serverChecksum and bServerHasChecksum already cached above)

        if (bServerHasChecksum && !bChecksumAlreadyValidated)
        {
            const CChecksum clientChecksum = CChecksum::GenerateChecksumFromFileUnsafe(m_strFilename);

            if (clientChecksum == zeroChecksum)
            {
                m_strLastError = SString("Cannot verify file: %s (checksum computation failed)", ExtractFilename(m_strFilename).c_str());
                return false;
            }

            if (clientChecksum != serverChecksum)
            {
                char szMd5Got[33];
                CMD5Hasher::ConvertToHex(clientChecksum.md5, szMd5Got);
                m_strLastError = SString("Checksum mismatch: %s (file hash: %.8s...)", ExtractFilename(m_strFilename).c_str(), szMd5Got);
                return false;
            }
        }
    }

    if (!g_pCore->GetNetwork()->CheckFile("txd", m_strFilename))
    {
        m_strLastError = SString("File rejected as invalid: %s", ExtractFilename(m_strFilename).c_str());
        return false;
    }

    // Default: use original data
    strOutFilename = m_strFilename;

    // Should we try to reduce the size of this txd?
    if (g_pCore->GetRightSizeTxdEnabled() && g_pGame && g_pGame->GetRenderWare())
    {
        // See if previously shrunk result exists
        SString strLargeSha256 = GenerateSha256HexStringFromFile(m_strFilename);
        SString strShrunkFilename = PathJoin(ExtractPath(m_strFilename), SString("_3_%s", *strLargeSha256.Left(32)));
        uint    uiShrunkSize = (uint)FileSize(strShrunkFilename);
        if (uiShrunkSize >= 128)
        {
            // Read cksum from the end
            SString strSmallSha256Check;
            FileLoad(strShrunkFilename, strSmallSha256Check, 64, uiShrunkSize - 64);

            // Check cksum
            SString strSmallSha256 = GenerateHashHexStringFromFile(EHashFunction::SHA256, strShrunkFilename, uiShrunkSize - 64);
            if (strSmallSha256Check == strSmallSha256)
            {
                // Have valid previous shrunk result?
                SString headBytes;
                FileLoad(strShrunkFilename, headBytes, 33);
                if (IsTXDData(headBytes))
                {
                    // Result is: use shrunk data
                    strOutFilename = strShrunkFilename;
                }
                else
                {
                    // Result is: keep original data
                }
                return true;
            }
        }

        // See if txd should be shrunk
        if (g_pGame->GetRenderWare()->RightSizeTxd(m_strFilename, strShrunkFilename, 256))
        {
            // Yes
            strOutFilename = strShrunkFilename;
            FileAppend(strShrunkFilename, SStringX("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 12));
            FileAppend(strShrunkFilename, GenerateSha256HexStringFromFile(strShrunkFilename));
            // Use cached file size if available, otherwise compute it
            const long long originalSizeKB = (cachedFileSize >= 0 ? cachedFileSize : FileSize(m_strFilename)) / 1024;
            AddReportLog(9400, SString("RightSized %s(%s) from %d KB => %d KB", *ExtractFilename(m_strFilename), *strLargeSha256.Left(8),
                                       (uint)originalSizeKB, (uint)(FileSize(strShrunkFilename) / 1024)));
        }
        else
        {
            // No
            // Indicate for next time
            FileSave(strShrunkFilename, strLargeSha256);
            FileAppend(strShrunkFilename, GenerateSha256HexStringFromFile(strShrunkFilename));
        }
    }

    return true;
}

// Return true if data looks like TXD file contents
bool CClientTXD::IsTXDData(const SString& strData)
{
    return strData.length() > 32 && memcmp(strData, "\x16\x00\x00\x00", 4) == 0;
}
