#pragma once

#include <vector>

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

class CDFFModelOptimizationInfo
{
public:
    CDFFModelOptimizationInfo()
    {
        bDataSet = false;
        m_dffNameHash = 0;
        m_txdNameHash = 0;
    }

    bool         IsDataSet() { return bDataSet; }
    unsigned int GetTotalOptimizedTextures() { return optimizedTextures.size(); }
    unsigned int GetDFFNameHash() { return m_dffNameHash; }
    unsigned int GetTXDNameHash() { return m_txdNameHash; }

    void SetNames(unsigned int dffNameHash, unsigned int txdNameHash)
    {
        m_dffNameHash = dffNameHash;
        m_txdNameHash = txdNameHash;
        bDataSet = true;
    }

    void Addtexture(unsigned int textureNameHash, unsigned int textureSizeWithinUnwrappedImageInBytes)
    {
        optimizedTextures.emplace_back(textureNameHash, textureSizeWithinUnwrappedImageInBytes);
    }

private:
    bool                           bDataSet;
    unsigned int                   m_dffNameHash;
    unsigned int                   m_txdNameHash;
    std::vector<SOptimizedTexture> optimizedTextures;
};
