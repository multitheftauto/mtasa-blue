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

    // Buffer size as calculated by GTA - This is also the size of the largest file in all of the loaded IMGs

    // g_pGame->GetStreaming()->GetStreamingBufferSize() / 2048;
    // TODO: In the default gta3.img the biggest file is 1260 sectors, so to be fail safe, we double it
    // ideally, we'd just take this value from the game, but there's no clean/easy way to do that [without loading the img archives]
    // so, for now, this is good enough
    m_LargestFileSizeBlocks = m_GTALargestFileSizeBlocks = 1260 * 2;
}

CClientIMGManager::~CClientIMGManager()
{
    // Delete all our IMG's
    RemoveAll();
}

void CClientIMGManager::InitDefaultBufferSize()
{
}

CClientIMG* CClientIMGManager::GetElementFromArchiveID(unsigned char ucArchiveID)
{
    // By default GTA has 5 IMG's
    if (ucArchiveID < 6)
        return nullptr;

    for (auto iter : m_List)
    {
        CClientIMG* pIMG = iter;
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
    for (auto iter : m_List)
    {
        delete iter;
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
    uchar ucArhiveID = g_pGame->GetStreaming()->GetStreamingInfo(uiModel)->archiveId;
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

size_t CClientIMGManager::CalculateLargestFile() const
{
    auto largest = m_GTALargestFileSizeBlocks;

    for (const auto img : m_List)
    {
        if (!img->IsStreamed())
            continue;

        largest = std::max(img->GetLargestFileSizeBlocks(), largest);
    }

    return largest;
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
    m_LargestFileSizeBlocks = CalculateLargestFile();

    // Only update if necessary, otherwise leave it be [User might've set it manually - we don't want to touch that]
    if (const auto s = g_pGame->GetStreaming(); m_LargestFileSizeBlocks > s->GetStreamingBufferSize())
    {
        s->SetStreamingBufferSize(m_LargestFileSizeBlocks);
    }
}
