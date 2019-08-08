//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwErrorSet(char* message, ...)
{
    char    text[512];
    va_list ap;
    va_start(ap, message);
    vsnprintf(text, 512, message, ap);
    va_end(ap);
    MessageBoxA(NULL, text, "gtaRwApi", 0);
    return rwTRUE;
}

gtaRwStream *gtaRwStreamOpen(gtaRwStreamType type, gtaRwStreamAccessType accessType, const void *pData) {
    gtaRwStream *stream = (gtaRwStream *)gtaRwMemAllocAndZero(sizeof(gtaRwStream));
    if (stream) {
        stream->type = type;
        stream->accessType = accessType;
        stream->rwOwned = 1;
        switch (type) {
        case rwSTREAMFILE:
            if (ftell((FILE *)pData) == -1) {
                gtaRwMemFree(stream);
                return rwNULL;
            }
            stream->Type.file.fpFile = (void *)pData;
            break;
        case rwSTREAMFILENAME:
            switch (accessType) {
            case rwSTREAMREAD:
                stream->Type.file.fpFile = fopen((char *)pData, "rb");
                break;
            case rwSTREAMWRITE:
                stream->Type.file.fpFile = fopen((char *)pData, "wb");
                break;
            case rwSTREAMAPPEND:
                stream->Type.file.fpFile = fopen((char *)pData, "ab");
                break;
            default:
                gtaRwError("Invalid stream access type.");
                gtaRwMemFree(stream);
                return rwNULL;
            }
            if (!stream->Type.file.fpFile) {
                gtaRwError("Error opening the file %s", pData);
                gtaRwMemFree(stream);
                return rwNULL;
            }
            break;
        case rwSTREAMMEMORY:
            switch (accessType) {
            case rwSTREAMREAD:
                stream->Type.memory.position = 0;
                stream->Type.memory.nSize = ((gtaRwMemory *)pData)->length;
                stream->Type.memory.memBlock = ((gtaRwMemory *)pData)->start;
                break;
            case rwSTREAMWRITE:
                stream->Type.memory.position = 0;
                stream->Type.memory.nSize = 0;
                stream->Type.memory.memBlock = 0;
                break;
            case rwSTREAMAPPEND:
                stream->Type.memory.position = ((gtaRwMemory *)pData)->length;
                stream->Type.memory.nSize = ((gtaRwMemory *)pData)->length;
                stream->Type.memory.memBlock = ((gtaRwMemory *)pData)->start;
                break;
            default:
                gtaRwError("Invalid stream access type.");
                gtaRwMemFree(stream);
                return rwNULL;
            }
            break;
        case rwSTREAMCUSTOM:
            gtaRwMemCopy(&stream->Type, pData, sizeof(gtaRwStreamCustom));
            break;
        default:
            gtaRwError("Invalid stream type.");
            gtaRwMemFree(stream);
            return rwNULL;
        }
        return stream;
    }
    else
        return rwNULL;
}

gtaRwBool gtaRwStreamClose(gtaRwStream *stream, void *pData) {
    gtaRwBool result;
    switch (stream->type) {
    case rwSTREAMFILENAME:
        result = fclose((FILE *)stream->Type.file.fpFile) == 0;
        break;
    case rwSTREAMMEMORY:
        result = rwTRUE;
        if (stream->accessType != rwSTREAMREAD && pData) {
            ((gtaRwMemory *)pData)->start = stream->Type.memory.memBlock;
            ((gtaRwMemory *)pData)->length = stream->Type.memory.position;
        }
        break;
    case rwSTREAMCUSTOM:
        if (stream->Type.custom.sfnclose)
            stream->Type.custom.sfnclose(stream->Type.custom.data);
        result = rwTRUE;
        break;
    case rwSTREAMFILE:
        result = rwTRUE;
        break;
    default:
        gtaRwError("Invalid stream type.");
        gtaRwMemFree(stream);
        return rwNULL;
    }
    if (stream->rwOwned)
        gtaRwMemFree(stream);
    return result;
}

