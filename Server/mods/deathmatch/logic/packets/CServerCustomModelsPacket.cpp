/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerCustomModelsPacket.cpp
 *  PURPOSE:     Server custom models
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CServerCustomModelsPacket::CServerCustomModelsPacket(eModelType modelType, const std::map<unsigned short, unsigned short> modelsParent)
    : m_ModelType(modelType), m_ModelsParent(modelsParent){};

CServerCustomModelsPacket::CServerCustomModelsPacket(eModelType modelType, unsigned short modelID, unsigned short parentID)
    : m_ModelType(modelType), m_ModelsParent({ { modelID, parentID } }) {};

bool CServerCustomModelsPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (m_ModelsParent.size() == 0)
        return false;

	BitStream.Write((uchar) m_ModelType);
    BitStream.WriteCompressed(static_cast<unsigned short>(m_ModelsParent.size()));
    for (auto iter = m_ModelsParent.begin(); iter != m_ModelsParent.end(); ++iter)
    {
        BitStream.Write(iter->first);
        BitStream.Write(iter->second);
    }

    return true;
}
