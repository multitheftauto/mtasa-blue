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

#define GIF_ALLOCATE(pointer, size, release) pointer = (uint8_t*)realloc((release) ? 0 : pointer, (release) ? size : 0UL)
#define BIGE                                 0
#define _SWAP(h)                             ((BIGE) ? ((uint16_t)(h << 8) | (h << 8)) : h)

CClientGif::CClientGif(CClientManager* pManager, ElementID ID, CGifItem* pGifItem) : ClassInit(this), CClientTexture(pManager, ID, pGifItem)
{
    SetTypeName("gif");
    m_pManager = pManager;
    m_uiStride = GetRenderItem()->m_uiSizeX * 4;
    UpdateTick();
    m_pGifDisplay = std::make_unique<CClientGifDisplay>(m_pManager->GetDisplayManager(), this);
}

CClientGif::~CClientGif()
{
    Unlink();
}

void CClientGif::Register(std::vector<std::vector<unsigned char>>&& frms, std::vector<int>& dls)
{
    m_vecFrames = std::move(frms);
    m_vecDelays = std::move(dls);
    m_vecDefaultDelays = m_vecDelays;
    m_pGifDisplay->UpdateTexture();
}

void CClientGif::Next()
{
    if (m_uiShowing >= GetImageCount())
    {
        m_uiShowing = 1;
    }
    else
    {
        m_uiShowing++;
    }
}

void CClientGif::Unlink()
{
    for (std::vector<unsigned char>& frame : m_vecFrames)
    {
        frame.clear();
    }
    m_vecFrames.clear();
    m_vecDelays.clear();
    m_vecDefaultDelays.clear();
    m_bIsDestoryed = true;
    CClientRenderElement::Unlink();
}

CClientGifLoader::CClientGifLoader(SString& data)
{
    m_pBuffer = reinterpret_cast<uint8_t*>(data.data());
    m_lgSize = (long)((unsigned long)data.size());
}

CClientGifLoader::CClientGifLoader(uint8_t* data, unsigned long dataSize)
{
    m_pBuffer = data;
    m_lgSize = (long)dataSize;
}

CClientGifLoader::CClientGifLoader(char* data)
{
    m_pBuffer = reinterpret_cast<uint8_t*>(data);
    m_lgSize = (long)((unsigned long)strlen(data));
}

