//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwUnknownExtension gtaRwUnknownExtension;

struct gtaRwUnknownExtension {
    gtaRwBool enabled;
    gtaRwUInt32 id;
    gtaRwUInt8 *data;
    gtaRwUInt32 dataSize;
};

gtaRwBool gtaRwUnknownExtensionWrite(gtaRwUnknownExtension *extObj, gtaRwStream *stream);
gtaRwBool gtaRwUnknownExtensionRead(gtaRwUnknownExtension *extObj, gtaRwStream *stream, gtaRwUInt32 id, gtaRwUInt32 dataSize);
gtaRwUInt32 gtaRwUnknownExtensionSize(gtaRwUnknownExtension *extObj);
void gtaRwUnknownExtensionInit(gtaRwUnknownExtension *extObj, gtaRwUInt32 id, gtaRwUInt8 *data, gtaRwUInt32 dataSize);
void gtaRwUnknownExtensionDestroy(gtaRwUnknownExtension *extObj);
gtaRwUInt32 gtaRwUnknownExtensionsSize(gtaRwUnknownExtension *extObjects, gtaRwUInt32 numExtensions);
gtaRwBool gtaRwUnknownExtensionsWrite(gtaRwUnknownExtension *extObjects, gtaRwUInt32 numExtensions, gtaRwStream *stream);
gtaRwBool gtaRwUnknownExtensionAllocateAndRead(gtaRwUnknownExtension **ppExtObjects, gtaRwUInt32 *numExtensions, gtaRwUInt32 type, gtaRwUInt32 entryLength, gtaRwStream *stream);
