#pragma once

#include <vector>
#include <fstream>
#include <istream>

struct SOptimizedTexture
{
    unsigned int m_textureNameHash;
    unsigned int m_textureSizeWithinUnwrappedImageInBytes;

    SOptimizedTexture(unsigned int textureNameHash, unsigned int textureSizeWithinUnwrappedImageInBytes)
    {
        m_textureNameHash = textureNameHash;
        m_textureSizeWithinUnwrappedImageInBytes = textureSizeWithinUnwrappedImageInBytes;
    }
};

struct SOptimizedDFF
{
    SOptimizedDFF(unsigned int dffNameHash, unsigned int txdNameHash)
    {
        m_dffNameHash = dffNameHash;
        m_txdNameHash = txdNameHash;
    }

    unsigned int GetTotalOptimizedTextures() { return optimizedTextures.size(); }
    unsigned int GetDFFNameHash() { return m_dffNameHash; }
    unsigned int GetTXDNameHash() { return m_txdNameHash; }

    void Addtexture(unsigned int textureNameHash, unsigned int textureSizeWithinUnwrappedImageInBytes)
    {
        optimizedTextures.emplace_back(textureNameHash, textureSizeWithinUnwrappedImageInBytes);
    }

    SOptimizedTexture& GetOptimizedTexture(size_t index) { return optimizedTextures[index]; }

private:
    unsigned int                   m_dffNameHash;
    unsigned int                   m_txdNameHash;
    std::vector<SOptimizedTexture> optimizedTextures;
};

enum eDFFModelOptimizationOperation
{
    OPTIMIZE_FILE_READ = 0,
    OPTIMIZE_FILE_WRITE
};

struct SOptimizationInfoHeader
{
    char         version[4];            // GMOI = GTA Model Optimization Info
    unsigned int totalEntries;
};

struct SOptimizationInfoEntry
{
    unsigned int dffNameHash;
    unsigned int txdNameHash;
    unsigned int totalOptimizedTextures;
};

class CDFFModelOptimizationInfo
{
public:
    CDFFModelOptimizationInfo(){};
    ~CDFFModelOptimizationInfo();

    bool CreateTheFile(const SString& filePath, eDFFModelOptimizationOperation fileOperation);
    void CloseFile();

    unsigned int GetOutputFileSize();
    void         WriteHeader();
    void         WriteOptimizedDFFs();
    bool         ReadOptimizedDFFs();

    void AllocateSpace(size_t space) { optimizedDFFs.reserve(space); }
    void FreeMemory() { std::vector<SOptimizedDFF>().swap(optimizedDFFs); }

    SOptimizedDFF& InsertDFF(const char* dffName);

private:
    std::vector<char>          tempData;
    std::vector<SOptimizedDFF> optimizedDFFs;
    std::ifstream              inputFileStream;
    std::ofstream              outputFileStream;
    const char*                pHeaderVersion = "GMOI";
};
