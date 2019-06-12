#pragma once

#include "..\game\RenderWare.h"
enum eTextureAtlasErrorCodes
{
    TEX_ATLAS_SUCCESS = 0,                    // okay, created atlas
    TEX_ATLAS_CANT_FIT_INTO_ATLAS,            // unable to fit textures into atlas, we need more than one atlas in this case
    TEX_ATLAS_DX_ERROR,                       // failed due to directX error
    TEX_ATLAS_TEXTURE_RESOURCE_CREATE_FAILED, // failed to create atlas texture resource 
    TEX_ATLAS_COPY_TO_ATLAS_FAILED            // failed to copy the texture to atlas
};

struct RwTexture;
class CTextureAtlas;

class CTextureInfo
{
public:

    CTextureInfo(RwTexture* pTexture, const float theWidth, const float theHeight, CTextureAtlas* pTextureAtlas)
    {
        m_fWidth = theWidth; m_fHeight = theHeight;
        memset(&m_Region, 0, sizeof(RECT));
        m_pTexture = pTexture;
        m_uiNameHash = HashString(m_pTexture->name);
        m_pTextureAtlas = pTextureAtlas;
    }

    void SetTexture(RwTexture* ptheTexture)
    {
        m_pTexture = ptheTexture;
        m_uiNameHash = HashString(m_pTexture->name);
    }

    CTextureAtlas* GetAtlas() { return m_pTextureAtlas; }
    unsigned int GetNameHash() { return m_uiNameHash; }
    float GetWidth() { return m_fWidth; }
    float GetHeight() { return m_fHeight; }
    RECT& GetRegion() { return m_Region; }
    RwTexture* GetTexture() { return m_pTexture;  }

private:
    CTextureAtlas * m_pTextureAtlas;
    unsigned int m_uiNameHash;
    float m_fWidth, m_fHeight;
    RECT  m_Region;
    RwTexture* m_pTexture;

};

class CTextureAtlasInterface
{
public:
 /*   virtual RwTexture* GetAtlasTexture() = 0;
    virtual const CVector2D& GetSize() = 0;
    virtual float GetWidth() = 0;
    virtual float GetHeight() = 0;*/
};

