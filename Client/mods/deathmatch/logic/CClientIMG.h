/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientIMG.h
*  PURPOSE:     IMG manager class header

*
*****************************************************************************/

#pragma once

#include "CClientEntity.h"

struct tImgHeader
{
    char            szMagic[4];
    unsigned int    uiFilesCount;
};

struct tImgFileInfo
{
    unsigned int   uiOffset;
    unsigned short usSize;
    unsigned short usUnpackedSize;
    char           szFileName[24];
};

class CClientIMG : public CClientEntity
{
    DECLARE_CLASS(CClientIMG, CClientEntity)
public:
    CClientIMG(class CClientManager* pManager, ElementID ID);
    ~CClientIMG();

    void Unlink(){};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    eClientEntityType GetType() const { return CCLIENTIMG; }
    bool              Load(SString sFilePath);
    bool              Unload();
    tImgFileInfo*     GetFileInfo(unsigned int usFileID);
    unsigned int      GetFileID(SString sFileName);

    bool              Stream();
    bool              LinkModel(unsigned short usModelID, unsigned int usFileID);
private:

    SString                     m_strFilename;
    unsigned char               m_ucStreamID;
    unsigned int                m_uiFilesCount;
    std::vector<tImgFileInfo>   m_pContentInfo;
};