void CClientGifLoader::CreateFrame(std::vector<std::vector<unsigned char>>& frames, std::vector<int>& delays)
{
    long                       frameStride = m_lgWidth * 4;
    long                       frameSize = frameStride * m_lgHeight;
    std::vector<unsigned char> frame;
    for (long i = 0; i < frameSize; i++, frame.push_back(0))
        ;
    uint32_t  x;
    uint32_t  y;
    uint32_t  yoffset;
    uint32_t  iterator;
    uint32_t  inter;
    uint32_t  source;
    uint32_t  dstSource;
    uint32_t* pDecoder;
    uint32_t* pPrevious;
#define ARGB(i) \
    ((m_pAddress[i] == m_lgTransparent) ? 0 \
                                        : ((uint32_t)m_pPallete[m_pAddress[i]].r << 16 | (uint32_t)m_pPallete[m_pAddress[i]].g << 8 | \
                                           (uint32_t)m_pPallete[m_pAddress[i]].b << 0 | 0xff000000))
    if (!m_lgFrameNumber)
    {
        m_uiFrameCount = ((m_uiFrameCount < 0) ? -m_uiFrameCount : m_uiFrameCount) * m_lgHeight;
        m_uiFrameCount = (m_uiFrameCount < 0xffff) ? m_uiFrameCount : 0xffff;
        dstSource = (uint32_t)(m_lgWidth * m_lgHeight);
        m_pPalleteDecoder = calloc(sizeof(uint32_t), dstSource);
        m_pPalleteDecoderPrevious = calloc(sizeof(uint32_t), dstSource);
    }
    pDecoder = (uint32_t*)m_pPalleteDecoder;
    dstSource = (uint32_t)(m_lgWidth * m_rcRect.top + m_rcRect.left);
    inter = (!(iterator = m_lgInterlaced ? 0 : 4)) ? 4 : 5;
    for (source = -1; iterator < inter; iterator++)
    {
        for (yoffset = 16U >> ((iterator > 1) ? iterator : 0), y = (8 >> iterator) & 7; y < (uint32_t)m_rcRect.height; y += yoffset)
        {
            for (x = 0; x < (uint32_t)m_rcRect.width; x++)
            {
                if (m_lgTransparent != (long)m_pAddress[++source])
                {
                    pDecoder[(uint32_t)m_lgWidth * y + x + dstSource] = ARGB(source);
                }
            }
        }
    }
    memcpy((uint32_t*)frame.data(), pDecoder, sizeof(uint32_t) * (uint32_t)m_lgWidth * (uint32_t)m_lgHeight);
    if ((m_lgMode == CGif_Previous) && !m_ulgLast)
    {
        m_rcRect.width = m_lgWidth;
        m_rcRect.height = m_lgHeight;
        m_lgMode = CGif_Background;
        dstSource = 0;
    }
    else
    {
        m_ulgLast = (m_lgMode == CGif_Previous) ? m_ulgLast : (unsigned long)(m_lgFrameNumber + 1);
        pDecoder = (uint32_t*)((m_lgMode == CGif_Previous) ? m_pPalleteDecoder : m_pPalleteDecoderPrevious);
        pPrevious = (uint32_t*)((m_lgMode == CGif_Previous) ? m_pPalleteDecoderPrevious : m_pPalleteDecoder);
        for (x = (uint32_t)(m_lgWidth * m_lgHeight); --x; pDecoder[x - 1] = pPrevious[x - 1])
            ;
    }
    if (m_lgMode == CGif_Background)
    {
        for (m_pAddress[0] = (uint8_t)((m_lgTransparent >= 0) ? m_lgTransparent : m_lgBackground), y = 0, pDecoder = (uint32_t*)m_pPalleteDecoder;
             y < (uint32_t)m_rcRect.height; y++)
        {
            for (x = 0; x < (uint32_t)m_rcRect.width; x++)
            {
                pDecoder[(uint32_t)m_lgWidth * y + x + dstSource] = ARGB(0);
            }
        }
    }
#undef ARGB
    frames.push_back(std::move(frame));
    delays.push_back((int)m_lgDelay);
}

