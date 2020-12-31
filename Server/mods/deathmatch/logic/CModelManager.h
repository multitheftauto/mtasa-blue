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
    unsigned short GetModelParent(unsigned short modelID);
    eModelType GetModelType(unsigned short modelID);

private:
    std::map<unsigned short, unsigned short> m_ModelsParent;
    std::map<unsigned short, eModelType> m_ModelsType;
};
