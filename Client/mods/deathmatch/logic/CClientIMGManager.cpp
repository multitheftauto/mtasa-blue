/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientIMGManager.cpp
 *  PURPOSE:     IMG container manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "game/CStreaming.h"

CClientIMGManager::CClientIMGManager(CClientManager* pManager)
{
    // Init
    m_bRemoveFromList = true;
}

CClientIMGManager::~CClientIMGManager()
{
    // Delete all our IMG's
    RemoveAll();
}

void CClientIMGManager::InitDefaultBufferSize()
{
    m_uiDefaultStreamerBufferSize = g_pGame->GetStreaming()->GetStreamingBufferSize();
}

CClientIMG* CClientIMGManager::GetElementFromArchiveID(unsigned char ucArchiveID)
{
    // By default GTA has 5 IMG's
    if (ucArchiveID < 6)
        return nullptr;

    std::list<CClientIMG*>::iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        CClientIMG* pIMG = *iter;
        if (ucArchiveID == pIMG->GetArchiveID())
        {
            return pIMG;
        }
    }

    return nullptr;
}

void CClientIMGManager::RemoveAll()
{
    // Make sure they don't remove themselves from our list
    m_bRemoveFromList = false;

    // Run through our list deleting the IMG's
    std::list<CClientIMG*>::iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        delete *iter;
    }

    // Allow list removal again
    m_bRemoveFromList = true;
}

bool CClientIMGManager::Exists(CClientIMG* pIMG)
{
    return std::find(m_List.begin(), m_List.end(), pIMG) != m_List.end();
}

CClientIMG* CClientIMGManager::GetElementThatLinked(unsigned int uiModel)
{
    unsigned char ucArhiveID = g_pGame->GetStreaming()->GetStreamingInfo(uiModel)->archiveId;
    return GetElementFromArchiveID(ucArhiveID);
}

bool CClientIMGManager::IsLinkableModel(unsigned int uiModel)
{
    return uiModel <= g_pGame->GetCountOfAllFileIDs();            // StreamModelInfoSize
}

bool CClientIMGManager::RestoreModel(unsigned int uiModel)
{
    // Get the DFF file that replaced it
    CClientIMG* pIMG = GetElementThatLinked(uiModel);

    if (pIMG)
    {
        // Restore it
        return pIMG->UnlinkModel(uiModel);
    }

    // Nothing to restore
    return false;
}

void CClientIMGManager::RemoveFromList(CClientIMG* pIMG)
{
    // Can we remove anything from the list?
    if (m_bRemoveFromList)
    {
        m_List.remove(pIMG);
    }
}

void CClientIMGManager::UpdateStreamerBufferSize()
{
    unsigned short usRequestStreamSize = m_uiDefaultStreamerBufferSize;

    for (CClientIMG* pImg : m_List)
    {
        if (!pImg->IsStreamed())
            continue;
        unsigned short usStreamSize = pImg->GetRequiredBufferSize();
        if (usStreamSize > usRequestStreamSize)
            usRequestStreamSize = usStreamSize;
    }

    g_pGame->GetStreaming()->SetStreamingBufferSize(usRequestStreamSize);
}
