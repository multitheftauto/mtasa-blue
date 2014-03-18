/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpMaterial.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  See http://www.gtamodding.com/index.php?title=Material_%28RW_Section%29
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_MATERIAL_
#define _RENDERWARE_MATERIAL_

struct RpMaterialLighting
{
    float ambient, specular, diffuse;
};
struct CEnvMapMaterialSA //size: 12 bytes
    // see http://www.gtamodding.com/index.php?title=Reflection_Material_%28RW_Section%29
{
    char                envMod;             // 0
    char                envMod2;            // 1

    char                envMod3;            // 2
    char                envMod4;            // 3

    unsigned char       envMapBrightness;   // 4

    BYTE                pad3[1];            // 5

    unsigned short      updateFrame;        // 6
    RwTexture*          envTexture;         // 8
};
struct CSpecMapMaterialSA    //size: 8 bytes
    // see http://www.gtamodding.com/index.php?title=Specular_Material_%28RW_Section%29
{
    float               specular;           // 0
    RwTexture*          specMap;            // 4
};
struct matFXBlendData
{
    RwTexture*          fxTexture;          // 0
    unsigned int        srcBlend;           // 4
    unsigned int        dstBlend;           // 8
    BYTE                pad[12];            // 12
};
struct CSpecialFXMatSA
{
    union
    {
        struct
        {
            BYTE                pad[4];             // 0
            RwTexture*          bumpTexture;        // 4
            BYTE                pad2[20];           // 8
            RwTexture*          bumpTexture2;       // 28
        } bumpMapEffect;
        struct
        {
            RwMatrix*           fxMatrix;           // 0
        } effect5;
        struct
        {
            RwMatrix*           fxMatrix;           // 0
            RwMatrix*           fxMatrix2;          // 4
            BYTE                pad[16];            // 8
            matFXBlendData      fxBlend;            // 24
        } effect6;

        matFXBlendData      effect4;                // 0

        BYTE                pad[48];                // 0
    };

    unsigned int        effectType;                 // 48
};
//padlevel: 5
struct RpMaterial
{
    RwTexture*          texture;        // 0
    RwColor             color;          // 4
    void*               render;         // 8
    union
    {
        RpMaterialLighting  lighting;       // 12
        
        // Shader data.
        struct
        {
            BYTE            pad4[4];        // 12
            unsigned int    shaderFlags;    // 16, hack by Rockstar
            BYTE            pad5[4];        // 20
        };
    };
    unsigned short      refs;           // 24
    short               id;             // 26

    CSpecialFXMatSA*    specialFX;      // 28, plugin struct of 4 bytes

    BYTE                pad[40];        // 32

    CEnvMapMaterialSA*  envMapMat;      // 72, env map material plugin, allocated from pool
    CSpecMapMaterialSA* specMapMat;     // 76, specular map material plugin, allocated from pool
    
    void                SetTexture      ( RwTexture *tex );
};
struct RpMaterials
{
                    RpMaterials         ( unsigned int count );
                    ~RpMaterials        ( void );

    bool            Add                 ( RpMaterial *mat );

    RpMaterial**    data;
    unsigned int    entries;
    unsigned int    max;
};

#endif //_RENDERWARE_MATERIAL_