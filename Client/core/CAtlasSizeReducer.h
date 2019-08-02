#pragma once


struct SOptimizedDFF;
struct SOptimizedTexture;

class CAtlasSizeReducer
{
public:
    CAtlasSizeReducer(SOptimizedDFF* pOptimizedDFF, unsigned int defaultTXDSizeInBytes, std::vector<SOptimizedTexture*>* vecOptimizedTextures,
                      std::set<RwTexture*>* setOfUsedTextures)
    {
        m_pOptimizedDFF = pOptimizedDFF;
        m_vecOptimizedTextures = vecOptimizedTextures;
        m_setOfUsedTextures = setOfUsedTextures;

        m_defaultTXDSizeInMbs = (defaultTXDSizeInBytes / 1024.0f) / 1024.0f;

        m_totalAtlasSizeReducedInBytes = 0;
        m_atlasSizeInPercentange = 0.0f;
        m_atlasSizeInMBs = 0.0f;

        GetAtlasSize();

        m_vecOptimizedTexturesCache = *m_vecOptimizedTextures;
    }

    std::set<unsigned int>& GetSetOfIgnoredTextureNameHashes() { return m_setOfIgnoredTextureNameHashes; }

    bool                    IsAtlasTooBig()
    {
        if (m_defaultTXDSizeInMbs < 0.6f)
        {
            if (m_atlasSizeInPercentange >= 600.0f)
            {
                return true;
            }
        }

        if (m_defaultTXDSizeInMbs < 0.7f)
        {
            if (m_atlasSizeInPercentange >= 450.0f)
            {
                return true;
            }
            return false;
        }

        if (m_defaultTXDSizeInMbs < 1.0f)
        {
            if (m_atlasSizeInPercentange >= 300.0f)
            {
                return true;
            }
            return false;
        }

        if (m_defaultTXDSizeInMbs < 1.6f)
        {
            if (m_atlasSizeInPercentange >= 250.0f)
            {
                return true;
            }
            return false;
        }

        if (m_defaultTXDSizeInMbs < 2.5f)
        {
            if (m_atlasSizeInPercentange >= 230.0f)
            {
                return true;
            }
            return false;
        }

        if (m_atlasSizeInPercentange >= 200.0f)
        {
            return true;
        }
        return false;
    }

    bool IsAtlasWorthIt()
    {
        if (m_setOfIgnoredTextureNameHashes.size() == 0 && m_atlasSizeInMBs > m_defaultTXDSizeInMbs)
        {
            if ((m_atlasSizeInMBs - m_defaultTXDSizeInMbs) >= 2.0f && IsAtlasTooBig())
            {
                return false;
            }
        }
        return true;
    }

    SOptimizedTexture* RemoveHighestSizeOptimizedTextureFromContainer()
    {
        size_t             optimizedTextureWithHighestSizeIndex = 0;
        SOptimizedTexture* optimizedTextureWithHighestSize = m_vecOptimizedTextures->at(0);
        for (size_t textureIndex = 0; textureIndex < m_vecOptimizedTextures->size(); textureIndex++)
        {
            SOptimizedTexture* optimizedTexture = m_vecOptimizedTextures->at(textureIndex);
            unsigned int       textureSizeInBytes = optimizedTexture->m_textureSizeWithinAtlasInBytes;
            if (textureSizeInBytes > optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes)
            {
                optimizedTextureWithHighestSize = optimizedTexture;
                optimizedTextureWithHighestSizeIndex = textureIndex;
            }
        }
        m_vecOptimizedTextures->erase(m_vecOptimizedTextures->begin() + optimizedTextureWithHighestSizeIndex);
        return optimizedTextureWithHighestSize;
    }

    SOptimizedTexture* RemoveHighestSizeOptimizedTextureFromCachedContainer()
    {
        size_t             optimizedTextureWithHighestSizeIndex = 0;
        SOptimizedTexture* optimizedTextureWithHighestSize = m_vecOptimizedTexturesCache[0];
        for (size_t textureIndex = 0; textureIndex < m_vecOptimizedTexturesCache.size(); textureIndex++)
        {
            SOptimizedTexture* optimizedTexture = m_vecOptimizedTexturesCache[textureIndex];
            unsigned int       textureSizeInBytes = optimizedTexture->m_textureSizeWithinAtlasInBytes;
            if (textureSizeInBytes > optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes)
            {
                optimizedTextureWithHighestSize = optimizedTexture;
                optimizedTextureWithHighestSizeIndex = textureIndex;
            }
        }
        m_vecOptimizedTexturesCache.erase(m_vecOptimizedTexturesCache.begin() + optimizedTextureWithHighestSizeIndex);
        return optimizedTextureWithHighestSize;
    }

    void GetAtlasSize()
    {
        unsigned int atlasSizeInBytes = 0;
        for (unsigned int textureIndex = 0; textureIndex < m_pOptimizedDFF->GetTotalOptimizedTextures(); textureIndex++)
        {
            SOptimizedTexture& optimizedTexture = m_pOptimizedDFF->GetOptimizedTexture(textureIndex);
            const unsigned int maxSizeLimitInBytes = 1000 * 1024 * 1024;            // 1000 MB
            if (optimizedTexture.m_textureSizeWithinAtlasInBytes > maxSizeLimitInBytes)
            {
                optimizedTexture.m_textureSizeWithinAtlasInBytes = 0;
            }
            atlasSizeInBytes += optimizedTexture.m_textureSizeWithinAtlasInBytes;
        }

        for (unsigned int textureIndex = 0; textureIndex < m_pOptimizedDFF->GetTotalOptimizedTextures(); textureIndex++)
        {
            SOptimizedTexture& optimizedTexture = m_pOptimizedDFF->GetOptimizedTexture(textureIndex);
            if (optimizedTexture.m_textureSizeWithinAtlasInBytes > atlasSizeInBytes)
            {
                optimizedTexture.m_textureSizeWithinAtlasInBytes = 0;
            }
        }

        m_atlasSizeInMBs = (atlasSizeInBytes / 1024.0f) / 1024.0f;
        m_atlasSizeInPercentange = (m_atlasSizeInMBs * 100.0f) / m_defaultTXDSizeInMbs;
    }

