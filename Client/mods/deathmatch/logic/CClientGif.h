/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientGifDisplay.h"

enum {
    CGif_None = 0,
    CGif_Current = 1,
    CGif_Background = 2,
    CGif_Previous = 3
};

class CClientGifRect {
public:
    uint left = 0;
    uint top = 0;
    uint width = 0;
    uint height = 0;
    bool valid = false;
    CClientGifRect() = default;
    CClientGifRect(const uint& l, const uint& t, const uint& w, const uint& h) : left(l), top(t), width(w), height(h) { valid = true; }
};

class CClientGifLoader {
public:
    CClientGifLoader() = default;
    CClientGifLoader(SString&);
    CClientGifLoader(uint8_t*,unsigned long);
    CClientGifLoader(char*);
    void        CreateFrame(std::vector<unsigned char*>&,std::vector<int>&);
    void        Load(std::vector<unsigned char*>&,std::vector<int>&,long);
    uint8_t*    GetBuffer() { return buffer; }
    long&       GetWidth() { return width; }
    long&       GetHeight() { return height; }
    long&       GetDelay() { return delay; }
    long&       GetFrameNumber() { return frameNumber; }
    long&       GetFrameCount() { return frameCount; }
    SString     GetFormat() { return format; }
    bool        operator!();

    static long SkipChunk(uint8_t**, long);
    static long LoadHeader(unsigned, uint8_t**, void**, unsigned, long*, long);
    static long LoadFrame(uint8_t**, long*, uint8_t*, uint8_t*);

private:
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    }* pallete;
    SString        format = "Undefined";
    uint8_t*       buffer = nullptr;
    uint8_t*       address = nullptr;
    void*          palleteDecoder = nullptr;
    void*          palleteDecoderPrevious = nullptr;
    long           size = 0;
    long           width = 0;
    long           height = 0;
    long           palleteSize = 0;
    long           background = 0;
    long           transparent = 0;
    long           interlaced = 0;
    long           delay = 0;
    long           frameNumber = 0;
    long           frameCount = 0;
    long           mode = CGif_None;
    unsigned long  last = 0;
    CClientGifRect rect = {0,0,0,0};
    bool           error = false;
};

class CClientGif final : public CClientTexture {
    DECLARE_CLASS(CClientGif, CClientTexture);

public:
    CClientGif(CClientManager*, ElementID, CGifItem*);
    ~CClientGif();
    virtual void                 Unlink() override;
    void                         Register(std::vector<unsigned char*>,std::vector<int>);
    CResource*                   GetResource() { return m_pResource; }
    void                         SetResource(CResource* resource) { m_pResource = resource; }
    eClientEntityType            GetType() const { return CCLIENTGIF; }
    CGifItem*                    GetRenderItem() const { return static_cast<CGifItem*>(m_pRenderItem); }
    CClientGifDisplay*           GetDisplay() const { return m_pGifDisplay.get(); }
    std::vector<unsigned char*>& GetFrames() { return frames; }
    uint                         GetStride() { return stride; }
    uint                         GetShowingFrame() { return showing; }
    uint                         GetFrameDelay(const uint& id) { return delays[(id < 1 ? showing : (id > GetImageCount() ? showing : id)) - 1]; }
    uint                         GetFrameDefaultDelay(const uint& id) { return defaultDelays[(id < 1 ? showing : (id > GetImageCount() ? showing : id)) - 1]; }
    int                          GetImageCount() { return frames.size(); }
    double&                      GetTick() { return tick; }
    unsigned char*               GetFrame() { return showing <= GetImageCount() ? frames[showing - 1] : nullptr; }
    SString&                     GetFormat() { return format; }
    uint                         GetFrameCount() { return frameCount; }
    void                         SetFrameDelay(const uint& id, const uint32_t& delay) { delays[(id < 1 ? showing : (id > GetImageCount() ? (showing) : id)) - 1] = delay; }
    void                         SetFormat(const SString& fmt) { if (!fmt.empty()) format = fmt; }
    void                         SetFrameCount(const uint& count) { frameCount = count; }
    void                         UpdateTick() { tick = (double)GetTickCount64_(); }
    void                         Play() { playing = true; }
    void                         Stop() { playing = false; }
    void                         Next();
    void                         NavigateToThumbnail() { showing = 1; }
    bool                         IsPlaying() { return playing; }
    bool                         IsDestoryed() const { return m_bIsDestoryed; }

private:
    CResource*                                 m_pResource = nullptr;
    CClientManager*                            m_pManager;
    std::unique_ptr<CClientGifDisplay>         m_pGifDisplay;
    SString                                    format = "Undefined";
    std::vector<unsigned char*>                frames;
    std::vector<int>                           defaultDelays;
    std::vector<int>                           delays;
    uint                                       frameCount = 0;
    uint                                       stride;
    uint                                       showing = 1;
    double                                     tick = (double)GetTickCount64_();
    bool                                       playing = false;
    bool                                       m_bIsDestoryed = false;
    std::string                                error = "";
};