void CClientGifLoader::Load(std::vector<std::vector<unsigned char>>& frames, std::vector<int>& delays, long skip)
{
    const long    Blen = (1 << 12) * sizeof(uint32_t);
    const uint8_t extensionHeaderMark = 0x21;
    const uint8_t frameHeaderMark = 0x2c;
    const uint8_t endOfBufferMark = 0x3b;
    const uint8_t graphicControlMark = 0xf9;
    const uint8_t appMetadataMark = 0xff;
    if (!m_pBuffer)
    {
        m_bError = true;
        return;
    }
#pragma pack(push, 1)
    struct GlobalHeader
    {
        uint8_t  head[6];
        uint16_t width;
        uint16_t height;
        uint8_t  flags;
        uint8_t  background;
        uint8_t  aspectRatio;
    }* header = (struct GlobalHeader*)m_pBuffer;
    struct FrameHeader
    {
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        uint8_t  flags;
    }* frameHeader;
    struct GraphicControlHeader
    {
        uint8_t  flags;
        uint16_t delay;
        uint8_t  transparent;
    }* graphicControl = 0;
#pragma pack(pop)
    long     desc;
    long     blen;
    uint8_t* decoder;
    if (            // check if header is : `GIF89a` or `GIF87a`
        !header || (m_lgSize <= (long)sizeof(*header)) || (*(decoder = header->head) != 71) || decoder[1] != 73 || decoder[2] != 70 || decoder[3] != 56 ||
        skip < 0 || (decoder[4] != 55 && decoder[4] != 57) || decoder[5] != 97)
    {
        m_bError = true;
        return;
    }
    m_strFormat = decoder[4] ? "GIF87a" : "GIF89a";
    decoder = (uint8_t*)(header + 1) + LoadHeader(header->flags, 0, 0, 0, 0, 0L) * 3L;
    if ((m_lgSize -= decoder - (uint8_t*)header) <= 0)
    {
        m_bError = true;
        return;
    }
    m_lgWidth = _SWAP(header->width);
    m_lgHeight = _SWAP(header->height);
    for (m_pAddress = decoder, m_lgBackground = header->background, blen = --m_lgSize; blen >= 0 && ((desc = *m_pAddress++) != endOfBufferMark);
         blen = SkipChunk(&m_pAddress, blen) - 1)
    {
        if (desc == frameHeaderMark)
        {
            frameHeader = (struct FrameHeader*)m_pAddress;
            if (LoadHeader(header->flags, &m_pAddress, (void**)&m_pPallete, frameHeader->flags, &blen, sizeof(*frameHeader)) <= 0)
            {
                break;
            }
            m_rcRect.width = _SWAP(frameHeader->width);
            m_rcRect.height = _SWAP(frameHeader->height);
            m_rcRect.left = (m_rcRect.width > m_rcRect.left) ? m_rcRect.width : m_rcRect.left;
            m_rcRect.top = (m_rcRect.height > m_rcRect.top) ? m_rcRect.height : m_rcRect.top;
            m_lgFrameNumber++;
        }
    }
    blen = m_rcRect.left * m_rcRect.top * (long)sizeof(*m_pAddress);
    GIF_ALLOCATE(m_pAddress, (unsigned long)(blen + Blen + 2), 1);
    m_uiFrameCount = (desc != endOfBufferMark) ? -m_lgFrameNumber : m_lgFrameNumber;
    for (m_pAddress += Blen, m_lgFrameNumber = -1; blen && (skip < ((m_uiFrameCount < 0) ? -m_uiFrameCount : m_uiFrameCount)) && m_lgSize >= 0;
         m_lgSize = (desc != endOfBufferMark) ? ((desc != frameHeaderMark) || (skip > m_lgFrameNumber)) ? SkipChunk(&decoder, m_lgSize) - 1 : m_lgSize - 1 : -1)
    {
        if ((desc = *decoder++) == frameHeaderMark)
        {            // found a frame
            m_lgInterlaced = !!((frameHeader = (struct FrameHeader*)decoder)->flags & 0x40);
            *(void**)&m_pPallete = (void*)(header + 1);
            m_lgPalleteSize = LoadHeader(header->flags, &decoder, (void**)&m_pPallete, frameHeader->flags, &m_lgSize, sizeof(*frameHeader));
            if ((skip <= ++m_lgFrameNumber) && ((m_lgPalleteSize <= 0) || LoadFrame(&decoder, &m_lgSize, m_pAddress, m_pAddress + blen) < 0))
            {
                m_lgSize = -(m_lgFrameNumber--) - 1;            // failed to load frame
            }
            else if (skip <= m_lgFrameNumber)
            {
                m_rcRect.left = _SWAP(frameHeader->x);
                m_rcRect.top = _SWAP(frameHeader->y);
                m_rcRect.width = _SWAP(frameHeader->width);
                m_rcRect.height = _SWAP(frameHeader->height);
                m_lgDelay = (graphicControl) ? _SWAP(graphicControl->delay) : 0;
                m_lgTransparent = (graphicControl && (graphicControl->flags & 0x01)) ? graphicControl->transparent : -1;
                m_lgDelay = ((graphicControl && (graphicControl->flags & 0x02)) ? -m_lgDelay - 1 : m_lgDelay) * 10;
                m_lgMode = (graphicControl && !(graphicControl->flags & 0x10)) ? (graphicControl->flags & 0x0c) >> 2 : CGif_None;
                graphicControl = 0;
                CreateFrame(frames, delays);            // creating frame plain
            }
        }
        else if (desc == extensionHeaderMark)
        {            // found an extension
            if (*decoder == graphicControlMark)
            {
                graphicControl = (struct GraphicControlHeader*)(decoder + 1 + 1);
            }
        }
    }
    m_pAddress -= Blen;
    GIF_ALLOCATE(m_pAddress, (unsigned long)(blen + Blen + 2), 0);
}

bool CClientGifLoader::operator!()
{
    return m_pBuffer ? m_bError : false;
}

long CClientGifLoader::SkipChunk(uint8_t** buffer, long size)
{
    long skip;
    for (skip = 2, ++size, ++(*buffer); ((size -= skip) > 0) && (skip > 1); *buffer += (skip = 1 + **buffer))
        ;
    return size;
}