    void ReduceSize()
    {
        if (m_defaultTXDSizeInMbs >= m_atlasSizeInMBs)
        {
            return;
        }

        unsigned int       texturesToIgnore = 0;
        const unsigned int maxTexturesToIgnore = 5;
        SOptimizedTexture* optimizedTextureWithHighestSize = RemoveHighestSizeOptimizedTextureFromContainer();
        float              removedTextureSizeWithinAtlas = ((optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes) / 1024.0f) / 1024.0f;
        while (removedTextureSizeWithinAtlas >= 1.0f && m_vecOptimizedTextures->size() > 0)
        {
            if (m_setOfUsedTextures->size() <= 2)
            {
                printf("WARNING! setOfUsedTextures is %u and requires ignoring textures! m_atlasSizeInMBs: %f\n", m_setOfUsedTextures->size(),
                       m_atlasSizeInMBs);
                break;
            }

            m_totalAtlasSizeReducedInBytes += optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes;
            m_setOfIgnoredTextureNameHashes.insert(optimizedTextureWithHighestSize->m_textureNameHash);

            optimizedTextureWithHighestSize = RemoveHighestSizeOptimizedTextureFromContainer();
            removedTextureSizeWithinAtlas = ((optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes) / 1024.0f) / 1024.0f;
            texturesToIgnore++;
            if (texturesToIgnore >= maxTexturesToIgnore || texturesToIgnore == (m_setOfUsedTextures->size() - 1))
            {
                break;
            }
        }

        if (m_setOfUsedTextures->size() > 2 && !IsAtlasWorthIt())
        {
            // ignore 2 textures max. Hopefully, this will reduce some size....
            unsigned int texturesToIgnoreCount = std::min(m_setOfUsedTextures->size() - 2, (unsigned int)2);
            for (unsigned int i = 0; i < texturesToIgnoreCount; i++)
            {
                SOptimizedTexture* optimizedTextureWithHighestSize = RemoveHighestSizeOptimizedTextureFromCachedContainer();
                m_totalAtlasSizeReducedInBytes += optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes;
                m_setOfIgnoredTextureNameHashes.insert(optimizedTextureWithHighestSize->m_textureNameHash);
            }
        }

        PrintDebuggingData();
    }

    void PrintDebuggingData()
    {
        float atlasSizeReducedInMBs = ((m_totalAtlasSizeReducedInBytes) / 1024.0f) / 1024.0f;
        printf("\nAtlas Size Reduced: %f MBs | Textures Ignored: %u\n\n", atlasSizeReducedInMBs, m_setOfIgnoredTextureNameHashes.size());
        printf("m_atlasSizeInMBs: %f | defaultTXDSizeInMbs: %f | m_atlasSizeInPercentange: %f\n", m_atlasSizeInMBs, m_defaultTXDSizeInMbs,
               m_atlasSizeInPercentange);

        // printf("\atlasSizeInBytes: %u | defaultTXDSizeInBytes: %u | m_atlasSizeInPercentange: %f\n",
        //     atlasSizeInBytes, m_defaultTXDSizeInBytes, m_atlasSizeInPercentange);

        for (unsigned int textureIndex = 0; textureIndex < m_pOptimizedDFF->GetTotalOptimizedTextures(); textureIndex++)
        {
            SOptimizedTexture&    optimizedTexture = m_pOptimizedDFF->GetOptimizedTexture(textureIndex);
            std::set<RwTexture*>& setOfUsedTexturesByReference = *m_setOfUsedTextures;
            for (auto& pTexture : setOfUsedTexturesByReference)
            {
                if (HashString(pTexture->name) == optimizedTexture.m_textureNameHash && optimizedTexture.m_textureSizeWithinAtlasInBytes > 0)
                {
                    float sizeInAtlas = (optimizedTexture.m_textureSizeWithinAtlasInBytes / 1024.0f) / 1024.0f;
                    printf("\n[DEBUG] texture: %s | SizeInAtlas: %f MBs, %u bytes\n\n", pTexture->name, sizeInAtlas,
                           optimizedTexture.m_textureSizeWithinAtlasInBytes);
                    // printf("\n[DEBUG] texture: %s | SizeInAtlas: %u bytes\n\n", pTexture->name, optimizedTexture.m_textureSizeWithinAtlasInBytes);
                    break;
                }
            }
        }
    }

private:
    unsigned int m_totalAtlasSizeReducedInBytes;
    float        m_defaultTXDSizeInMbs;
    float        m_atlasSizeInPercentange;
    float        m_atlasSizeInMBs;

    SOptimizedDFF*                   m_pOptimizedDFF;
    std::vector<SOptimizedTexture*>  m_vecOptimizedTexturesCache;
    std::vector<SOptimizedTexture*>* m_vecOptimizedTextures;
    std::set<RwTexture*>*            m_setOfUsedTextures;
    std::set<unsigned int>           m_setOfIgnoredTextureNameHashes;
};

