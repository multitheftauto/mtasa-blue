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

CClientGif::CClientGif(CClientManager* pManager, ElementID ID, CGifItem* pGifItem)
    : ClassInit(this), CClientTexture(pManager, ID, pGifItem)
{
    SetTypeName("gif");
    m_pManager = pManager;
    m_bStride = GetRenderItem()->m_uiSizeX * 4;
    UpdateTick();
    m_pGifDisplay = std::make_unique<CClientGifDisplay>(m_pManager->GetDisplayManager(), this);
}

CClientGif::~CClientGif(){
    Unlink();
}

void CClientGif::Register(std::vector<std::vector<unsigned char>>&& frms,std::vector<int>& dls) {
    m_bFrames = std::move(frms);
    m_bDelays = std::move(dls);
    m_bDefaultDelays = m_bDelays;
    m_pGifDisplay->UpdateTexture();
}

void CClientGif::Next(){
    if (m_bShowing >= GetImageCount()) {
        m_bShowing = 1;
    }else{
        m_bShowing++;
    }
}

void CClientGif::Unlink(){
    for (std::vector<unsigned char>& frame : m_bFrames) {
        frame.clear();
    }
    m_bFrames.clear();
    m_bDelays.clear();
    m_bDefaultDelays.clear();
    m_bIsDestoryed = true;
    CClientRenderElement::Unlink();
}

CClientGifLoader::CClientGifLoader(SString& data) {
    m_pBuffer = reinterpret_cast<uint8_t*>(data.data());
    m_bSize = (long)((unsigned long)data.size());
}

CClientGifLoader::CClientGifLoader(uint8_t* data,unsigned long dataSize) {
    m_pBuffer = data;
    m_bSize = (long)dataSize;
}

CClientGifLoader::CClientGifLoader(char* data) {
    m_pBuffer = reinterpret_cast<uint8_t*>(data);
    m_bSize = (long)((unsigned long)strlen(data));
}

void CClientGifLoader::CreateFrame(std::vector<std::vector<unsigned char>>& frames,std::vector<int>& delays) {
    long frameStride = m_bWidth*4;
    long frameSize = frameStride*m_bHeight;
    std::vector<unsigned char> frame;
    for (long i = 0; i < frameSize; i++, frame.push_back(0));
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
        (m_pAddress[i] == m_bTransparent) ? \
        0 : \
        ( \
            (uint32_t)m_pPallete[m_pAddress[i]].r << 16 | \
            (uint32_t)m_pPallete[m_pAddress[i]].g << 8 | \
            (uint32_t)m_pPallete[m_pAddress[i]].b << 0 | \
            0xff000000 \
        ) \
    )
    if (!m_bFrameNumber) {
        m_bFrameCount = ((m_bFrameCount < 0) ? -m_bFrameCount : m_bFrameCount)*m_bHeight;
        m_bFrameCount = (m_bFrameCount < 0xffff) ? m_bFrameCount : 0xffff;
        dstSource = (uint32_t)(m_bWidth*m_bHeight);
        m_pPalleteDecoder = calloc(sizeof(uint32_t),dstSource);
        m_pPalleteDecoderPrevious = calloc(sizeof(uint32_t),dstSource);
    }
    pDecoder = (uint32_t*)m_pPalleteDecoder;
    dstSource = (uint32_t)(m_bWidth*m_bRect.top + m_bRect.left);
    inter = (!(iterator = m_bInterlaced ? 0 : 4)) ? 4 : 5;
    for (source = -1; iterator < inter; iterator++) {
        for (yoffset = 16U >> ((iterator > 1) ? iterator : 0), y = (8 >> iterator) & 7; y < (uint32_t)m_bRect.height; y += yoffset) {
            for (x = 0; x < (uint32_t)m_bRect.width; x++) {
                if (m_bTransparent != (long)m_pAddress[++source]) {
                    pDecoder[(uint32_t)m_bWidth*y + x + dstSource] = ARGB(source);
                }
            }
        }
    }
    memcpy((uint32_t*)frame.data(), pDecoder, sizeof(uint32_t)*(uint32_t)m_bWidth*(uint32_t)m_bHeight);
    if ((m_bMode == CGif_Previous) && !m_bLast) {
        m_bRect.width = m_bWidth;
        m_bRect.height = m_bHeight;
        m_bMode = CGif_Background;
        dstSource = 0;
    }else{
        m_bLast = (m_bMode == CGif_Previous) ? m_bLast : (unsigned long)(m_bFrameNumber + 1);
        pDecoder = (uint32_t*)((m_bMode == CGif_Previous) ? m_pPalleteDecoder : m_pPalleteDecoderPrevious);
        pPrevious = (uint32_t*)((m_bMode == CGif_Previous) ? m_pPalleteDecoderPrevious : m_pPalleteDecoder);
        for (x = (uint32_t)(m_bWidth*m_bHeight);--x;pDecoder[x - 1] = pPrevious[x - 1]);
    }
    if (m_bMode == CGif_Background) {
        for (m_pAddress[0] = (uint8_t)((m_bTransparent >= 0) ? m_bTransparent : m_bBackground), y = 0, pDecoder = (uint32_t*)m_pPalleteDecoder; y < (uint32_t)m_bRect.height; y++) {
            for (x = 0; x < (uint32_t)m_bRect.width; x++) {
                pDecoder[(uint32_t)m_bWidth*y + x + dstSource] = ARGB(0);
            }
        }
    }
    #undef ARGB
    frames.push_back(std::move(frame));
    delays.push_back((int)m_bDelay);
}

