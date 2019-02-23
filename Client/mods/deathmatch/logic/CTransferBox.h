/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTransferBox.h
 *  PURPOSE:     Header for transfer box class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define TRANSFERBOX_FRAMES 10
#define TRANSFERBOX_DELAY 50

class CTransferBox
{
public:
    enum Type
    {
        NORMAL,
        PACKET,
        MAX_TYPES
    };

    CTransferBox();
    virtual ~CTransferBox();

    void Show();
    void Hide();

    void SetInfo(double dDownloadSizeNow, CTransferBox::Type eTransferType = CTransferBox::NORMAL);
    void DoPulse();
    bool IsVisible() const { return m_pWindow->IsVisible(); };

    bool IsEnabled() const;
    void SetEnabled(bool bEnabled) const;

    void   AddToTotalSize(double dSize) { m_dTotalSize += dSize; };
    double GetTotalSize() const { return m_dTotalSize; };

private:
    CGUIWindow*                                       m_pWindow;
    SFixedArray<CGUIStaticImage*, TRANSFERBOX_FRAMES> m_pIcon;
    CGUILabel*                                        m_pInfo;
    CGUIProgressBar*                                  m_pProgress;

    bool m_bMultipleDownloads;

    unsigned int m_uiVisible;
    CElapsedTime m_AnimTimer;
    double       m_dTotalSize;

    SString m_strTransferText[Type::MAX_TYPES];
};
