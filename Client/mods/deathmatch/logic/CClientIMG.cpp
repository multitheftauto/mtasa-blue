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
    m_pContentInfo = NULL;
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

    m_strFilename = std::move(sFilePath);

/*    g_pClientGame->GetResourceManager()->ValidateResourceFile(m_strFilename, nullptr, 0);
    if (!g_pCore->GetNetwork()->CheckFile("img", m_strFilename))
        return false;
*/

    tImgHeadrer fileHeadrer;

    SString buffer;

    FileLoad(m_strFilename, buffer, sizeof(tImgHeadrer), 0);

    if (fileHeadrer.szMagic != "VER2")
        return false;

    m_uiFilesCount = fileHeadrer.uiFilesCount;

    m_pContentInfo = new tImgFileInfo[m_uiFilesCount];

    FileLoad(m_strFilename, *(SString*)m_pContentInfo, m_uiFilesCount * sizeof(tImgFileInfo), 8);

    Stream();
    return true;
}

tImgFileInfo* CClientIMG::GetFileInfo(unsigned short usFileID)
{
    return &m_pContentInfo[usFileID];
}

bool CClientIMG::Stream()
{
    m_ucStreamID = g_pGame->GetStreaming()->AddStreamHandler(*m_strFilename);
    return true;
}

bool CClientIMG::LinkModel(unsigned short usModelID, unsigned short usFileID )
{
    if (!m_ucStreamID)
        return false;

    tImgFileInfo* pFileInfo = GetFileInfo(usFileID);

    g_pGame->GetStreaming()->SetModelStreamInfo(usModelID, m_ucStreamID, pFileInfo->uiOffset, pFileInfo->usSize);
    return true;
}
