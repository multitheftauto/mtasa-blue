/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientGif.h"

#define GIF_ALLOCATE(pointer,size,release) pointer = (uint8_t*)realloc((release) ? 0 : pointer,(release) ? size : 0UL)
#define BIGE 0
#define _SWAP(h) ((BIGE) ? ((uint16_t)(h << 8) | (h << 8)) : h)

CClientGif::CClientGif(CClientManager* pManager, ElementID ID, CGifItem* p_GifItem)
    : ClassInit(this), CClientTexture(pManager, ID, p_GifItem)
{
    SetTypeName("gif");
    m_pManager = pManager;
    stride = GetRenderItem()->m_uiSizeX * 4;
    UpdateTick();
    m_pGifDisplay = std::make_unique<CClientGifDisplay>(m_pManager->GetDisplayManager(), this);
}

CClientGif::~CClientGif(){
    Unlink();
}

void CClientGif::Register(std::vector<unsigned char*>&& frms, std::vector<int>&& dls) {
    frames = std::move(frms);
    delays = std::move(dls);
    defaultDelays = delays;
    m_pGifDisplay->UpdateTexture();
}

void CClientGif::Next(){
    if (showing >= GetImageCount()) {
        showing = 1;
    }else{
        showing++;
    }
}

void CClientGif::Unlink(){
    for (unsigned char* frame : frames) {
        if (frame) {
            delete[] frame;
        }
    }
    frames.clear();
    delays.clear();
    defaultDelays.clear();
    m_bIsDestoryed = true;
    CClientRenderElement::Unlink();
}

CClientGifLoader::CClientGifLoader(SString& data) {
    buffer = reinterpret_cast<uint8_t*>(data.data());
    size = (long)((unsigned long)data.size());
}

CClientGifLoader::CClientGifLoader(uint8_t* data,unsigned long dataSize) {
    buffer = data;
    size = (long)dataSize;
}

CClientGifLoader::CClientGifLoader(char* data) {
    buffer = reinterpret_cast<uint8_t*>(data);
    size = (long)((unsigned long)strlen(data));
}

void CClientGifLoader::CreateFrame(std::vector<unsigned char*>& frames, std::vector<int>& delays) {
    long frameStride = width*4;
    long frameSize = frameStride*height;
    unsigned char* frame = new unsigned char[frameSize];
    memset(frame, 0, frameSize);
    uint32_t x;
    uint32_t y;
    uint32_t yoffset;
    uint32_t iterator;
    uint32_t inter;
    uint32_t source;
    uint32_t dstSource;
    uint32_t* pDecoder;
    uint32_t* pPrevious;
    #define ARGB(i) ( \
        (address[i] == transparent) ? \
        0 : \
        ( \
            (uint32_t)pallete[address[i]].r << 16 | \
            (uint32_t)pallete[address[i]].g << 8 | \
            (uint32_t)pallete[address[i]].b << 0 | \
            0xff000000 \
        ) \
    )
    if (!frameNumber) {
        frameCount = ((frameCount < 0) ? -frameCount : frameCount)*height;
        frameCount = (frameCount < 0xffff) ? frameCount : 0xffff;
        dstSource = (uint32_t)(width*height);
        palleteDecoder = calloc(sizeof(uint32_t),dstSource);
        palleteDecoderPrevious = calloc(sizeof(uint32_t),dstSource);
    }
    pDecoder = (uint32_t*)palleteDecoder;
    dstSource = (uint32_t)(width*rect.top + rect.left);
    inter = (!(iterator = interlaced ? 0 : 4)) ? 4 : 5;
    for (source = -1; iterator < inter; iterator++) {
        for (yoffset = 16U >> ((iterator > 1) ? iterator : 0), y = (8 >> iterator) & 7; y < (uint32_t)rect.height; y += yoffset) {
            for (x = 0; x < (uint32_t)rect.width; x++) {
                if (transparent != (long)address[++source]) {
                    pDecoder[(uint32_t)width*y + x + dstSource] = ARGB(source);
                }
            }
        }
    }
    memcpy((uint32_t*)frame, pDecoder, sizeof(uint32_t)*(uint32_t)width*(uint32_t)height); // copy pixels to frame
    if ((mode == CGif_Previous) && !last) {
        rect.width = width;
        rect.height = height;
        mode = CGif_Background;
        dstSource = 0;
    }else{
        last = (mode == CGif_Previous) ? last : (unsigned long)(frameNumber + 1);
        pDecoder = (uint32_t*)((mode == CGif_Previous) ? palleteDecoder : palleteDecoderPrevious);
        pPrevious = (uint32_t*)((mode == CGif_Previous) ? palleteDecoderPrevious : palleteDecoder);
        for (x = (uint32_t)(width*height);--x;pDecoder[x - 1] = pPrevious[x - 1]);
    }
    if (mode == CGif_Background) {
        for (address[0] = (uint8_t)((transparent >= 0) ? transparent : background), y = 0, pDecoder = (uint32_t*)palleteDecoder; y < (uint32_t)rect.height; y++) {
            for (x = 0; x < (uint32_t)rect.width; x++) {
                pDecoder[(uint32_t)width*y + x + dstSource] = ARGB(0);
            }
        }
    }
    #undef ARGB
    frames.push_back(frame);
    delays.push_back((int)delay);
}

