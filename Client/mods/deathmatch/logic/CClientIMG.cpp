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
    if (m_ucStreamID)
        Unlink();
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

bool CClientIMG::Stream()
{
    m_ucStreamID = g_pGame->GetStreaming()->AddStreamHandler(*m_strFilename);
    return true;
}

bool CClientIMG::LinkModel(unsigned short usModelID, unsigned int uiFileID )
{
    if (!m_ucStreamID)
        return false;

    if (uiFileID >= m_uiFilesCount)
        return false;

    tImgFileInfo* pFileInfo = GetFileInfo(uiFileID);

    g_pGame->GetStreaming()->SetModelStreamInfo(usModelID, m_ucStreamID, pFileInfo->uiOffset, pFileInfo->usSize);
    return true;
}
