/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CModelManager.h
 *  PURPOSE:     Model manager class
 *
 *****************************************************************************/

class CModelManager;

#pragma once

class CModelManager
{
public:
    bool RequestModel(unsigned short modelID, unsigned short parentID, eModelType modelType);

    bool IsValidModel(unsigned short modelID);
    ushort GetModelParent(unsigned short modelID);
    eModelType GetModelType(unsigned short modelID);
    bool HasModelsByType(eModelType modelType);
    std::map<unsigned short, unsigned short> GetModels(eModelType modelType);

private:
    std::map<eModelType, std::map<unsigned short, unsigned short>> m_Models;
    std::map<unsigned short, unsigned short> m_ModelsParent;
    std::map<unsigned short, eModelType> m_ModelsType;
};
