#include "StdInc.h"
#include "CIDELoader.h"

CIDELoader::CIDELoader()
{
    for (auto& fileName : ideFilePaths)
    {
        SString fileContents;
        if (LoadFileToString(fileName, fileContents))
        {
            ParseContents(fileContents);
        }
        else
        {
            std::printf("failed to read file: %s\n", fileName.c_str());
        }
    }
}

bool CIDELoader::LoadFileToString(const SString & fileName, SString& contents)
{
    std::ifstream in(fileName);
    if (in)
    {
        contents.assign((std::istreambuf_iterator<char>(in)),
            (std::istreambuf_iterator<char>()));
        return true;
    }
    return false;
}

void CIDELoader::RemoveStringTabsSpaces(SString& contents)
{
    SString::iterator end_pos = std::remove(contents.begin(), contents.end(), ' ');
    contents.erase(end_pos, contents.end());

    end_pos = std::remove(contents.begin(), contents.end(), '\t');
    contents.erase(end_pos, contents.end());
}

void CIDELoader::ParseObjsLine(const SString& line)
{
    if (line.size() == 0 || line[0] == '#')
    {
        return;
    }

    //std::cout << line << std::endl;
    std::vector<SString> vecSplittedStrings;
    line.Split(",", vecSplittedStrings);

    if (vecSplittedStrings.size() >= 3)
    {
        const SString strDFFName = vecSplittedStrings[1] + ".dff";
        const SString strTXDName = vecSplittedStrings[2] + ".txd";
        const unsigned int uiDFFNameHash = HashString(strDFFName.ToLower());
        const unsigned int uiTXDNameHash = HashString(strTXDName.ToLower());

      //  std::printf("dff name: %s | txd name: %s\n", strDFFName.c_str(), strTXDName.c_str());

        auto it = mapOfTxdDescriptors.find(uiTXDNameHash);
        if (it == mapOfTxdDescriptors.end())
        {
            auto thePair = mapOfTxdDescriptors.emplace(uiTXDNameHash, 0);
            it = thePair.first;
        }

        mapOfDffTxdDescriptors[uiDFFNameHash] = &it->second;
    }
    else
    {
        std::printf("\n\n\nError !!!: vecSplittedStrings SIZE IS LESS THAN 3 | line : %s\n\n\n", line.c_str());
    }

}

// we only need to read the objs section
void CIDELoader::ParseContents(SString& contents)
{
    SString line;
    std::istringstream inStream(contents);
    while (std::getline(inStream, line))
    {
        RemoveStringTabsSpaces(line);
        if (line == "objs")
        {
            while (std::getline(inStream, line))
            {
                RemoveStringTabsSpaces(line);
                if (line == "end")
                {
                    break;
                }

                ParseObjsLine(line);
            }
        }
        else if (line == "tobj")
        {
            while (std::getline(inStream, line))
            {
                RemoveStringTabsSpaces(line);
                if (line == "end")
                {
                    break;
                }

                ParseObjsLine(line);
            }
        }
        else if (line == "peds")
        {
            while (std::getline(inStream, line))
            {
                RemoveStringTabsSpaces(line);
                if (line == "end")
                {
                    break;
                }

                ParseObjsLine(line);
            }
        }
        else if (line == "weap")
        {
            while (std::getline(inStream, line))
            {
                RemoveStringTabsSpaces(line);
                if (line == "end")
                {
                    break;
                }

                ParseObjsLine(line);
            }
        }
        else if (line == "anim")
        {
            while (std::getline(inStream, line))
            {
                RemoveStringTabsSpaces(line);
                if (line == "end")
                {
                    break;
                }

                ParseObjsLine(line);
            }
        }
    }

  
}

void CIDELoader::AddTXDDFFInfoToMaps(CIMGArchive* pIMgArchive)
{
    int i = 0;
    auto vecArchiveEntryHeaders = pIMgArchive->GetArchiveDirEntries();
    for (auto& entryHeader: vecArchiveEntryHeaders)
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

                /*
                // REMOVE THIS AFTER TESTing
                if (strFileName == "slamvan.txd")
                {

                    //bbb_lr_slv1.dff
                    STXDDescriptor* pTXDDescriptor = GetTXDDescriptorFromDFFName(0xa2743d30);
                    if (pTXDDescriptor)
                    {
                        EntryHeader * pTXDEntryHeader = pTXDDescriptor->GetTXDEntryHeaderOfIMGArchive();
                        unsigned int actualFileOffset = pTXDEntryHeader->offset * 2048;

                        std::printf("AddTXDDFFInfoToMaps: pTXDEntryHeader = %p | EntryHeaderIndex: %d\n", pTXDEntryHeader, i);
                        std::printf("AddTXDDFFInfoToMaps: pTXDEntryHeader->offset = %u\n", pTXDEntryHeader->offset);
                    }
                }
                // REMOVE END
                */
            }
        }
        else if (strFileExtension == "dff")
        {
            const unsigned int uiDFFNameHash = HashString(strFileName);

            auto it = mapOfDffTxdDescriptors.find(uiDFFNameHash);
            if (it != mapOfDffTxdDescriptors.end())
            {
                it->second->AddDFFNameToSet(uiDFFNameHash);
            }
        }
        i++;
    }
}

STXDDescriptor* CIDELoader::GetTXDDescriptorFromDFFName(const unsigned int uiDFFNameHash)
{
    auto it = mapOfDffTxdDescriptors.find(uiDFFNameHash);
    if (it != mapOfDffTxdDescriptors.end())
    {
        return it->second;
    }
    return nullptr;
}
