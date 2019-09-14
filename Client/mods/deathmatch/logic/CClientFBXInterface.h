/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientFBXInterface.h
 *  PURPOSE:     Interface for CClientFBX
 *
 *****************************************************************************/

class CClientMaterial;

class CClientFBXInterface
{
public:
    virtual void                                                         CreateTexture(unsigned long long ullTextureId, CPixels* pPixels) = 0;
    virtual CMaterialItem*                                               GetTextureById(unsigned long long ullTextureId) = 0;
    virtual bool                                                         IsTextureCreated(unsigned long long ullTextureId) = 0;
    virtual std::unordered_map<unsigned long long, std::vector<CMatrix>> GetTemplatesRenderingMatrix() = 0;
    virtual bool                                                         IsLoaded() = 0;
    virtual void                                                         Render() = 0;
};