long CClientGifLoader::LoadHeader(unsigned globalFlags, uint8_t** buffer, void** pallete, unsigned state, long* size, long length)
{
    if (length && (!(*buffer += length) || ((*size -= length) <= 0)))
    {
        return -2;
    }
    if (length && (state & 0x80))
    {
        *pallete = *buffer;
        *buffer += (length = 2 << (state & 7)) * 3L;
        return ((*size -= length * 3L) > 0) ? length : -1;
    }
    return (globalFlags & 0x80) ? (2 << (globalFlags & 7)) : 0;
}

long CClientGifLoader::LoadFrame(uint8_t** buffer, long* size, uint8_t* address, uint8_t* blen)
{
    const long headerLength = sizeof(uint16_t);
    const long tableLength = 1 << 12;
    uint16_t   accumulator;
    uint16_t   mask;
    long       lastCodeTable;
    long       iterator;
    long       previous;
    long       current;
    long       minLZWSize;
    long       currentLZWSize;
    long       blockSequense;
    long       bitSize;
    uint32_t*  code = (uint32_t*)address - tableLength;
    if ((--(*size) <= headerLength) || !*++(*buffer))
    {
        return -4;
    }
    mask = (uint16_t)((1 << (currentLZWSize = (minLZWSize = *(*buffer - 1)) + 1)) - 1);
    if (minLZWSize < 2 || minLZWSize > 8)
    {
        return -3;
    }
    if ((lastCodeTable = (1L << minLZWSize)) != (mask & _SWAP(*(uint16_t*)(*buffer + 1))))
    {
        return -2;
    }
    for (current = ++lastCodeTable; current; code[--current] = 0)
        ;
    for (--(*size), bitSize = -currentLZWSize, previous = current = 0; ((*size -= (blockSequense = *(*buffer)++) + 1) >= 0) && blockSequense;
         *buffer += blockSequense)
    {
        for (; blockSequense > 0; blockSequense -= headerLength, *buffer += headerLength)
        {
            for (accumulator = (uint16_t)(_SWAP(*(uint16_t*)*buffer) & ((blockSequense < headerLength) ? ((1U << (8 * blockSequense)) - 1U) : ~0U)),
                current |= accumulator << (currentLZWSize + bitSize), accumulator = (uint16_t)(accumulator >> -bitSize),
                bitSize += 8 * ((blockSequense < headerLength) ? blockSequense : headerLength);
                 bitSize >= 0; bitSize -= currentLZWSize, previous = current, current = accumulator, accumulator = (uint16_t)(accumulator >> currentLZWSize))
            {
                if (((current &= mask) & ~1L) == (1L << minLZWSize))
                {
                    if (~(lastCodeTable = current + 1) & 1)
                    {            // end of frame data
                        return (*((*buffer += blockSequense + 1) - 1)) ? -1 : 1;
                    }
                    mask = (uint16_t)((1 << (currentLZWSize = minLZWSize + 1)) - 1);
                }
                else
                {
                    if (lastCodeTable < tableLength)
                    {
                        if ((lastCodeTable == mask) && (lastCodeTable < tableLength - 1))
                        {
                            mask = (uint16_t)(mask + mask + 1);
                            currentLZWSize++;
                        }
                        code[lastCodeTable] = (uint32_t)previous + (code[previous] & 0xfff000);
                    }
                    previous = (long)code[iterator = (lastCodeTable > current) ? current : previous];
                    if ((address += (previous = (previous >> 12) & 0xfff)) > blen)
                    {
                        continue;
                    }
                    for (previous++; (iterator &= 0xfff) >> minLZWSize; *address-- = (uint8_t)((iterator = (long)code[iterator]) >> 24))
                        ;
                    (address += previous)[-previous] = (uint8_t)iterator;
                    if (lastCodeTable < tableLength)
                    {
                        if (lastCodeTable == current)
                        {
                            *address++ = (uint8_t)iterator;
                        }
                        else if (lastCodeTable < current)
                        {
                            return -5;            // wrong code
                        }
                        code[lastCodeTable++] += ((uint32_t)iterator << 24) + 0x1000;
                    }
                }
            }
        }
    }
    return (++(*size) >= 0) ? 0 : -4;            // recoverable error
}

#undef _SWAP