gtaRwUInt32 gtaRwStreamRead(gtaRwStream *stream, void *buffer, gtaRwUInt32 length) {
    gtaRwUInt32 numBytesRead;
    switch (stream->type) {
    case rwSTREAMFILE:
    case rwSTREAMFILENAME:
        numBytesRead = fread(buffer, 1, length, (FILE *)stream->Type.file.fpFile);
        if (numBytesRead != length) {
            if (feof((FILE *)stream->Type.file.fpFile))
                gtaRwError("At the end of the stream.");
            else
                gtaRwError("Read error on stream");
        }
        return numBytesRead;
    case rwSTREAMMEMORY:
        if (length > stream->Type.memory.nSize - stream->Type.memory.position) {
            gtaRwError("At the end of the stream.");
            return stream->Type.memory.nSize - stream->Type.memory.position;
        }
        else {
            gtaRwMemCopy(buffer, (const void *)(stream->Type.memory.position + stream->Type.memory.memBlock), length);
            stream->Type.memory.position += length;
            return length;
        }
    case rwSTREAMCUSTOM:
        return stream->Type.custom.sfnread(stream->Type.custom.data, buffer, length);
    }
    gtaRwError("Invalid stream type.");
    return 0;
}

gtaRwStream *gtaRwStreamSkip(gtaRwStream *stream, gtaRwUInt32 offset) {
    gtaRwStream *result; gtaRwUInt32 newPosn;
    if (offset) {
        switch (stream->type) {
        case rwSTREAMFILE:
        case rwSTREAMFILENAME:
            if (fseek((FILE *)stream->Type.file.fpFile, offset, 1)) {
                if (feof((FILE *)stream->Type.file.fpFile))
                    gtaRwError("At the end of the stream.");
                result = rwNULL;
            }
            else
                result = stream;
            break;
        case rwSTREAMMEMORY:
            newPosn = stream->Type.memory.position + offset;
            if (newPosn <= stream->Type.memory.nSize) {
                stream->Type.memory.position = newPosn;
                result = stream;
            }
            else {
                gtaRwError("At the end of the stream.");
                result = rwNULL;
            }
            break;
        case rwSTREAMCUSTOM:
            result = stream->Type.custom.sfnskip(stream->Type.custom.data, offset) != rwNULL ? stream : rwNULL;
            break;
        default:
            gtaRwError("Invalid stream type.");
            result = rwNULL;
        }
    }
    else
        result = stream;
    return result;
}

gtaRwBool gtaRwStreamReadChunkHeader(gtaRwStream *stream, gtaRwUInt32 *pType, gtaRwUInt32 *pLength, gtaRwUInt32 *pVersion, gtaRwUInt32 *pBuild) {
    gtaRwUInt32 buffer[3];
    gtaRwChunkHeaderInfo chunkHeaderInfo;
    if (gtaRwStreamRead(stream, &buffer, 12) == 12) {
        chunkHeaderInfo.type = buffer[0];
        chunkHeaderInfo.length = buffer[1];
        if (buffer[2] & 0xFFFF0000) {
            chunkHeaderInfo.version = (buffer[2] >> 16) & 0x3F | (((buffer[2] >> 14) & 0x3FF00) + 0x30000);
            chunkHeaderInfo.buildNum = (gtaRwUInt16)buffer[2];
        }
        else {
            chunkHeaderInfo.version = buffer[2] << 8;
            chunkHeaderInfo.buildNum = 0;
        }
        if (pType)
            *pType = chunkHeaderInfo.type;
        if (pLength)
            *pLength = chunkHeaderInfo.length;
        if (pBuild)
            *pBuild = chunkHeaderInfo.buildNum;
        if (pVersion)
            *pVersion = chunkHeaderInfo.version;
        return rwTRUE;
    }
    else {
        gtaRwError("Read error on stream");
        return rwFALSE;
    }
}

