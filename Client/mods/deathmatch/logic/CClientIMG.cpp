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
    m_ucStreamID = -1;
    SetTypeName("img");
}

CClientIMG::~CClientIMG()
{
    if (m_ucStreamID != -1)
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

    m_pRestoreData.resize(m_uiFilesCount);
    m_ucStreamID = g_pGame->GetStreaming()->AddStreamHandler(*m_strFilename);
    return m_ucStreamID != -1;
}

bool CClientIMG::StreamDisable()
{
    if (m_ucStreamID == -1)
        return false;

    for (unsigned int i = 0; i < m_pRestoreData.size(); i++ )
    {
        tLinkedModelRestoreInfo* pRestoreData = &m_pRestoreData[i];
        if (pRestoreData->uiOffset)
            g_pGame->GetStreaming()->SetModelStreamInfo(pRestoreData->uiModelID, pRestoreData->ucStreamID, pRestoreData->uiOffset, pRestoreData->usSize);
    }

    m_pRestoreData.clear();
    return true;
}

bool CClientIMG::LinkModel(unsigned int usModelID, unsigned int uiFileID )
{
    if (m_ucStreamID == -1)
        return false;

    if (uiFileID >= m_uiFilesCount)
        return false;

    tImgFileInfo* pFileInfo = GetFileInfo(uiFileID);

    g_pGame->GetStreaming()->SetModelStreamInfo(usModelID, m_ucStreamID, pFileInfo->uiOffset, pFileInfo->usSize);
    return true;
}

bool CClientIMG::UnlinkModel(unsigned int uiModelID)
{
    for (unsigned int i = 0; i < m_pRestoreData.size(); i++)
    {
        if (m_pRestoreData[i].uiModelID == uiModelID)
        {
            g_pGame->GetStreaming()->SetModelStreamInfo(uiModelID, m_pRestoreData[i].ucStreamID, m_pRestoreData[i].uiOffset, m_pRestoreData[i].usSize);
            return true;
        }
    }

    return false;
}
