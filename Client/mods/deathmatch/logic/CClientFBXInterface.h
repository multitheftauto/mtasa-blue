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
    virtual void             CreateTexture(SString strTextureName, CPixels* pPixels) = 0;
    virtual CMaterialItem*   GetTextureByName(SString strTextureName) = 0;
    virtual bool             IsTextureCreated(SString strTextureName) = 0;
};