void CClientGifLoader::Load(std::vector<unsigned char*>& frames,std::vector<int>& delays,long skip) {
    const long Blen = (1 << 12)*sizeof(uint32_t);
    const uint8_t extensionHeaderMark = 0x21;
    const uint8_t frameHeaderMark = 0x2c;
    const uint8_t endOfBufferMark = 0x3b;
    const uint8_t graphicControlMark = 0xf9;
    const uint8_t appMetadataMark = 0xff;
    if (!buffer) {
        error = true;
        return;
    }
    #pragma pack(push,1)
    struct GlobalHeader {
        uint8_t  head[6];
        uint16_t width;
        uint16_t height;
        uint8_t  flags;
        uint8_t  background;
        uint8_t  aspectRatio;
    }* header = (struct GlobalHeader*)buffer;
    struct FrameHeader {
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        uint8_t  flags;
    }* frameHeader;
    struct GraphicControlHeader {
        uint8_t  flags;
        uint16_t delay;
        uint8_t  transparent;
    }* graphicControl = 0;
    #pragma pack(pop)
    long desc;
    long blen;
    uint8_t* decoder;
    if ( // check if header is : `GIF89a` or `GIF87a`
        !header ||
        (size <= (long)sizeof(*header)) ||
        (*(decoder = header->head) != 71) ||
        decoder[1] != 73 ||
        decoder[2] != 70 ||
        decoder[3] != 56 ||
        skip < 0 ||
        (decoder[4] != 55 && decoder[4] != 57) ||
        decoder[5] != 97
    ){
        error = true;
        return;
    }
    format = decoder[4] ? "GIF87a" : "GIF89a";
    decoder = (uint8_t*)(header + 1) + LoadHeader(header->flags, 0, 0, 0, 0, 0L) * 3L;
    if ((size -= decoder - (uint8_t*)header) <= 0) {
        error = true;
        return;
    }
    width = _SWAP(header->width);
    height = _SWAP(header->height);
    for (
        address = decoder,
        background = header->background,
        blen = --size;
        blen >= 0 &&
        ((desc = *address++) != endOfBufferMark);
        blen = SkipChunk(&address,blen) - 1
    ){
        if (desc == frameHeaderMark) {
            frameHeader = (struct FrameHeader*)address;
            if (LoadHeader(header->flags, &address, (void**)&pallete,frameHeader->flags,&blen,sizeof(*frameHeader)) <= 0) {
                break;
            }
            rect.width = _SWAP(frameHeader->width);
            rect.height = _SWAP(frameHeader->height);
            rect.left = (rect.width > rect.left) ? rect.width : rect.left;
            rect.top = (rect.height > rect.top) ? rect.height : rect.top;
            frameNumber++;
        }
    }
    blen = rect.left*rect.top*(long)sizeof(*address);
    GIF_ALLOCATE(address, (unsigned long)(blen + Blen + 2),1);
    frameCount = (desc != endOfBufferMark) ? -frameNumber : frameNumber;
    for (
        address += Blen,
        frameNumber = -1;
        blen &&
        (skip < ((frameCount < 0) ? -frameCount : frameCount)) && size >= 0;
        size = (desc != endOfBufferMark) ? ((desc != frameHeaderMark) || (skip > frameNumber)) ? SkipChunk(&decoder,size) - 1 : size - 1 : -1
    ){
        if ((desc = *decoder++) == frameHeaderMark) { // found a frame
            interlaced = !!((frameHeader = (struct FrameHeader*)decoder)->flags & 0x40);
            *(void**)&pallete = (void*)(header + 1);
            palleteSize = LoadHeader(header->flags, &decoder, (void**)&pallete, frameHeader->flags, &size, sizeof(*frameHeader));
            if (
                (skip <= ++frameNumber) &&
                (
                    (palleteSize <= 0) ||
                    LoadFrame(&decoder,&size,address,address + blen) < 0
                )
            ){
                size = -(frameNumber--) - 1; // failed to load frame
            }else if (skip <= frameNumber) {
                rect.left = _SWAP(frameHeader->x);
                rect.top = _SWAP(frameHeader->y);
                rect.width = _SWAP(frameHeader->width);
                rect.height = _SWAP(frameHeader->height);
                delay = (graphicControl) ? _SWAP(graphicControl->delay) : 0;
                transparent = (graphicControl && (graphicControl->flags & 0x01)) ? graphicControl->transparent : -1;
                delay = ((graphicControl && (graphicControl->flags & 0x02)) ? -delay - 1 : delay)*10;
                mode = (graphicControl && !(graphicControl->flags & 0x10)) ? (graphicControl->flags & 0x0c) >> 2 : CGif_None;
                graphicControl = 0;
                CreateFrame(frames,delays); // creating frame plain
            }
        }else if (desc == extensionHeaderMark){ // found an extension
            if (*decoder == graphicControlMark) {
                graphicControl = (struct GraphicControlHeader*)(decoder + 1 + 1);
            }
        }
    }
    address -= Blen;
    GIF_ALLOCATE(address, (unsigned long)(blen + Blen + 2), 0);
}

