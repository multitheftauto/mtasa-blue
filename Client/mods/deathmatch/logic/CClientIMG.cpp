/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientIMG.cpp
 *  PURPOSE:     IMG manager class
 *
 *****************************************************************************/

#include <StdInc.h>

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
    m_ucArchiveID = -1;
    SetTypeName("img");
}

CClientIMG::~CClientIMG()
{
    if (m_ucArchiveID != -1)
        StreamDisable();
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
    FILE* pFile = File::Fopen(m_strFilename, "rb");
    if (!pFile)
        return false;

    tImgHeader filerHeader;

    // Read header
    int iReadCount = fread(&filerHeader, sizeof(filerHeader), 1, pFile);

    if (!iReadCount || memcmp(&filerHeader.szMagic, "VER2", 4))
    {
        fclose(pFile);
        return false;
    }

    m_uiFilesCount = filerHeader.uiFilesCount;

    // Read content info

    m_pContentInfo.resize(m_uiFilesCount);

    iReadCount = fread(&m_pContentInfo.at(0), sizeof(tImgFileInfo), m_uiFilesCount, pFile);
    
    if (iReadCount != m_uiFilesCount)
    {
        fclose(pFile);
        m_pContentInfo.clear();
        return false;
    }

    //Stream();

    fclose(pFile);
    return true;
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
        if (strFileName.compare(m_pContentInfo[i].szFileName))
            return i;
    }
    return -1;
}

bool CClientIMG::StreamEnable()
{
    if (!m_uiFilesCount)
        return false;

    m_pRestoreData.reserve(m_uiFilesCount);
    m_ucArchiveID = g_pGame->GetStreaming()->AddStreamHandler(*m_strFilename);
    return m_ucArchiveID != -1;
}

bool CClientIMG::StreamDisable()
{
    if (m_ucArchiveID == -1)
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

    g_pGame->GetStreaming()->RemoveStreamHandler(m_ucArchiveID);
    m_ucArchiveID = -1;
    return true;
}

bool CClientIMG::LinkModel(unsigned int uiModelID, unsigned int uiFileID)
{
    if (m_ucArchiveID == -1)
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
