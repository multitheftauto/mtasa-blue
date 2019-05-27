//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwTextureSkyMipmap gtaRwTextureSkyMipmap;

struct gtaRwTextureSkyMipmap {
    gtaRwBool enabled;
    union {
        gtaRwInt32 klValues;
        struct {
            gtaRwInt32 kValue : 12;
            gtaRwInt32 lValue : 2;
        };
    };
};

void gtaRwTextureSkyMipmapSetK(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwReal k);
void gtaRwTextureSkyMipmapSetL(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwUInt32 l);
gtaRwReal gtaRwTextureSkyMipmapGetK(gtaRwTextureSkyMipmap *skyMipmapObj);
gtaRwUInt32 gtaRwTextureSkyMipmapGetL(gtaRwTextureSkyMipmap *skyMipmapObj);
gtaRwBool gtaRwTextureSkyMipmapWrite(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwStream *stream);
gtaRwBool gtaRwTextureSkyMipmapRead(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwTextureSkyMipmapSize(gtaRwTextureSkyMipmap *skyMipmapObj);
void gtaRwTextureSkyMipmapInit(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwInt32 kValue, gtaRwUInt32 lValue);
void gtaRwTextureSkyMipmapInitWithUncompressedKL(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwReal kValue, gtaRwUInt32 lValue);
void gtaRwTextureSkyMipmapDestroy(gtaRwTextureSkyMipmap *skyMipmapObj);
