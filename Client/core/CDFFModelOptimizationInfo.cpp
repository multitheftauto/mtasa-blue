#include <StdInc.h>
#include "CDFFModelOptimizationInfo.h"

CDFFModelOptimizationInfo::~CDFFModelOptimizationInfo()
{
    CloseFile();
}

SOptimizedDFF& CDFFModelOptimizationInfo::InsertDFF(unsigned int dffNameHash, unsigned int txdNameHash)
{
    return optimizedDFFs.emplace_back(dffNameHash, txdNameHash);
};

SOptimizedDFF& CDFFModelOptimizationInfo::InsertDFF(const char* dffName)
{
    unsigned int dffNameHash = HashString(dffName);

    SString fileName = dffName;
    size_t  lastindex = fileName.find_last_of(".");
    SString fileNameWithoutExtension = fileName.substr(0, lastindex);
    SString strTXDFileName = fileNameWithoutExtension + ".txd";

    unsigned int txdNameHash = HashString(strTXDFileName);
    return optimizedDFFs.emplace_back(dffNameHash, txdNameHash);
};

bool CDFFModelOptimizationInfo::CreateTheFile(const SString& filePath, eDFFModelOptimizationOperation fileOperation)
{
    m_fileOperation = fileOperation;
    if (fileOperation == OPTIMIZE_FILE_READ)
    {
        inputFileStream.open(FromUTF8(filePath), std::ios::binary | std::ios::ate);
        return inputFileStream.fail() == false;
    }
    else if (fileOperation == OPTIMIZE_FILE_WRITE)
    {
        outputFileStream.open(FromUTF8(filePath), std::ofstream::out | std::ios::binary | std::ios::trunc);
        return outputFileStream.fail() == false;
    }
    return false;
}

unsigned int CDFFModelOptimizationInfo::GetOutputFileSize()
{
    // DFF Name Hash Size + Texture Name Hash Size + unsigned int (totalTextures) size = 12
    unsigned int size = 12 * optimizedDFFs.size();
    for (auto& optimizedDFF : optimizedDFFs)
    {
        size += optimizedDFF.GetTotalOptimizedTextures() * sizeof(SOptimizedTexture);
    }
    return size;
}

void CDFFModelOptimizationInfo::WriteHeader()
{
    SOptimizationInfoHeader header = {0};
    memcpy(header.version, pHeaderVersion, 4);
    header.totalEntries = optimizedDFFs.size();

    outputFileStream.seekp(0, std::ios::beg);
    outputFileStream.write((char*)&header, sizeof(SOptimizationInfoHeader));
}

// Make sure to call AllocateSpace before calling this function
bool CDFFModelOptimizationInfo::ReadOptimizedDFFs()
{
    std::streamsize iFileSize = inputFileStream.tellg();
    if (iFileSize == -1)
    {
        return false;
    }

    inputFileStream.seekg(0, std::ios::beg);
    tempData.resize(static_cast<size_t>(iFileSize));
    inputFileStream.read(tempData.data(), iFileSize);

    char*                   pInputData = tempData.data();
    SOptimizationInfoHeader header = *(SOptimizationInfoHeader*)pInputData;
    pInputData += sizeof(SOptimizationInfoHeader);
    if (strncmp(header.version, pHeaderVersion, 4) != 0)
    {
        printf("Incorrect header\n");
        return false;
    }

    printf("ReadOptimizedDFFs: yes GMOI | totalEntries: %u\n", header.totalEntries);

    for (unsigned int i = 0; i < header.totalEntries; i++)
    {
        SOptimizationInfoEntry optimizationInfoEntry = *(SOptimizationInfoEntry*)pInputData;
        pInputData += sizeof(SOptimizationInfoEntry);
        SOptimizedDFF& optimizedDFF = optimizedDFFs.emplace_back(optimizationInfoEntry.dffNameHash, optimizationInfoEntry.txdNameHash);

        //printf("texture entires: %u\n", optimizationInfoEntry.totalOptimizedTextures);

        for (unsigned int textureIndex = 0; textureIndex < optimizationInfoEntry.totalOptimizedTextures; textureIndex++)
        {
            SOptimizedTexture* pOptimizedTexture = (SOptimizedTexture*)pInputData;
            optimizedDFF.Addtexture(pOptimizedTexture->m_textureNameHash, pOptimizedTexture->m_textureSizeWithinUnwrappedImageInBytes);
            pInputData += sizeof(SOptimizedTexture);
        }
    }

    std::vector<char>().swap(tempData);
    return true;
}

void CDFFModelOptimizationInfo::WriteOptimizedDFFs()
{
    tempData.resize(GetOutputFileSize());
    char* pOutputData = tempData.data();

    for (auto& optimizedDFF : optimizedDFFs)
    {
        unsigned int totalOptimizedTextures = optimizedDFF.GetTotalOptimizedTextures();

        *(unsigned int*)pOutputData = optimizedDFF.GetDFFNameHash();
        pOutputData += 4;
        *(unsigned int*)pOutputData = optimizedDFF.GetTXDNameHash();
        pOutputData += 4;
        *(unsigned int*)pOutputData = totalOptimizedTextures;
        pOutputData += 4;

        for (size_t textureIndex = 0; textureIndex < totalOptimizedTextures; textureIndex++)
        {
            SOptimizedTexture& optimizedTexture = optimizedDFF.GetOptimizedTexture(textureIndex);
            *(SOptimizedTexture*)pOutputData = optimizedTexture;
            pOutputData += sizeof(SOptimizedTexture);
        }
    }

    outputFileStream.write(tempData.data(), tempData.size());
    std::vector<char>().swap(tempData);
}

void CDFFModelOptimizationInfo::CloseFile()
{
    if (m_fileOperation == OPTIMIZE_FILE_READ)
    {
        inputFileStream.close();
    }
    else if (m_fileOperation == OPTIMIZE_FILE_WRITE)
    {
        outputFileStream.close();
    }
}
