//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwMaterial gtaRwMaterial;

struct gtaRwMaterial {
    gtaRwInt32 flags;   // Material flags - unused currently - for future expansion
    gtaRwRGBA color;    // Color of material
    gtaRwInt32 unused;  // Not used
    gtaRwBool textured; // Are we textured?
    gtaRwSurfaceProperties surfaceProps; // Surface properties
    gtaRwMaterialTexture texture;

    // Extensions

    gtaRwMaterialUVAnim uvAnim;
    gtaRwMaterialMatFX matFx;
    gtaRwMaterialNormalMap normalMap;
    gtaRwMaterialEnvMap envMap;
    gtaRwMaterialSpecMap specMap;

    gtaRwUnknownExtension *unknownExtensions;
    gtaRwUInt32 numUnknownExtensions;

    // TODO: rights
};

gtaRwBool gtaRwMaterialRead(gtaRwMaterial *matObj, gtaRwStream *stream);
gtaRwBool gtaRwMaterialWrite(gtaRwMaterial *matObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMaterialSize(gtaRwMaterial *matObj);
void gtaRwMaterialInit(gtaRwMaterial *matObj, gtaRwUInt8 red, gtaRwUInt8 green, gtaRwUInt8 blue, gtaRwUInt8 alpha, gtaRwBool textured, gtaRwReal ambient, gtaRwReal diffuse);
void gtaRwMaterialDestroy(gtaRwMaterial *matObj);
