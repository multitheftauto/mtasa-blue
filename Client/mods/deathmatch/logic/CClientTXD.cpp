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
    if (modelName.empty())
        return false;

    if (m_FileData.empty() && m_bIsRawData)
        return false;

    if (m_FileData.empty())
    {
        SString strUseFilename;
        if (!GetFilenameToUse(strUseFilename))
            return false;
        if (!FileLoad(std::nothrow, strUseFilename, m_FileData))
            return false;
    }

    return g_pGame->GetRenderWare()->ClothesAddFile(m_FileData.data(), m_FileData.size(), modelName.c_str());
}

bool CClientTXD::Import(unsigned short usModelID)
{
    m_strLastError.clear();

    if (usModelID >= CLOTHES_TEX_ID_FIRST && usModelID <= CLOTHES_TEX_ID_LAST)
    {
        if (m_FileData.empty() && m_bIsRawData)
        {
            m_strLastError = "[Import:Clothes] Raw data buffer was already freed (texture was first used for non-clothes model)";
            AddReportLog(9401, m_strLastError);
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
                    m_strLastError = "[Import:Clothes] Invalid or inaccessible file path";
                AddReportLog(9401, m_strLastError);
                return false;
            }

            if (!FileLoad(std::nothrow, strUseFilename, m_FileData))
            {
                m_strLastError = "[Import:Clothes] Failed to read TXD file from disk";
                AddReportLog(9401, m_strLastError);
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
                        m_strLastError = "[Import:Model] Invalid or inaccessible file path";
                    AddReportLog(9401, SString("[CClientTXD::Import] GetFilenameToUse failed for model %d: %s", usModelID, m_strLastError.c_str()));
                    return false;
                }
                if (!g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, strUseFilename, SString(), m_bFilteringEnabled, &m_strLastError))
                {
                    AddReportLog(9401, SString("[CClientTXD::Import] ModelInfoTXDLoadTextures failed for model %d: %s", usModelID, m_strLastError.c_str()));
                    return false;
                }
            }
            else
            {
                if (!g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, SString(), m_FileData, m_bFilteringEnabled, &m_strLastError))
                    return false;
            }
        }

        // If raw data and not used as clothes textures yet, then free raw data buffer to save RAM
        if (m_bIsRawData && !m_bUsingFileDataForClothes)
        {
            // This means the texture can't be used for clothes now
            SString().swap(m_FileData);
        }

        // Have we got textures and haven't already imported into this model?
        if (g_pGame->GetRenderWare()->ModelInfoTXDAddTextures(&m_ReplacementTextures, usModelID))
        {
            Restream(usModelID);
            return true;
        }
        else
        {
            // ModelInfoTXDAddTextures returns false if already imported or failed to get model info
            if (m_strLastError.empty())
                m_strLastError = "[Import:Model] Failed to apply textures (already applied to this model, or invalid model ID)";
            AddReportLog(9401, SString("[CClientTXD::Import] ModelInfoTXDAddTextures failed for model %d: %s", usModelID, m_strLastError.c_str()));
        }
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
            m_strLastError = "[LoadFromFile] Invalid or inaccessible file path";
        AddReportLog(9401, m_strLastError);
        return false;
    }

    return g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, strUseFilename, SString(), m_bFilteringEnabled, &m_strLastError);
}

bool CClientTXD::LoadFromBuffer(SString buffer)
{
    m_strLastError.clear();

    if (!g_pCore->GetNetwork()->CheckFile("txd", "", buffer.data(), buffer.size()))
    {
        m_strLastError = "[LoadFromBuffer] TXD data rejected as invalid";
        AddReportLog(9401, m_strLastError);
        return false;
    }

    m_FileData = std::move(buffer);

    return g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, SString(), m_FileData, m_bFilteringEnabled, &m_strLastError);
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

