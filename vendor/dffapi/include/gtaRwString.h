//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwString gtaRwString;

struct gtaRwString {
    gtaRwChar *string;
    gtaRwBool isUnicode;
};

void gtaRwStringInit(gtaRwString *strObj, gtaRwChar const *strData, gtaRwBool isUnicode);
void gtaRwStringDestroy(gtaRwString *strObj);
gtaRwBool gtaRwStringRead(gtaRwString *strObj, gtaRwStream *stream);
gtaRwBool gtaRwStringWrite(gtaRwString *strObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwStringSize(gtaRwString *strObj);
