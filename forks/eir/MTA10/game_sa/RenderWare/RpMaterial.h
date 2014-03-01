/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpMaterial.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
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
{
    float               specular;           // 0
    BYTE                pad[4];             // 4
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
    void*               unknown;        // 28

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