//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwTexDictionary gtaRwTexDictionary;

struct gtaRwTexDictionary {
    gtaRwUInt32 version;
    gtaRwUInt32 build;

    gtaRwUInt16 numTextures;
    gtaRwUInt8 deviceId;
    gtaRwInt8 _zero;
    gtaRwTextureD3D *textures;

    // extensions

    gtaRwUnknownExtension *unknownExtensions;
    gtaRwUInt32 numUnknownExtensions;
};

void gtaRwTexDictionaryInit(gtaRwTexDictionary *txdObj, gtaRwUInt16 numTextures);
void gtaRwTexDictionaryDestroy(gtaRwTexDictionary *txdObj);
gtaRwBool gtaRwTexDictionaryRead(gtaRwTexDictionary *txdObj, gtaRwStream *stream);
gtaRwBool gtaRwTexDictionaryWrite(gtaRwTexDictionary *txdObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwTexDictionarySize(gtaRwTexDictionary *txdObj);
