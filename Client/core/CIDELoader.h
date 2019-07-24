#pragma once
#include "CIMGArchive.h"
#include <array>

struct STXDDescriptor
{
    STXDImgArchiveInfo     m_TxdImgArchiveInfo;
    RwTexDictionary*       pTextureDictionary;
    CRenderWare*           pRenderWare;

    STXDDescriptor(int placeholder);

    ~STXDDescriptor() { }

    void SetTXDImgArchiveInfo(unsigned int uiOffset, unsigned short usSize) { m_TxdImgArchiveInfo = {uiOffset, usSize}; }

    STXDImgArchiveInfo* GetTXDImgArchiveInfo() { return &m_TxdImgArchiveInfo; }

    RwTexDictionary* GetTextureDictionary() { return pTextureDictionary; }

};

struct SDFFDescriptor
{
    int             m_nModelID;
    STXDDescriptor* m_pTxdDescriptor;

    SDFFDescriptor(int modelID, STXDDescriptor* pTxdDescriptor)
    {
        m_nModelID = modelID;
        m_pTxdDescriptor = pTxdDescriptor;
    }

    int GetModelID() { return m_nModelID; }

    STXDDescriptor* GetTXDDescriptor() { return m_pTxdDescriptor; }
};

class CIDELoader
{
public:
    CIDELoader();

    void FreeMemory();
    void LoadIDEFiles();
    bool LoadFileToString(const SString& fileName, SString& contents);
    void RemoveStringTabsSpaces(SString& contents);
    void ParseObjsLine(const SString& line);

    // we only need to read the objs section
    void            ParseContents(SString& contents);
    void            AddTXDDFFInfoToMaps(CIMGArchive* pIMgArchive);
    SDFFDescriptor* GetDFFDescriptor(const unsigned int uiDFFNameHash);

private:
    // Key: DFF name hash
    std::map<unsigned int, SDFFDescriptor> mapOfDffDescriptors;

    // Key: TXD name hash
    std::map<unsigned int, STXDDescriptor> mapOfTxdDescriptors;

    const std::array<std::string, 59> ideFilePaths = {
        "ide\\barriers.ide", "ide\\countn2.ide",  "ide\\countrye.ide", "ide\\countryN.ide", "ide\\countryS.ide", "ide\\countryW.ide", "ide\\counxref.ide",
        "ide\\default.ide",  "ide\\dynamic.ide",  "ide\\dynamic2.ide", "ide\\gen_int1.ide", "ide\\gen_int2.ide", "ide\\gen_int3.ide", "ide\\gen_int4.ide",
        "ide\\gen_int5.ide", "ide\\gen_intb.ide", "ide\\int_cont.ide", "ide\\int_LA.ide",   "ide\\int_SF.ide",   "ide\\int_veg.ide",  "ide\\LAe.ide",
        "ide\\LAe2.ide",     "ide\\LAhills.ide",  "ide\\LAn.ide",      "ide\\LAn2.ide",     "ide\\LAs.ide",      "ide\\LAs2.ide",     "ide\\LAw.ide",
        "ide\\LAw2.ide",     "ide\\LaWn.ide",     "ide\\LAxref.ide",   "ide\\leveldes.ide", "ide\\levelmap.ide", "ide\\levelxre.ide", "ide\\multiobj.ide",
        "ide\\peds.ide",     "ide\\procobj.ide",  "ide\\propext.ide",  "ide\\props.ide",    "ide\\props2.ide",   "ide\\savehous.ide", "ide\\seabed.ide",
        "ide\\SFe.ide",      "ide\\SFn.ide",      "ide\\SFs.ide",      "ide\\SFSe.ide",     "ide\\SFw.ide",      "ide\\SFxref.ide",   "ide\\stadint.ide",
        "ide\\txd.ide",      "ide\\txdcut.ide",   "ide\\vegasE.ide",   "ide\\VegasN.ide",   "ide\\VegasS.ide",   "ide\\VegasW.ide",   "ide\\vegaxref.ide",
        "ide\\vegepart.ide", "ide\\vehicles.ide", "ide\\veh_mods.ide"};
};
