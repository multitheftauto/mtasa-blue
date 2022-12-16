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
    uint8_t*    GetBuffer() { return m_pBuffer; }
    long&       GetWidth() { return m_bWidth; }
    long&       GetHeight() { return m_bHeight; }
    long&       GetDelay() { return m_bDelay; }
    long&       GetFrameNumber() { return m_bFrameNumber; }
    long&       GetFrameCount() { return m_bFrameCount; }
    SString     GetFormat() { return m_bFormat; }
    bool        operator!();

    static long SkipChunk(uint8_t**, long);
    static long LoadHeader(unsigned, uint8_t**, void**, unsigned, long*, long);
    static long LoadFrame(uint8_t**, long*, uint8_t*, uint8_t*);

private:
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    }* m_pPallete;
    SString        m_bFormat = "Undefined";
    uint8_t*       m_pBuffer = nullptr;
    uint8_t*       m_pAddress = nullptr;
    void*          m_pPalleteDecoder = nullptr;
    void*          m_pPalleteDecoderPrevious = nullptr;
    long           m_bSize = 0;
    long           m_bWidth = 0;
    long           m_bHeight = 0;
    long           m_bPalleteSize = 0;
    long           m_bBackground = 0;
    long           m_bTransparent = 0;
    long           m_bInterlaced = 0;
    long           m_bDelay = 0;
    long           m_bFrameNumber = 0;
    long           m_bFrameCount = 0;
    long           m_bMode = CGif_None;
    unsigned long  m_bLast = 0;
    CClientGifRect m_bRect = {0,0,0,0};
    bool           m_bError = false;
};

class CClientGif final : public CClientTexture {
    DECLARE_CLASS(CClientGif, CClientTexture);

public:
    CClientGif(CClientManager*, ElementID, CGifItem*);
    ~CClientGif();
    virtual void                 Unlink() override;
    void                         Register(std::vector<unsigned char*>&&,std::vector<int>&&);
    CResource*                   GetResource() { return m_pResource; }
    void                         SetResource(CResource* resource) { m_pResource = resource; }
    eClientEntityType            GetType() const { return CCLIENTGIF; }
    CGifItem*                    GetRenderItem() const { return static_cast<CGifItem*>(m_pRenderItem); }
    CClientGifDisplay*           GetDisplay() const { return m_pGifDisplay.get(); }
    std::vector<unsigned char*>  GetFrames() const { return m_bFrames; }
    uint                         GetStride() const { return m_bStride; }
    uint                         GetShowingFrame() const { return m_bShowing; }
    uint                         GetFrameDelay(const uint& id) const { return m_bDelays[(id < 1 ? m_bShowing : (id > GetImageCount() ? m_bShowing : id)) - 1]; }
    uint                         GetFrameDefaultDelay(const uint& id) const { return m_bDefaultDelays[(id < 1 ? m_bShowing : (id > GetImageCount() ? m_bShowing : id)) - 1]; }
    int                          GetImageCount() const { return m_bFrames.size(); }
    double                       GetTick() const { return m_bTick; }
    unsigned char*               GetFrame() const { return m_bShowing <= GetImageCount() ? m_bFrames[m_bShowing - 1] : nullptr; }
    SString                      GetFormat() const { return m_bFormat; }
    uint                         GetFrameCount() const { return m_bFrameCount; }
    void                         SetFrameDelay(const uint& id, const uint32_t& delay) { m_bDelays[(id < 1 ? m_bShowing : (id > GetImageCount() ? (m_bShowing) : id)) - 1] = delay; }
    void                         SetFormat(const SString& fmt) { if (!fmt.empty()) m_bFormat = fmt; }
    void                         SetFrameCount(const uint& count) { m_bFrameCount = count; }
    void                         UpdateTick() { m_bTick = (double)GetTickCount64_(); }
    void                         Play() { m_bPlaying = true; }
    void                         Stop() { m_bPlaying = false; }
    void                         Next();
    void                         NavigateToThumbnail() { m_bShowing = 1; }
    bool                         IsPlaying() const { return m_bPlaying; }
    bool                         IsDestoryed() const { return m_bIsDestoryed; }

private:
    CResource*                                 m_pResource = nullptr;
    CClientManager*                            m_pManager;
    std::unique_ptr<CClientGifDisplay>         m_pGifDisplay;
    SString                                    m_bFormat = "Undefined";
    std::vector<unsigned char*>                m_bFrames;
    std::vector<int>                           m_bDefaultDelays;
    std::vector<int>                           m_bDelays;
    uint                                       m_bFrameCount = 0;
    uint                                       m_bStride;
    uint                                       m_bShowing = 1;
    double                                     m_bTick = 0;
    bool                                       m_bPlaying = false;
    bool                                       m_bIsDestoryed = false;
};
