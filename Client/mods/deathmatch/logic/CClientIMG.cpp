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

#define INVALID_ARCHIVE_ID 0xFF

struct tImgHeader
{
    char         szMagic[4];
    unsigned int uiFilesCount;
};

CClientIMG::CClientIMG(class CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_uiFilesCount = 0;
    m_pManager = pManager;
    m_ucArchiveID = INVALID_ARCHIVE_ID;
    m_pFile = nullptr;
    SetTypeName("img");

    m_pImgManager = pManager->GetIMGManager();

    m_pImgManager->AddToList(this);
}

CClientIMG::~CClientIMG()
{
    m_pImgManager->RemoveFromList(this);

    if (IsStreamed())
        StreamDisable();

    Unload();
}

bool CClientIMG::Load(SString sFilePath)
{
    if (sFilePath.empty())
        return false;

    if (!FileExists(sFilePath))
        return false;

    if (!m_pContentInfo.empty())
        return false;

    m_strFilename = std::move(sFilePath);

/*    g_pClientGame->GetResourceManager()->ValidateResourceFile(m_strFilename, nullptr, 0);
    if (!g_pCore->GetNetwork()->CheckFile("img", m_strFilename))
        return false;
*/

    // Open the file
    m_pFile = File::Fopen(m_strFilename, "rb");
    if (!m_pFile)
        return false;

    tImgHeader fileHeader;

    // Read header
    int iReadCount = fread(&fileHeader, sizeof(fileHeader), 1, m_pFile);

    if (!iReadCount || memcmp(&fileHeader.szMagic, "VER2", 4))
    {
        fclose(m_pFile);
        return false;
    }

    m_uiFilesCount = fileHeader.uiFilesCount;

    // Read content info

    m_pContentInfo.resize(m_uiFilesCount);

    iReadCount = fread(&m_pContentInfo.at(0), sizeof(tImgFileInfo), m_uiFilesCount, m_pFile);
    
    if (iReadCount != m_uiFilesCount)
    {
        Unload();
        return false;
    }

    return true;
}

void CClientIMG::Unload()
{
    m_pContentInfo.clear();
    m_uiFilesCount = 0;

    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = nullptr;
    }
}

long CClientIMG::GetFile(unsigned int uiFileID, SString& buffer)
{
    tImgFileInfo* pFileInfo = GetFileInfo(uiFileID);
    if (!pFileInfo)
        return -1;

    unsigned long ulReadCount = pFileInfo->usSize * 2048;

    try
    {
        buffer.resize(ulReadCount);
    }
    catch (const std::bad_alloc&)
    {
        return -2;
    }

    long iReadCount = fread(buffer.data(), 1, ulReadCount, m_pFile);

    return iReadCount;
}

tImgFileInfo* CClientIMG::GetFileInfo(unsigned int usFileID)
{
    if (usFileID > m_uiFilesCount)
        return NULL;
    return &m_pContentInfo[usFileID];
}

unsigned int CClientIMG::GetFileID(SString strFileName)
{
    strFileName.resize(24);
    for (unsigned int i = 0; i < m_uiFilesCount; i++)
    {
        if (strFileName.EqualsI(m_pContentInfo[i].szFileName))
            return i;
    }
    return -1;
}

bool CClientIMG::IsStreamed()
{
    return m_ucArchiveID != INVALID_ARCHIVE_ID;
}

bool CClientIMG::StreamEnable()
{
    if (!m_uiFilesCount)
        return false;

    if (IsStreamed())
        return false;

    m_pRestoreData.reserve(m_uiFilesCount);
    m_ucArchiveID = g_pGame->GetStreaming()->AddArchive(*m_strFilename);
    return IsStreamed();
}

bool CClientIMG::StreamDisable()
{
    if (!IsStreamed())
        return false;

    for (unsigned int i = 0; i < m_pRestoreData.size(); i++ )
    {
        tLinkedModelRestoreInfo* pRestoreData = m_pRestoreData[i];
        if (pRestoreData)
        {
            g_pGame->GetStreaming()->SetStreamingInfoForModelId(pRestoreData->uiModelID, pRestoreData->ucStreamID, pRestoreData->uiOffset,
                                                                pRestoreData->usSize);
            delete pRestoreData;
        }
    }

    m_pRestoreData.clear();

    g_pGame->GetStreaming()->RemoveArchive(m_ucArchiveID);
    m_ucArchiveID = INVALID_ARCHIVE_ID;
    return true;
}

bool CClientIMG::LinkModel(unsigned int uiModelID, unsigned int uiFileID)
{
    if (!IsStreamed())
        return false;

    if (uiFileID >= m_uiFilesCount)
        return false;

    tImgFileInfo* pFileInfo = GetFileInfo(uiFileID);

    CStreamingInfo* pCurrentStreamingInfo = g_pGame->GetStreaming()->GetStreamingInfoFromModelId(uiModelID);

    tLinkedModelRestoreInfo* pModelRestoreData = new tLinkedModelRestoreInfo();
    pModelRestoreData->uiModelID = uiModelID;
    pModelRestoreData->ucStreamID = pCurrentStreamingInfo->archiveId;
    pModelRestoreData->uiOffset = pCurrentStreamingInfo->offsetInBlocks;
    pModelRestoreData->usSize = pCurrentStreamingInfo->sizeInBlocks;

    m_pRestoreData.push_back(pModelRestoreData);

    g_pGame->GetStreaming()->SetStreamingInfoForModelId(uiModelID, m_ucArchiveID, pFileInfo->uiOffset, pFileInfo->usSize);
    return true;
}

bool CClientIMG::UnlinkModel(unsigned int uiModelID)
{
    for (unsigned int i = 0; i < m_pRestoreData.size(); i++)
    {
        if (m_pRestoreData[i]->uiModelID == uiModelID)
        {
            tLinkedModelRestoreInfo* pRestoreData = m_pRestoreData[i];
            g_pGame->GetStreaming()->SetStreamingInfoForModelId(uiModelID, pRestoreData->ucStreamID, pRestoreData->uiOffset, pRestoreData->usSize);
            delete pRestoreData;
            m_pRestoreData.erase(m_pRestoreData.begin() + i);
            return true;
        }
    }

    return false;
}
