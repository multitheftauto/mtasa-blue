#include "StdInc.h"
#include "CIDELoader.h"

STXDDescriptor::STXDDescriptor(int placeholder)
{
    m_TxdImgArchiveInfo = {0};
    pTextureDictionary = nullptr;
    pRenderWare = g_pCore->GetGame()->GetRenderWare();
}

CIDELoader::CIDELoader()
{
}

void CIDELoader::FreeMemory()
{
    std::map<unsigned int, SDFFDescriptor>().swap(mapOfDffDescriptors);
    std::map<unsigned int, STXDDescriptor>().swap(mapOfTxdDescriptors);
    std::vector<ide::gtasa::ide_file>().swap(ideFiles);
}

void CIDELoader::LoadIDEFiles()
{
    ideFiles.reserve(ideFilePaths.size());
    for (auto& fileName : ideFilePaths)
    {
        ide::gtasa::ide_file& ideFile = ideFiles.emplace_back(fileName);
        if (!ideFile.LoadIDE())
        {
            std::printf("failed to read file: %s\n", fileName.c_str());
            return;
        }

        for (auto& obj : ideFile.objs)
        {
            CreateModelDescriptor((ide::gtasa::tModelObject*)&obj, obj.id, obj.modelName, obj.txdName);
        }

        for (auto& tobj : ideFile.tobj)
        {
            CreateModelDescriptor((ide::gtasa::tModelObject*)&tobj, tobj.id, tobj.modelName, tobj.txdName);
        }

        for (auto& anim : ideFile.anim)
        {
            CreateModelDescriptor((ide::gtasa::tModelObject*)&anim, anim.id, anim.modelName, anim.txdName);
        }

        for (auto& weap : ideFile.weap)
        {
            CreateModelDescriptor((ide::gtasa::tModelObject*)&weap, weap.id, weap.modelName, weap.txdName);
        }

        for (auto& car : ideFile.cars)
        {
            CreateModelDescriptor((ide::gtasa::tModelObject*)&car, car.id, car.modelName, car.txdName);
        }

        for (auto& ped : ideFile.peds)
        {
            CreateModelDescriptor((ide::gtasa::tModelObject*)&ped, ped.id, ped.modelName, ped.txdName);
        }
    }
}

bool CIDELoader::WriteIDEFiles(const SString& folderLocation)
{
    for (auto& ideFile : ideFiles)
    {
        if (!ideFile.WriteIDE(folderLocation))
        {
            return false;
        }
    }
    return true;
}

void CIDELoader::CreateModelDescriptor(ide::gtasa::tModelObject* pModelObject, const int modelID, const char* modelName, const char* txdName)
{
    SString strDFFName = modelName, strTXDName = txdName;
    strDFFName += ".dff";
    strTXDName += ".txd";
    const unsigned int uiDFFNameHash = HashString(strDFFName.ToLower());
    const unsigned int uiTXDNameHash = HashString(strTXDName.ToLower());

    //  std::printf("dff name: %s | txd name: %s\n", strDFFName.c_str(), strTXDName.c_str());

    auto it = mapOfTxdDescriptors.find(uiTXDNameHash);
    if (it == mapOfTxdDescriptors.end())
    {
        auto thePair = mapOfTxdDescriptors.emplace(uiTXDNameHash, 0);
        it = thePair.first;
    }

    auto dffDescriptorIterator = mapOfDffDescriptors.find(uiDFFNameHash);
    if (dffDescriptorIterator == mapOfDffDescriptors.end())
    {
        mapOfDffDescriptors.emplace(std::piecewise_construct, std::forward_as_tuple(uiDFFNameHash), std::forward_as_tuple(modelID, &it->second, pModelObject));
    }
}

void CIDELoader::AddTXDDFFInfoToMaps(CIMGArchive* pIMgArchive)
{
    auto vecArchiveEntryHeaders = pIMgArchive->GetArchiveDirEntries();
    for (auto& entryHeader : vecArchiveEntryHeaders)
    {
        SString strFileName = entryHeader.fileName;
        strFileName = strFileName.ToLower();

        SString strFileExtension = strFileName.substr(strFileName.find_last_of(".") + 1);

        if (strFileExtension == "txd")
        {
            const unsigned int uiTXDNameHash = HashString(strFileName);

            auto itTxd = mapOfTxdDescriptors.find(uiTXDNameHash);
            if (itTxd != mapOfTxdDescriptors.end())
            {
                itTxd->second.SetTXDImgArchiveInfo(entryHeader.offset, entryHeader.usSize);
            }
        }
    }
}

SDFFDescriptor* CIDELoader::GetDFFDescriptor(const unsigned int uiDFFNameHash)
{
    auto it = mapOfDffDescriptors.find(uiDFFNameHash);
    if (it != mapOfDffDescriptors.end())
    {
        return &it->second;
    }
    return nullptr;
}
