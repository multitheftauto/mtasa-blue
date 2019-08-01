#pragma once

// REMOVE THIS LATER AFTER MOVING THE CODING TO the .cpp file
#include "StdInc.h"

class CAtlasSizeReducer
{
public:
    CAtlasSizeReducer(SOptimizedDFF* pOptimizedDFF, unsigned int defaultTXDSizeInBytes, std::vector<SOptimizedTexture*>* vecOptimizedTextures,
                      std::set<RwTexture*>* setOfUsedTextures)
    {
        m_pOptimizedDFF = pOptimizedDFF;
        m_defaultTXDSizeInBytes = defaultTXDSizeInBytes;
        m_vecOptimizedTextures = vecOptimizedTextures;
        m_setOfUsedTextures = setOfUsedTextures;
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

    void ReduceSize() 
    {
        // /*
        unsigned int atlasSizeInBytes = 0;
        float        atlasSizeInMBs = 0.0f;
        float        defaultTXDSizeInMbs = 0.0f;
        float        atlasSizeInPercentange = 0.0f;
        unsigned int texturesToIgnore = 0;
        unsigned int totalAtlasSizeReducedInBytes = 0;
        if (m_pOptimizedDFF)
        {
            defaultTXDSizeInMbs = (m_defaultTXDSizeInBytes / 1024.0f) / 1024.0f;
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

            atlasSizeInMBs = (atlasSizeInBytes / 1024.0f) / 1024.0f;
            atlasSizeInPercentange = (atlasSizeInMBs * 100.0f) / defaultTXDSizeInMbs;

            if (defaultTXDSizeInMbs >= atlasSizeInMBs)
            {
                return;
            }

            const unsigned int maxTexturesToIgnore = 5;
            SOptimizedTexture* optimizedTextureWithHighestSize = RemoveHighestSizeOptimizedTextureFromContainer();
            float              removedTextureSizeWithinAtlas = ((optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes) / 1024.0f) / 1024.0f;
            while (removedTextureSizeWithinAtlas >= 1.0f && m_vecOptimizedTextures->size() > 0)
            {
                if (m_setOfUsedTextures->size() <= 2)
                {
                    printf("WARNING! setOfUsedTextures is %u and requires ignoring textures! atlasSizeInMBs: %f\n", m_setOfUsedTextures->size(),
                           atlasSizeInMBs);
                    break;
                }

                totalAtlasSizeReducedInBytes += optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes;
                m_setOfIgnoredTextureNameHashes.insert(optimizedTextureWithHighestSize->m_textureNameHash);
                //IgnoreTexture(optimizedTextureWithHighestSize->m_textureNameHash);

                optimizedTextureWithHighestSize = RemoveHighestSizeOptimizedTextureFromContainer();
                removedTextureSizeWithinAtlas = ((optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes) / 1024.0f) / 1024.0f;
                texturesToIgnore++;
                if (texturesToIgnore >= maxTexturesToIgnore || texturesToIgnore == (m_setOfUsedTextures->size() - 1))
                {
                    break;
                }
            }
        }
        //*/

        float atlasSizeReducedInMBs = ((totalAtlasSizeReducedInBytes) / 1024.0f) / 1024.0f;
        printf("\nAtlas Size Reduced: %f MBs | Textures Ignored: %u | texturesToIgnore variable: %u\n\n", atlasSizeReducedInMBs,
               m_setOfIgnoredTextureNameHashes.size(), texturesToIgnore);
        if (m_pOptimizedDFF)
        {
            /*
            unsigned int atlasSizeInBytes = 0;
            float        atlasSizeInMBs = 0.0f;
            float        defaultTXDSizeInMbs = (m_defaultTXDSizeInBytes / 1024.0f) / 1024.0f;
            for (unsigned int textureIndex = 0; textureIndex < m_pOptimizedDFF->GetTotalOptimizedTextures(); textureIndex++)
            {
                SOptimizedTexture& optimizedTexture = m_pOptimizedDFF->GetOptimizedTexture(textureIndex);
                const unsigned int maxSizeLimitInBytes = 1000 * 1024 * 1024;            // 1000 MB
                if (optimizedTexture.m_textureSizeWithinAtlasInBytes > maxSizeLimitInBytes)
                {
                    optimizedTexture.m_textureSizeWithinAtlasInBytes = 0;
                }
                atlasSizeInBytes += optimizedTexture.m_textureSizeWithinAtlasInBytes;
                m_vecOptimizedTextures.emplace_back(&optimizedTexture);
            }

            atlasSizeInMBs = (atlasSizeInBytes / 1024.0f) / 1024.0f;
            float atlasSizeInPercentange = (atlasSizeInMBs * 100.0f) / defaultTXDSizeInMbs;
            */
            printf("atlasSizeInMBs: %f | defaultTXDSizeInMbs: %f | atlasSizeInPercentange: %f\n", atlasSizeInMBs, defaultTXDSizeInMbs, atlasSizeInPercentange);

            // printf("\atlasSizeInBytes: %u | defaultTXDSizeInBytes: %u | atlasSizeInPercentange: %f\n",
            //     atlasSizeInBytes, m_defaultTXDSizeInBytes, atlasSizeInPercentange);

            for (unsigned int textureIndex = 0; textureIndex < m_pOptimizedDFF->GetTotalOptimizedTextures(); textureIndex++)
            {
                SOptimizedTexture& optimizedTexture = m_pOptimizedDFF->GetOptimizedTexture(textureIndex);
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
    }
private:
    SOptimizedDFF* m_pOptimizedDFF;
    unsigned int   m_defaultTXDSizeInBytes;
    std::vector<SOptimizedTexture*>* m_vecOptimizedTextures;
    std::set<RwTexture*>*            m_setOfUsedTextures;
    std::set<unsigned int>           m_setOfIgnoredTextureNameHashes;
};
