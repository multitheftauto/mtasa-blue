/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColModel.cpp
 *  PURPOSE:     Model collision (.col file) entity class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CColModel.h>

CClientColModel::CClientColModel(CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_pManager = pManager;
    m_pColModelManager = pManager->GetColModelManager();
    m_pColModel = NULL;

    SetTypeName("col");

    // Add us to DFF manager's list
    m_pColModelManager->AddToList(this);
}

CClientColModel::~CClientColModel()
{
    // Remove us from DFF manager's list
    m_pColModelManager->RemoveFromList(this);

    // Unreplace our cols
    RestoreAll();

    if (m_pColModel)
        m_pColModel->Destroy();
}

bool CClientColModel::Load(bool isRaw, SString input)
{
    if (m_pColModel)
        return false;

    if (isRaw)
    {
        return LoadFromBuffer(std::move(input));
    }
    else
    {
        return LoadFromFile(std::move(input));
    }
}

bool CClientColModel::LoadFromFile(SString filePath)
{
    SString buffer;

    if (!FileLoad(std::nothrow, filePath, buffer))
        return false;

    g_pClientGame->GetResourceManager()->ValidateResourceFile(filePath, buffer.data(), buffer.size());

    if (!g_pCore->GetNetwork()->CheckFile("col", filePath, buffer.data(), buffer.size()))
        return false;

    m_pColModel = g_pGame->GetRenderWare()->ReadCOL(std::move(buffer));

    return m_pColModel != nullptr;
}

bool CClientColModel::LoadFromBuffer(SString buffer)
{
    if (!g_pCore->GetNetwork()->CheckFile("col", "", buffer.data(), buffer.size()))
        return false;

    m_pColModel = g_pGame->GetRenderWare()->ReadCOL(std::move(buffer));

    return m_pColModel != nullptr;
}

bool CClientColModel::Replace(unsigned short usModel)
{
    // We have a model loaded?
    if (m_pColModel)
    {
        // Check if anyone has replaced that model already. If so make it forget that
        // it replaced it. We're the new replacer.
        CClientColModel* pAlready = m_pColModelManager->GetElementThatReplaced(usModel);
        if (pAlready)
        {
            pAlready->m_Replaced.remove(usModel);
        }

        // Replace the collisions
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModel);
        pModelInfo->SetColModel(m_pColModel);

        // Remember that we replaced it
        m_Replaced.push_back(usModel);

        // Success
        return true;
    }

    // Failed
    return false;
}

void CClientColModel::Restore(unsigned short usModel)
{
    // Restore it
    InternalRestore(usModel);

    // Remove the restored id from the list
    m_Replaced.remove(usModel);
}

void CClientColModel::RestoreAll()
{
    // Loop through our replaced ids
    std::list<unsigned short>::iterator iter = m_Replaced.begin();
    for (; iter != m_Replaced.end(); iter++)
    {
        // Restore this model
        InternalRestore(*iter);
    }

    // Clear the list
    m_Replaced.clear();
}

bool CClientColModel::HasReplaced(unsigned short usModel)
{
    // Loop through our replaced ids
    std::list<unsigned short>::iterator iter = m_Replaced.begin();
    for (; iter != m_Replaced.end(); iter++)
    {
        // Is this the given ID
        if (*iter == usModel)
        {
            // We have replaced it
            return true;
        }
    }

    // We have not replaced it
    return false;
}

void CClientColModel::InternalRestore(unsigned short usModel)
{
    // Grab the model info and restore it
    CModelInfo* pModel = g_pGame->GetModelInfo(usModel);
    pModel->RestoreColModel();
}

// Return true if data looks like COL file contents
bool CClientColModel::IsCOLData(const SString& strData)
{
    // COL file format: version[4] + size[4] + name[24] = minimum 32 bytes
    // Version is 4-char string: "COLL", "COL2", "COL3", or "COL4" (not null-terminated)
    if (strData.length() < 32)
        return false;

    // Check if starts with "COL" magic number
    if (memcmp(strData, "COL", 3) != 0)
        return false;

    // Validate 4th character is valid version indicator
    const char versionChar = strData[3];
    return versionChar == 'L' || versionChar == '2' || versionChar == '3' || versionChar == '4';
}