gtaRwBool gtaRwStreamFindChunk(gtaRwStream *stream, gtaRwUInt32 type, gtaRwUInt32 *lengthOut, gtaRwUInt32 *versionOut, gtaRwUInt32 *buildOut) {
    gtaRwUInt32 outType, outLength, outVersion, outBuild;
    if (!gtaRwStreamReadChunkHeader(stream, &outType, &outLength, &outVersion, &outBuild))
        return rwFALSE;
    while (outType != type) {
        if (!gtaRwStreamSkip(stream, outLength))
            return rwFALSE;
        if (!gtaRwStreamReadChunkHeader(stream, &outType, &outLength, &outVersion, rwNULL))
            return rwFALSE;
    }
    if (lengthOut)
        *lengthOut = outLength;
    if (versionOut)
        *versionOut = outVersion;
    if (buildOut)
        *buildOut = outBuild;
    return rwTRUE;
}

gtaRwStream *gtaRwStreamReadReal(gtaRwStream *stream, gtaRwReal *reals, gtaRwUInt32 numBytes) {
    if (gtaRwStreamRead(stream, reals, numBytes))
        return stream;
    else {
        gtaRwError("Read error on stream");
        return rwNULL;
    }
}

gtaRwStream *gtaRwStreamWrite(gtaRwStream *stream, const void *buffer, gtaRwUInt32 length) {
    void *newBlock;
    switch (stream->type) {
    case rwSTREAMFILE:
    case rwSTREAMFILENAME:
        if (fwrite(buffer, 1, length, (FILE *)stream->Type.file.fpFile) != length) {
            gtaRwError("Write error on stream");
            return rwNULL;
        }
        break;
    case rwSTREAMMEMORY:
        if (!stream->Type.memory.memBlock) {
            stream->Type.memory.memBlock = (gtaRwUInt8 *)gtaRwMemAllocAndZero(512);
            if (!stream->Type.memory.memBlock) {
                gtaRwError("Unable to allocate %d bytes of memory", 512);
                return rwNULL;
            }
            stream->Type.memory.nSize = 512;
        }
        if (stream->Type.memory.nSize - stream->Type.memory.position < length) {
            newBlock = (gtaRwUInt8 *)realloc(stream->Type.memory.memBlock, length >= 512 ?
                (stream->Type.memory.nSize + length) : (stream->Type.memory.nSize + 512));
            if (!newBlock) {
                gtaRwError("Unable to allocate %d bytes of memory", length >= 512 ?
                    stream->Type.memory.nSize + length : stream->Type.memory.nSize + 512);
                return rwNULL;
            }
            stream->Type.memory.memBlock = newBlock;
            stream->Type.memory.nSize = length >= 512 ? stream->Type.memory.nSize + length : stream->Type.memory.nSize + 512;
        }
        gtaRwMemCopy((void *)(stream->Type.memory.position + stream->Type.memory.memBlock), buffer, length);
        stream->Type.memory.position += length;
        break;
    case 4:
        if (!stream->Type.custom.sfnwrite(stream->Type.custom.data, buffer, length))
            return rwNULL;
        break;
    default:
        gtaRwError("Invalid stream type.");
        return rwNULL;
    }
    return stream;
}

gtaRwStream *gtaRwStreamWriteVersionedChunkHeader(gtaRwStream *stream, gtaRwUInt32 type, gtaRwUInt32 size, gtaRwUInt32 version, gtaRwUInt16 buildNum) {
    gtaRwUInt32 data[3];
    data[0] = type;
    data[1] = size;
    data[2] = buildNum | ((version & 0x3F) << 16) | ((version + 0x10000) << 14) & 0xFFC00000;
    return gtaRwStreamWrite(stream, data, 12);
}

gtaRwStream *gtaRwStreamWriteInt32(gtaRwStream *stream, gtaRwUInt32 *integers, gtaRwUInt32 length) {
    gtaRwStreamWrite(stream, integers, length);
    return stream;
}
