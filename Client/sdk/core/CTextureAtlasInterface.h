#pragma once

class CTextureInfo
{
public:

    CTextureInfo(const SString& theFilePath, const float theWidth, const float theHeight)
    {
        m_strFilePath = theFilePath;
        m_fWidth = theWidth; m_fHeight = theHeight;
        memset(&m_Region, 0, sizeof(RECT));
        m_pTexture = nullptr;
    }

    const SString& GetFilePath() { return m_strFilePath; }
    float GetWidth() { return m_fWidth; }
    float GetHeight() { return m_fHeight; }
    RECT& GetRegion() { return m_Region; }
    IDirect3DTexture9*& GetTexture() { return m_pTexture;  }

    void SetTexture(IDirect3DTexture9* ptheTexture) { m_pTexture = ptheTexture; }

private:

    SString m_strFilePath;
    float m_fWidth, m_fHeight;
    RECT  m_Region;
    IDirect3DTexture9* m_pTexture;
};


class CTextureAtlasInterface
{
public:
    virtual IDirect3DTexture9* GetAtlasTexture() = 0;
    virtual const CVector2D& GetSize() = 0;
    virtual float GetWidth() = 0;
    virtual float GetHeight() = 0;
};
