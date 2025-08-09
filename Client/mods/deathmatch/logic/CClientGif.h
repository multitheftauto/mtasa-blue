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

enum
{
    CGif_None = 0,
    CGif_Current = 1,
    CGif_Background = 2,
    CGif_Previous = 3
};

class CClientGifRect
{
public:
    uint left = 0;
    uint top = 0;
    uint width = 0;
    uint height = 0;
    bool valid = false;
    CClientGifRect() = default;
    CClientGifRect(const uint& l, const uint& t, const uint& w, const uint& h) : left(l), top(t), width(w), height(h) { valid = true; }
};

class CClientGifLoader
{
public:
    CClientGifLoader() = default;
    CClientGifLoader(SString& data);
    CClientGifLoader(uint8_t* data, unsigned long dataSize);
    CClientGifLoader(char* data);

    void     CreateFrame(std::vector<std::vector<unsigned char>>& frames, std::vector<int>& delays);
    void     Load(std::vector<std::vector<unsigned char>>& frames, std::vector<int>& delays, long skip);
    uint8_t* GetBuffer() { return m_pBuffer; }
    long&    GetWidth() { return m_lgWidth; }
    long&    GetHeight() { return m_lgHeight; }
    long&    GetDelay() { return m_lgDelay; }
    long&    GetFrameNumber() { return m_lgFrameNumber; }
    long&    GetFrameCount() { return m_uiFrameCount; }
    SString  GetFormat() { return m_strFormat; }
    bool     operator!();

    static long SkipChunk(uint8_t** buffer, long size);
    static long LoadHeader(unsigned globalFlags, uint8_t** buffer, void** pallete, unsigned state, long* size, long length);
    static long LoadFrame(uint8_t** buffer, long* size, uint8_t* address, uint8_t* blen);

private:
    struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    }*             m_pPallete;
    SString        m_strFormat = "Undefined";
    uint8_t*       m_pBuffer = nullptr;
    uint8_t*       m_pAddress = nullptr;
    void*          m_pPalleteDecoder = nullptr;
    void*          m_pPalleteDecoderPrevious = nullptr;
    long           m_lgSize = 0;
    long           m_lgWidth = 0;
    long           m_lgHeight = 0;
    long           m_lgPalleteSize = 0;
    long           m_lgBackground = 0;
    long           m_lgTransparent = 0;
    long           m_lgInterlaced = 0;
    long           m_lgDelay = 0;
    long           m_lgFrameNumber = 0;
    long           m_uiFrameCount = 0;
    long           m_lgMode = CGif_None;
    unsigned long  m_ulgLast = 0;
    CClientGifRect m_rcRect = {0, 0, 0, 0};
    bool           m_bError = false;
};

class CClientGif final : public CClientTexture
{
    DECLARE_CLASS(CClientGif, CClientTexture);

public:
    CClientGif(CClientManager* pManager, ElementID id, CGifItem* pGifItem);
    ~CClientGif();

    virtual void Unlink() override;
    void         Register(std::vector<std::vector<unsigned char>>&& frms, std::vector<int>& dls);

    CResource*                              GetResource() { return m_pResource; }
    void                                    SetResource(CResource* resource) { m_pResource = resource; }
    eClientEntityType                       GetType() const { return CCLIENTGIF; }
    CGifItem*                               GetRenderItem() const { return static_cast<CGifItem*>(m_pRenderItem); }
    CClientGifDisplay*                      GetDisplay() const { return m_pGifDisplay.get(); }
    std::vector<std::vector<unsigned char>> GetFrames() const { return m_vecFrames; }
    uint                                    GetStride() const { return m_uiStride; }
    uint                                    GetShowingFrame() const { return m_uiShowing; }
    uint                                    GetFrameDelay(const uint& id) const { return m_vecDelays[(id < 1 ? m_uiShowing : (id > GetImageCount() ? m_uiShowing : id)) - 1]; }
    uint                                    GetFrameDefaultDelay(const uint& id) const { return m_vecDefaultDelays[(id < 1 ? m_uiShowing : (id > GetImageCount() ? m_uiShowing : id)) - 1]; }
    int                                     GetImageCount() const { return m_vecFrames.size(); }
    double                                  GetTick() const { return m_dbTick; }
    unsigned char*                          GetFrame() { return m_uiShowing <= GetImageCount() ? m_vecFrames[m_uiShowing - 1].data() : nullptr; }
    SString                                 GetFormat() const { return m_strFormat; }
    uint                                    GetFrameCount() const { return m_uiFrameCount; }

    void SetFrameDelay(const uint& id, const uint32_t& delay) { m_vecDelays[(id < 1 ? m_uiShowing : (id > GetImageCount() ? (m_uiShowing) : id)) - 1] = delay; }
    void SetFormat(const SString& fmt) { if (!fmt.empty()) m_strFormat = fmt; }
    void SetFrameCount(const uint& count) { m_uiFrameCount = count; }
    void SetFrame(const int& frame) { if (frame <= GetImageCount()) m_uiShowing = 1; }

    void UpdateTick() { m_dbTick = (double)GetTickCount64_(); }
    void Play() { m_bPlaying = true; }
    void Stop() { m_bPlaying = false; }
    void Next();
    void NavigateToThumbnail() { m_uiShowing = 1; }

    bool IsPlaying() const { return m_bPlaying; }
    bool IsDestoryed() const { return m_bIsDestoryed; }

private:
    CResource*                              m_pResource = nullptr;
    CClientManager*                         m_pManager;
    std::unique_ptr<CClientGifDisplay>      m_pGifDisplay;
    SString                                 m_strFormat = "Undefined";
    std::vector<std::vector<unsigned char>> m_vecFrames;
    std::vector<int>                        m_vecDefaultDelays;
    std::vector<int>                        m_vecDelays;
    uint                                    m_uiFrameCount = 0;
    uint                                    m_uiStride;
    uint                                    m_uiShowing = 1;
    double                                  m_dbTick = 0;
    bool                                    m_bPlaying = false;
    bool                                    m_bIsDestoryed = false;
};
