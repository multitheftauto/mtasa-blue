#pragma once
#include "CIMGArchive.h"
#include <array>

struct STXDDescriptor
{
    STXDImgArchiveInfo m_TxdImgArchiveInfo;
    std::set <unsigned int> setOfDffNameHashes;
    RwTexDictionary* pTextureDictionary;

    STXDDescriptor(int placeholder)
    {
        m_TxdImgArchiveInfo = { 0 };
        pTextureDictionary = nullptr;
    }

    ~STXDDescriptor()
    {
        if (setOfDffNameHashes.size() == 0)
        {
            UnloadTexture();
        }
    }

    void SetTXDImgArchiveInfo(unsigned int uiOffset, unsigned short usSize)
    {
        m_TxdImgArchiveInfo = {uiOffset, usSize};
    }

    void SetTextureDictionary(RwTexDictionary* pTheTextureDictionary)
    {
        pTextureDictionary = pTheTextureDictionary;
    }

    STXDImgArchiveInfo* GetTXDImgArchiveInfo()
    {
        return &m_TxdImgArchiveInfo;
    }

    RwTexDictionary* GetTextureDictionary()
    {
        return pTextureDictionary;
    }

    void UnloadTexture()
    {
        if (pTextureDictionary)
        {
            std::printf("unloading texture | size is zero\n");
            CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
            pRenderWare->DestroyTXD(pTextureDictionary);
            pTextureDictionary = nullptr;
        }
    }

    void AddDFFNameToSet(const unsigned int uiDFFNameHash)
    {
        setOfDffNameHashes.insert(uiDFFNameHash);
    }

    // Call this function only if all names have been inserted
    // to setOfDffNameHashes
    void RemoveDFFNameFromSet(const unsigned int uiDFFNameHash)
    {
        setOfDffNameHashes.erase(uiDFFNameHash);
        if (setOfDffNameHashes.size() == 0)
        {
            UnloadTexture();
        }
    }
};

class CIDELoader
{
public:
    CIDELoader();

    bool LoadFileToString(const SString & fileName, SString& contents);
    void RemoveStringTabsSpaces(SString& contents);
    void ParseObjsLine(const SString& line);

    // we only need to read the objs section
    void ParseContents(SString& contents);
    void AddTXDDFFInfoToMaps(CIMGArchive* pIMgArchive);
    STXDDescriptor* GetTXDDescriptorFromDFFName(const unsigned int uiDFFNameHash);

private:
    // Key: DFF name hash | Value: TXD name hash
    std::map<unsigned int, STXDDescriptor*> mapOfDffTxdDescriptors;

    // Key: TXD name hash | Value: STXDReference
    std::map<unsigned int, STXDDescriptor> mapOfTxdDescriptors;

    const std::array <std::string, 59> ideFilePaths =
    {
        "ide\\barriers.ide",
        "ide\\countn2.ide",
        "ide\\countrye.ide",
        "ide\\countryN.ide",
        "ide\\countryS.ide",
        "ide\\countryW.ide",
        "ide\\counxref.ide",
        "ide\\default.ide",
        "ide\\dynamic.ide",
        "ide\\dynamic2.ide",
        "ide\\gen_int1.ide",
        "ide\\gen_int2.ide",
        "ide\\gen_int3.ide",
        "ide\\gen_int4.ide",
        "ide\\gen_int5.ide",
        "ide\\gen_intb.ide",
        "ide\\int_cont.ide",
        "ide\\int_LA.ide",
        "ide\\int_SF.ide",
        "ide\\int_veg.ide",
        "ide\\LAe.ide",
        "ide\\LAe2.ide",
        "ide\\LAhills.ide",
        "ide\\LAn.ide",
        "ide\\LAn2.ide",
        "ide\\LAs.ide",
        "ide\\LAs2.ide",
        "ide\\LAw.ide",
        "ide\\LAw2.ide",
        "ide\\LaWn.ide",
        "ide\\LAxref.ide",
        "ide\\leveldes.ide",
        "ide\\levelmap.ide",
        "ide\\levelxre.ide",
        "ide\\multiobj.ide",
        "ide\\peds.ide",
        "ide\\procobj.ide",
        "ide\\propext.ide",
        "ide\\props.ide",
        "ide\\props2.ide",
        "ide\\savehous.ide",
        "ide\\seabed.ide",
        "ide\\SFe.ide",
        "ide\\SFn.ide",
        "ide\\SFs.ide",
        "ide\\SFSe.ide",
        "ide\\SFw.ide",
        "ide\\SFxref.ide",
        "ide\\stadint.ide",
        "ide\\txd.ide",
        "ide\\txdcut.ide",
        "ide\\vegasE.ide",
        "ide\\VegasN.ide",
        "ide\\VegasS.ide",
        "ide\\VegasW.ide",
        "ide\\vegaxref.ide",
        "ide\\vegepart.ide",
        "ide\\vehicles.ide",
        "ide\\veh_mods.ide"
    };

};
