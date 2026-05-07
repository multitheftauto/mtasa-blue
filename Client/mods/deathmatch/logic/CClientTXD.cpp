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

    // Subscribe so the renderware module can re-decode our replacement textures
    // after a D3D9 device reset (DEFAULT-pool textures are auto-destroyed on Reset).
    g_pGame->GetRenderWare()->RegisterReplacementOwner(this);
}

CClientTXD::~CClientTXD()
{
    g_pGame->GetRenderWare()->UnregisterReplacementOwner(this);

    // Remove us from all the models
    g_pGame->GetRenderWare()->ModelInfoTXDRemoveTextures(&m_ReplacementTextures);

    // Restream affected models
    for (uint i = 0; i < m_ReplacementTextures.usedInModelIds.size(); i++)
    {
        Restream(m_ReplacementTextures.usedInModelIds[i]);
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
    if (usModelID >= CLOTHES_TEX_ID_FIRST && usModelID <= CLOTHES_TEX_ID_LAST)
    {
        if (m_FileData.empty() && m_bIsRawData)
            return false;  // Raw data has been freed already because texture was first used as non-clothes

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
                return false;

            if (!FileLoad(std::nothrow, strUseFilename, m_FileData))
                return false;
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
                    return false;
                g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, strUseFilename, SString(), m_bFilteringEnabled);
                if (m_ReplacementTextures.textures.empty())
                    return false;
            }
            else
            {
                g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, NULL, m_FileData, m_bFilteringEnabled);
                if (m_ReplacementTextures.textures.empty())
                    return false;
            }
        }

        // The replacement textures live in D3DPOOL_DEFAULT after the conversion done by
        // CRenderWareSA::ReadTXD (issue #4062). DEFAULT-pool resources are auto-destroyed
        // on a D3D9 device reset, so we need the original TXD bytes to re-decode them in
        // CRenderWareSA::OnDeviceReset:
        //   - file-path TXDs re-read from disk via GetFilenameToUse,
        //   - raw-data TXDs use the m_FileData buffer kept since LoadFromBuffer.
        // We deliberately don't drop m_FileData here for raw-data TXDs (master used to)
        // because the system-memory cost is the same as the MANAGED shadow we saved by
        // converting the textures, and keeping it lets us recover from any device reset.

        // Have we got textures and haven't already imported into this model?
        if (g_pGame->GetRenderWare()->ModelInfoTXDAddTextures(&m_ReplacementTextures, usModelID))
        {
            Restream(usModelID);
            return true;
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

    SString strUseFilename;

    if (!GetFilenameToUse(strUseFilename))
        return false;

    return g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, strUseFilename, SString(), m_bFilteringEnabled);
}

bool CClientTXD::LoadFromBuffer(SString buffer)
{
    if (!g_pCore->GetNetwork()->CheckFile("txd", "", buffer.data(), buffer.size()))
        return false;

    m_FileData = std::move(buffer);

    return g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, NULL, m_FileData, m_bFilteringEnabled);
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
    g_pClientGame->GetResourceManager()->ValidateResourceFile(m_strFilename, nullptr, 0);
    if (!g_pCore->GetNetwork()->CheckFile("txd", m_strFilename))
        return false;

    // Default: use original data
    strOutFilename = m_strFilename;

    // Should we try to reduce the size of this txd?
    if (g_pCore->GetRightSizeTxdEnabled())
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
            AddReportLog(9400, SString("RightSized %s(%s) from %d KB => %d KB", *ExtractFilename(m_strFilename), *strLargeSha256.Left(8),
                                       (uint)FileSize(m_strFilename) / 1024, (uint)FileSize(strShrunkFilename) / 1024));
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

bool CClientTXD::RebuildReplacementsAfterDeviceReset()
{
    // CRenderWareSA::OnDeviceLost has already released our IDirect3DTexture9 instances
    // (DEFAULT pool) and NULLed rasterExt->texture. The RwTextures themselves are still
    // alive inside their respective GTA TXDs, but they now wrap rasters with no backing
    // D3D resource, so we must tear them down before re-decoding fresh ones.

    // Snapshot the set of models we were applied to so we can replay the imports.
    std::vector<unsigned short> savedModelIds = m_ReplacementTextures.usedInModelIds;

    // Walk the full perTxdList: removes our textures from each GTA TXD (originals come
    // back), destroys the cloned copies, and cascades through DestroyTexture for the
    // originals (which calls ReleaseTrackedDefaultPoolTexture, a no-op in this state
    // because the D3D pointer was already cleared in OnDeviceLost).
    g_pGame->GetRenderWare()->ModelInfoTXDRemoveTextures(&m_ReplacementTextures);
    m_ReplacementTextures = SReplacementTextures();

    // Buffer-path TXDs whose source bytes were freed cannot be rebuilt; the
    // replacements stay gone for the rest of the session and the script must
    // re-call engineLoadTXD to recover.
    if (m_bIsRawData && m_FileData.empty())
        return false;

    bool bLoaded = false;
    if (m_bIsRawData)
    {
        bLoaded = g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, NULL, m_FileData, m_bFilteringEnabled);
    }
    else
    {
        SString strUseFilename;
        if (!GetFilenameToUse(strUseFilename))
            return false;
        bLoaded = g_pGame->GetRenderWare()->ModelInfoTXDLoadTextures(&m_ReplacementTextures, strUseFilename, SString(), m_bFilteringEnabled);
    }

    if (!bLoaded || m_ReplacementTextures.textures.empty())
        return false;

    // Re-apply to every model the script had previously imported this TXD onto.
    for (unsigned short modelId : savedModelIds)
    {
        if (g_pGame->GetRenderWare()->ModelInfoTXDAddTextures(&m_ReplacementTextures, modelId))
            Restream(modelId);
    }

    return true;
}
