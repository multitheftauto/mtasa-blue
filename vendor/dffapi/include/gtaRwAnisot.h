//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwTextureAnisot gtaRwTextureAnisot;

struct gtaRwTextureAnisot {
    gtaRwBool enabled;
    gtaRwUInt32 anisotropyLevel;
};

gtaRwBool gtaRwTextureAnisotWrite(gtaRwTextureAnisot *anisotObj, gtaRwStream *stream);
gtaRwBool gtaRwTextureAnisotRead(gtaRwTextureAnisot *anisotObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwTextureAnisotSize(gtaRwTextureAnisot *anisotObj);
void gtaRwTextureAnisotInit(gtaRwTextureAnisot *anisotObj, gtaRwUInt32 anisotLevel);
void gtaRwTextureAnisotDestroy(gtaRwTextureAnisot *anisotObj);
