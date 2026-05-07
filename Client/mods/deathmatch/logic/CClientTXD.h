/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTXD.h
*  PURPOSE:     TXD manager class header

*
*****************************************************************************/

#pragma once

#include <game/CRenderWare.h>
#include "CClientEntity.h"

class CClientTXD final : public CClientEntity, public CRwReplacementOwner
{
    DECLARE_CLASS(CClientTXD, CClientEntity)
public:
    CClientTXD(class CClientManager* pManager, ElementID ID);
    ~CClientTXD();

    void Unlink() {};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition) {};

    eClientEntityType GetType() const { return CCLIENTTXD; }
    bool              Load(bool isRaw, SString input, bool enableFiltering);
    bool              AddClothingTexture(const std::string& modelName);
    bool              Import(unsigned short usModelID);
    static bool       IsImportableModel(unsigned short usModelID);
    static bool       IsTXDData(const SString& strData);

    // CRwReplacementOwner: re-decode this TXD after a D3D9 device reset.
    // Returns true if rebuild succeeded, false if the source bytes are unrecoverable
    // (raw-data TXDs whose buffer was freed at Import).
    bool RebuildReplacementsAfterDeviceReset() override;

private:
    bool LoadFromFile(SString filePath);
    bool LoadFromBuffer(SString buffer);

    void Restream(unsigned short usModel);
    bool GetFilenameToUse(SString& strOutFilename);

    SString              m_strFilename;
    bool                 m_bFilteringEnabled;
    bool                 m_bIsRawData;
    bool                 m_bUsingFileDataForClothes;
    SString              m_FileData;
    SReplacementTextures m_ReplacementTextures;
};