void CClientGifLoader::Load(std::vector<std::vector<unsigned char>>& frames,std::vector<int>& delays,long skip) {
    const long Blen = (1 << 12)*sizeof(uint32_t);
    const uint8_t extensionHeaderMark = 0x21;
    const uint8_t frameHeaderMark = 0x2c;
    const uint8_t endOfBufferMark = 0x3b;
    const uint8_t graphicControlMark = 0xf9;
    const uint8_t appMetadataMark = 0xff;
    if (!m_pBuffer) {
        m_bError = true;
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
    }* header = (struct GlobalHeader*)m_pBuffer;
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
        (m_bSize <= (long)sizeof(*header)) ||
        (*(decoder = header->head) != 71) ||
        decoder[1] != 73 ||
        decoder[2] != 70 ||
        decoder[3] != 56 ||
        skip < 0 ||
        (decoder[4] != 55 && decoder[4] != 57) ||
        decoder[5] != 97
    ){
        m_bError = true;
        return;
    }
    m_bFormat = decoder[4] ? "GIF87a" : "GIF89a";
    decoder = (uint8_t*)(header + 1) + LoadHeader(header->flags, 0, 0, 0, 0, 0L) * 3L;
    if ((m_bSize -= decoder - (uint8_t*)header) <= 0) {
        m_bError = true;
        return;
    }
    m_bWidth = _SWAP(header->width);
    m_bHeight = _SWAP(header->height);
    for (
        m_pAddress = decoder,
        m_bBackground = header->background,
        blen = --m_bSize;
        blen >= 0 &&
        ((desc = *m_pAddress++) != endOfBufferMark);
        blen = SkipChunk(&m_pAddress,blen) - 1
    ){
        if (desc == frameHeaderMark) {
            frameHeader = (struct FrameHeader*)m_pAddress;
            if (LoadHeader(header->flags, &m_pAddress, (void**)&m_pPallete,frameHeader->flags,&blen,sizeof(*frameHeader)) <= 0) {
                break;
            }
            m_bRect.width = _SWAP(frameHeader->width);
            m_bRect.height = _SWAP(frameHeader->height);
            m_bRect.left = (m_bRect.width > m_bRect.left) ? m_bRect.width : m_bRect.left;
            m_bRect.top = (m_bRect.height > m_bRect.top) ? m_bRect.height : m_bRect.top;
            m_bFrameNumber++;
        }
    }
    blen = m_bRect.left*m_bRect.top*(long)sizeof(*m_pAddress);
    GIF_ALLOCATE(m_pAddress, (unsigned long)(blen + Blen + 2),1);
    m_bFrameCount = (desc != endOfBufferMark) ? -m_bFrameNumber : m_bFrameNumber;
    for (
        m_pAddress += Blen,
        m_bFrameNumber = -1;
        blen &&
        (skip < ((m_bFrameCount < 0) ? -m_bFrameCount : m_bFrameCount)) && m_bSize >= 0;
        m_bSize = (desc != endOfBufferMark) ? ((desc != frameHeaderMark) || (skip > m_bFrameNumber)) ? SkipChunk(&decoder,m_bSize) - 1 : m_bSize - 1 : -1
    ){
        if ((desc = *decoder++) == frameHeaderMark) { // found a frame
            m_bInterlaced = !!((frameHeader = (struct FrameHeader*)decoder)->flags & 0x40);
            *(void**)&m_pPallete = (void*)(header + 1);
            m_bPalleteSize = LoadHeader(header->flags, &decoder, (void**)&m_pPallete, frameHeader->flags, &m_bSize, sizeof(*frameHeader));
            if (
                (skip <= ++m_bFrameNumber) &&
                (
                    (m_bPalleteSize <= 0) ||
                    LoadFrame(&decoder,&m_bSize,m_pAddress,m_pAddress + blen) < 0
                )
            ){
                m_bSize = -(m_bFrameNumber--) - 1; // failed to load frame
            }else if (skip <= m_bFrameNumber) {
                m_bRect.left = _SWAP(frameHeader->x);
                m_bRect.top = _SWAP(frameHeader->y);
                m_bRect.width = _SWAP(frameHeader->width);
                m_bRect.height = _SWAP(frameHeader->height);
                m_bDelay = (graphicControl) ? _SWAP(graphicControl->delay) : 0;
                m_bTransparent = (graphicControl && (graphicControl->flags & 0x01)) ? graphicControl->transparent : -1;
                m_bDelay = ((graphicControl && (graphicControl->flags & 0x02)) ? -m_bDelay - 1 : m_bDelay)*10;
                m_bMode = (graphicControl && !(graphicControl->flags & 0x10)) ? (graphicControl->flags & 0x0c) >> 2 : CGif_None;
                graphicControl = 0;
                CreateFrame(frames,delays); // creating frame plain
            }
        }else if (desc == extensionHeaderMark){ // found an extension
            if (*decoder == graphicControlMark) {
                graphicControl = (struct GraphicControlHeader*)(decoder + 1 + 1);
            }
        }
    }
    m_pAddress -= Blen;
    GIF_ALLOCATE(m_pAddress, (unsigned long)(blen + Blen + 2), 0);
}

bool CClientGifLoader::operator!() {
    return m_pBuffer ? m_bError : false;
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
