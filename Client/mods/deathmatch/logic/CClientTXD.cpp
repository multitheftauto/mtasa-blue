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
            return false;            // Raw data has been freed already because texture was first used as non-clothes

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
