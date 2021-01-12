/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerCustomModelsPacket.h
 *  PURPOSE:     Server custom models
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../packets/CPacket.h"

class CServerCustomModelsPacket final : public CPacket
{
public:
    CServerCustomModelsPacket(eModelType modelType, const std::map<unsigned short, unsigned short> modelsParent);
    CServerCustomModelsPacket(eModelType modelType, unsigned short modelID, unsigned short parentID);

    inline ePacketID     GetPacketID() const { return PACKET_ID_SERVER_CUSTOM_MODELS; };
    inline unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    eModelType m_ModelType;
	std::map<unsigned short, unsigned short> m_ModelsParent;
};