bool CClientGifLoader::operator!() {
    return buffer ? error : false;
}

long CClientGifLoader::SkipChunk(uint8_t** buffer, long size){
    long skip;
    for (skip = 2, ++size, ++(*buffer); ((size -= skip) > 0) && (skip > 1);*buffer += (skip = 1 + **buffer));
    return size;
}

long CClientGifLoader::LoadHeader(unsigned globalFlags, uint8_t** buffer, void** pallete, unsigned state, long* size, long length) {
    if (length && (!(*buffer += length) || ((*size -= length) <= 0))){
        return -2;
    }
    if (length && (state & 0x80)) {
        *pallete = *buffer;
        *buffer += (length = 2 << (state & 7))*3L;
        return ((*size -= length * 3L) > 0) ? length : -1;
    }
    return (globalFlags & 0x80) ? (2 << (globalFlags & 7)) : 0;
}

long CClientGifLoader::LoadFrame(uint8_t** buffer, long* size, uint8_t* address, uint8_t* blen){
    const long headerLength = sizeof(uint16_t);
    const long tableLength = 1 << 12;
    uint16_t accumulator;
    uint16_t mask;
    long lastCodeTable;
    long iterator;
    long previous;
    long current;
    long minLZWSize;
    long currentLZWSize;
    long blockSequense;
    long bitSize;
    uint32_t* code = (uint32_t*)address - tableLength;
    if ((--(*size) <= headerLength) || !*++(*buffer)){
        return -4;
    }
    mask = (uint16_t)((1 << (currentLZWSize = (minLZWSize = *(*buffer - 1)) + 1)) - 1);
    if (minLZWSize < 2 || minLZWSize > 8) {
        return -3;
    }
    if ((lastCodeTable = (1L << minLZWSize)) != (mask & _SWAP(*(uint16_t*)(*buffer + 1)))) {
        return -2;
    }
    for (current = ++lastCodeTable; current; code[--current] = 0);
    for (--(*size), bitSize = -currentLZWSize, previous = current = 0; ((*size -= (blockSequense = *(*buffer)++) + 1) >= 0) && blockSequense; *buffer += blockSequense) {
        for (; blockSequense > 0; blockSequense -= headerLength, *buffer += headerLength) {
            for (
                accumulator = (uint16_t)(_SWAP(*(uint16_t*)*buffer) & ((blockSequense < headerLength) ? ((1U << (8 * blockSequense)) - 1U) : ~0U)),
                current |= accumulator << (currentLZWSize + bitSize),
                accumulator = (uint16_t)(accumulator >> -bitSize),
                bitSize += 8 * ((blockSequense < headerLength) ? blockSequense : headerLength);
                bitSize >= 0;
                bitSize -= currentLZWSize,
                previous = current,
                current = accumulator,
                accumulator = (uint16_t)(accumulator >> currentLZWSize)
            ){
                if (((current &= mask) & ~1L) == (1L << minLZWSize)) {
                    if (~(lastCodeTable = current + 1) & 1){ // end of frame data
                        return (*((*buffer += blockSequense + 1) - 1)) ? -1 : 1;
                    }
                    mask = (uint16_t)((1 << (currentLZWSize = minLZWSize + 1)) - 1);
                }else{
                    if (lastCodeTable < tableLength){
                        if ((lastCodeTable == mask) && (lastCodeTable < tableLength - 1)) {
                            mask = (uint16_t)(mask + mask + 1);
                            currentLZWSize++;
                        }
                        code[lastCodeTable] = (uint32_t)previous + (code[previous] & 0xfff000);
                    }
                    previous = (long)code[iterator = (lastCodeTable > current) ? current : previous];
                    if ((address += (previous = (previous >> 12) & 0xfff)) > blen) {
                        continue;
                    }
                    for (previous++; (iterator &= 0xfff) >> minLZWSize; *address-- = (uint8_t)((iterator = (long)code[iterator]) >> 24));
                    (address += previous)[-previous] = (uint8_t)iterator;
                    if (lastCodeTable < tableLength) {
                        if (lastCodeTable == current) {
                            *address++ = (uint8_t)iterator;
                        }else if (lastCodeTable < current){
                            return -5; // wrong code
                        }
                        code[lastCodeTable++] += ((uint32_t)iterator << 24) + 0x1000;
                    }
                }
            }
        }
    }
    return (++(*size) >= 0) ? 0 : -4; // recoverable error
}

#undef _SWAP