// Return filename to use, or false if not valid
bool CClientTXD::GetFilenameToUse(SString& strOutFilename)
{
    // Early out: file must exist
    if (!FileExists(m_strFilename))
    {
        m_strLastError = "[GetFilenameToUse] TXD file not yet available";
        return false;
    }

    CDownloadableResource* pResFile = nullptr;
    bool bChecksumAlreadyValidated = false;
    
    // Static zero checksum - only constructed once
    static const CChecksum zeroChecksum;
    
    // Cached values computed once when pResFile is available
    CChecksum serverChecksum;
    bool bServerHasChecksum = false;
    long long cachedFileSize = -1;

    // Block partial/downloading TXD files
    if (g_pClientGame)
    {
        if (auto* pResMgr = g_pClientGame->GetResourceManager())
        {
            SString strLookup = PathConform(m_strFilename).ToLower();
            pResFile = pResMgr->GetDownloadableResourceFile(strLookup);
            
            // Cache server checksum once for this pResFile
            if (pResFile)
            {
                serverChecksum = pResFile->GetServerChecksum();
                bServerHasChecksum = (serverChecksum != zeroChecksum);
            }
            
            if (pResFile && pResFile->IsAutoDownload() && !pResFile->IsDownloaded())
            {
                // File found in map, is auto-download, but not marked as downloaded yet.
                // This can happen when another resource tries to access a file before SetDownloaded() is called.
                // If the file exists and checksum matches server, it's actually ready to use.
                
                if (bServerHasChecksum)
                {
                    const CChecksum clientChecksum = CChecksum::GenerateChecksumFromFileUnsafe(m_strFilename);
                    
                    // Check if checksum generation failed (file locked, I/O error, etc.)
                    if (clientChecksum == zeroChecksum)
                    {
                        m_strLastError = "[GetFilenameToUse] TXD not yet downloaded (cannot read file)";
                        return false;
                    }
                    
                    if (clientChecksum == serverChecksum)
                    {
                        // File exists with correct checksum - it's ready to use!
                        // Mark it as downloaded to avoid repeated checksum calculations
                        pResFile->SetDownloaded();
                        bChecksumAlreadyValidated = true;
                    }
                    else
                    {
                        // File exists but checksum doesn't match - still downloading or corrupted
                        m_strLastError = "[GetFilenameToUse] TXD not yet downloaded (wait for onClientFileDownloadComplete)";
                        return false;
                    }
                }
                else
                {
                    // No server checksum to verify against - block until marked as downloaded
                    m_strLastError = "[GetFilenameToUse] TXD not yet downloaded (wait for onClientFileDownloadComplete)";
                    return false;
                }
            }
        }
    }

    if (g_pClientGame && g_pClientGame->GetResourceManager())
        g_pClientGame->GetResourceManager()->ValidateResourceFile(m_strFilename, nullptr, 0);

    // Validate downloaded resources
    if (pResFile)
    {
        const long long expectedSize = static_cast<long long>(pResFile->GetDownloadSize());
        
        // Cache file size for reuse (also used in RightSizeTxd logging)
        cachedFileSize = static_cast<long long>(FileSize(m_strFilename));

        // Fail if we can't read the file size
        if (cachedFileSize < 0)
        {
            AddReportLog(9402, SString("[CClientTXD::GetFilenameToUse] Cannot read file size for '%s'",
                m_strFilename.c_str()));
            m_strLastError = "[GetFilenameToUse] Cannot read TXD file";
            return false;
        }

        // Check size first (cheap) before computing checksum (expensive)
        if (expectedSize > 0 && cachedFileSize != expectedSize)
        {
            AddReportLog(9402, SString("[CClientTXD::GetFilenameToUse] Size mismatch for '%s': expected=%lld got=%lld",
                m_strFilename.c_str(), expectedSize, cachedFileSize));
            m_strLastError = "[GetFilenameToUse] TXD download size mismatch";
            return false;
        }

        // Only validate checksum if server provided it AND we haven't already validated it
        // (serverChecksum and bServerHasChecksum already cached above)

        if (bServerHasChecksum && !bChecksumAlreadyValidated)
        {
            const CChecksum clientChecksum = CChecksum::GenerateChecksumFromFileUnsafe(m_strFilename);

            // Check if checksum generation failed (returned zero checksum)
            if (clientChecksum == zeroChecksum)
            {
                AddReportLog(9402, SString("[CClientTXD::GetFilenameToUse] Failed to compute checksum for '%s'",
                    m_strFilename.c_str()));
                m_strLastError = "[GetFilenameToUse] Failed to compute TXD checksum";
                return false;
            }

            if (clientChecksum != serverChecksum)
            {
                char szMd5Expected[33];
                char szMd5Got[33];
                CMD5Hasher::ConvertToHex(serverChecksum.md5, szMd5Expected);
                CMD5Hasher::ConvertToHex(clientChecksum.md5, szMd5Got);

                AddReportLog(9402, SString("[CClientTXD::GetFilenameToUse] Checksum mismatch for '%s': expected=%s got=%s",
                    m_strFilename.c_str(), szMd5Expected, szMd5Got));
                m_strLastError = "[GetFilenameToUse] TXD download checksum mismatch";
                return false;
            }
        }
        else if (expectedSize <= 0 && !bServerHasChecksum)
        {
            // Server provided no size or checksum - log but allow (backwards compatibility)
            AddReportLog(9403, SString("[CClientTXD::GetFilenameToUse] No server validation data for '%s' - skipping integrity check",
                m_strFilename.c_str()));
        }
    }

    if (!g_pCore->GetNetwork()->CheckFile("txd", m_strFilename))
    {
        m_strLastError = "[GetFilenameToUse] TXD file rejected as invalid";
        AddReportLog(9401, m_strLastError);
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
