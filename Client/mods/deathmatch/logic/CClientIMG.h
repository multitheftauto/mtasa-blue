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

struct tImgFileInfo
{
    unsigned int   uiOffset;
    unsigned short usSize;
    unsigned short usUnpackedSize;
    char           szFileName[24];
};

struct tLinkedModelRestoreInfo
{
    unsigned int   uiModelID;
    unsigned char  ucStreamID;
    unsigned int   uiOffset;
    unsigned short usSize;
};

class CClientIMG : public CClientEntity
{
    DECLARE_CLASS(CClientIMG, CClientEntity)
    friend class CClientIMGManager;
public:
    CClientIMG(class CClientManager* pManager, ElementID ID);
    ~CClientIMG();

    void Unlink(){};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    eClientEntityType GetType() const { return CCLIENTIMG; };
    unsigned char     GetArchiveID() { return m_ucArchiveID; };
    unsigned int      GetFilesCount() { return m_uiFilesCount; };
    bool              Load(SString sFilePath);
    void              Unload();
    tImgFileInfo*     GetFileInfo(unsigned int uiFileID);
    unsigned int      GetFileID(SString sFileName);

    bool              StreamEnable();
    bool              StreamDisable();
    bool              IsStreamed();
    bool              LinkModel(unsigned int usModelID, unsigned int usFileID);
    bool              UnlinkModel(unsigned int usModelID);

private:
    class CClientIMGManager*    m_pImgManager;

    SString                     m_strFilename;
    unsigned char               m_ucArchiveID;
    unsigned int                m_uiFilesCount;
    std::vector<tImgFileInfo>   m_pContentInfo;

    std::vector<tLinkedModelRestoreInfo*> m_pRestoreData;
};
