/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientIMGManager.h
 *  PURPOSE:     IMG container manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

class CClientIMGManager;

#pragma once

#include "CClientIMG.h"

class CClientIMGManager
{
    friend class CClientIMG;

public:
    CClientIMGManager(class CClientManager* pManager);
    ~CClientIMGManager();

    void InitDefaultBufferSize();
    void RemoveAll();
    bool Exists(CClientIMG* pIMG);

    CClientIMG* GetElementFromArchiveID(unsigned char ucStreamID);
    CClientIMG* GetElementThatLinked(unsigned int uiModel);

    bool        RestoreModel(unsigned int uiModel);
    static bool IsLinkableModel(unsigned int uiModel);
    void        UpdateStreamerBufferSize();
    auto        GetLargestFileSizeBlocks() const { return m_LargestFileSizeBlocks; }

private:
    size_t CalculateLargestFile() const;
    void   AddToList(CClientIMG* pIMG) { m_List.push_back(pIMG); }
    void   RemoveFromList(CClientIMG* pIMG);

    std::list<CClientIMG*> m_List;
    bool                   m_bRemoveFromList;
    uint32                 m_GTALargestFileSizeBlocks;
    uint32                 m_LargestFileSizeBlocks; // Size of the largest file [in streaming blocks/sectors] in any of the loaded imgs
};
