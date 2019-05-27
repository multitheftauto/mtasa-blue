//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwStringInit(gtaRwString *strObj, gtaRwChar const *strData, gtaRwBool isUnicode) {
    gtaRwStringDestroy(strObj);
    if (strData) {
        strObj->string = (gtaRwChar *)gtaRwMemAllocAndZero(strlen(strData) + 1);
        strcpy(strObj->string, strData);
    }
    else
        strObj->string = rwNULL;
    strObj->isUnicode = isUnicode;
}

void gtaRwStringDestroy(gtaRwString *strObj) {
    if (strObj) {
        if (strObj->string)
            gtaRwMemFree(strObj->string);
        gtaRwMemZero(strObj, sizeof(gtaRwString));
    }
}

gtaRwBool gtaRwStringRead(gtaRwString *strObj, gtaRwStream *stream) {
    gtaRwUInt32 type, length;
    gtaRwChar *data;
    gtaRwStringDestroy(strObj);
    if (gtaRwStreamReadChunkHeader(stream, &type, &length, rwNULL, rwNULL)) {
        while (type != rwID_STRING && type != rwID_UNICODESTRING) {
            if (!gtaRwStreamSkip(stream, length))
                return rwFALSE;
            if (!gtaRwStreamReadChunkHeader(stream, &type, &length, rwNULL, rwNULL))
                return rwFALSE;
        }
        data = (gtaRwChar *)gtaRwMemAllocAndZero(length + 1);
        if (gtaRwStreamRead(stream, data, length) != length) {
            gtaRwMemFree(data);
            return rwFALSE;
        }
        if (type == rwID_UNICODESTRING) {
            gtaRwInt32 i;
            for (i = 0; ((gtaRwUInt16 *)data)[i]; i++)
                data[i] = (gtaRwChar)(((gtaRwUInt16 *)data)[i]);
            data[i] = '\0';
            strObj->isUnicode = rwTRUE;
        }
        length = strlen(data);
        strObj->string = (gtaRwChar *)gtaRwMemAlloc(length + 1);
        strcpy(strObj->string, data);
        gtaRwMemFree(data);
        return rwTRUE;
    }
    return rwFALSE;
}

gtaRwBool gtaRwStringWrite(gtaRwString *strObj, gtaRwStream *stream) {
    gtaRwUInt32 length;
    gtaRwChar *data;
    if (strObj->isUnicode) {
        if (!strObj->string)
            length = 4;
        else
            length = ((strlen(strObj->string) * 2) + 4) & ~3;
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_UNICODESTRING, length, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        data = (gtaRwChar *)gtaRwMemAllocAndZero(length);
        memset(data, 0, length);
        if (strObj->string) {
            for (gtaRwInt32 i = 0; strObj->string[i]; i++)
                ((gtaRwUInt16 *)data)[i] = strObj->string[i];
        }
    }
    else {
        if (!strObj->string)
            length = 4;
        else
            length = (strlen(strObj->string) + 4) & ~3;
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRING, length, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        data = (gtaRwChar *)gtaRwMemAllocAndZero(length);
        memset(data, 0, length);
        if (strObj->string)
            strcpy(data, strObj->string);
    }
    if (!gtaRwStreamWrite(stream, data, length)) {
        gtaRwMemFree(data);
        return rwFALSE;
    }
    gtaRwMemFree(data);
    return rwTRUE;
}

gtaRwUInt32 gtaRwStringSize(gtaRwString *strObj) {
    if (!strObj->string)
        return 16;
    else {
        if (strObj->isUnicode)
            return (((strlen(strObj->string) * 2) + 4) & ~3) + 12;
        else
            return ((strlen(strObj->string) + 4) & ~3) + 12;
    }
}
