/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTransferBox.cpp
 *  PURPOSE:     Transfer box GUI
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

#define TRANSFERBOX_HEIGHT         58
#define TRANSFERBOX_ICONSIZE       20
#define TRANSFERBOX_PROGRESSHEIGHT 28
#define TRANSFERBOX_YSTART         20
#define TRANSFERBOX_SPACER         11

CTransferBox::CTransferBox(TransferBoxType transferType) : m_GUI(g_pCore->GetGUI())
{
    switch (transferType)
    {
        case TransferBoxType::MAP_DOWNLOAD:
            m_titleProgressPrefix = _("Map download progress:");
            break;
        default:
            m_titleProgressPrefix = _("Download Progress:");
    }

    m_visible.set(TB_VISIBILITY_CLIENT_SCRIPT);
    m_visible.set(TB_VISIBILITY_SERVER_SCRIPT);

    g_pCore->GetCVars()->Get("always_show_transferbox", m_alwaysVisible);

    CreateTransferWindow();
}

void CTransferBox::CreateTransferWindow()
{
    // Find our largest piece of text, so we can size accordingly
    std::string largeTextSample = m_titleProgressPrefix + " " + SString(_("%s of %s"), "999.99 kB", "999.99 kB");
    float       fTransferBoxWidth = m_GUI->GetTextExtent(largeTextSample.c_str(), "default-bold-small");
    fTransferBoxWidth = std::max<float>(fTransferBoxWidth, m_GUI->GetTextExtent(_("Disconnect to cancel download"), "default-normal"));

    // Add some padding to our text for the size of the window
    fTransferBoxWidth += 80;

    CVector2D screenSize = m_GUI->GetResolution();

    m_window.reset(m_GUI->CreateWnd());
    m_window->SetText("");
    m_window->SetAlpha(0.7f);
    m_window->SetVisible(false);
    m_window->SetAlwaysOnTop(true);
    m_window->SetCloseButtonEnabled(false);
    m_window->SetSizingEnabled(false);
    m_window->SetPosition(CVector2D(screenSize.fX * 0.5f - fTransferBoxWidth * 0.5f, screenSize.fY * 0.85f - TRANSFERBOX_HEIGHT * 0.5f));
    m_window->SetSize(CVector2D(fTransferBoxWidth, TRANSFERBOX_HEIGHT));            // relative 0.35, 0.225

    m_progressBar.reset(m_GUI->CreateProgressBar(m_window.get()));
    m_progressBar->SetPosition(CVector2D(0, TRANSFERBOX_YSTART));
    m_progressBar->SetSize(CVector2D(fTransferBoxWidth, TRANSFERBOX_HEIGHT - TRANSFERBOX_YSTART - TRANSFERBOX_SPACER));

    m_infoLabel.reset(m_GUI->CreateLabel(m_progressBar.get(), _("Disconnect to cancel download")));
    float fTempX = (m_progressBar->GetSize().fX - m_GUI->GetTextExtent(m_infoLabel->GetText().c_str()) - TRANSFERBOX_ICONSIZE - 4) * 0.5f;
    m_infoLabel->SetPosition(CVector2D(fTempX + TRANSFERBOX_ICONSIZE + 4, 0));
    m_infoLabel->SetSize(CVector2D(fTransferBoxWidth, TRANSFERBOX_PROGRESSHEIGHT));
    m_infoLabel->SetTextColor(0, 0, 0);
    m_infoLabel->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);

    for (size_t i = 0; i < m_iconImages.size(); ++i)
    {
        SString filePath("cgui\\images\\transferset\\%u.png", i + 1);
        m_iconImages[i].reset(m_GUI->CreateStaticImage(m_progressBar.get()));
        m_iconImages[i]->SetFrameEnabled(false);
        m_iconImages[i]->SetPosition(CVector2D(fTempX, ((TRANSFERBOX_PROGRESSHEIGHT) / 2) - (TRANSFERBOX_ICONSIZE / 2)));
        m_iconImages[i]->SetSize(CVector2D(TRANSFERBOX_ICONSIZE, TRANSFERBOX_ICONSIZE));
        m_iconImages[i]->LoadFromFile(filePath);
        m_iconImages[i]->SetVisible(false);
    }

    m_iconIndex = 0;
    m_iconImages[m_iconIndex]->SetVisible(true);
}

void CTransferBox::Show()
{
    m_visible.set(TB_VISIBILITY_MTA);
    UpdateWindowVisibility();
}

void CTransferBox::Hide()
{
    m_visible.reset(TB_VISIBILITY_MTA);
    UpdateWindowVisibility();

    m_downloadTotalSize = 0;
}

void CTransferBox::SetDownloadProgress(uint64_t downloadedSizeTotal)
{
    SString current = GetDataUnit(downloadedSizeTotal);
    SString total = GetDataUnit(m_downloadTotalSize);
    SString progress = m_titleProgressPrefix + " " + SString(_("%s of %s"), current.c_str(), total.c_str());
    m_window->SetText(progress.c_str());
    m_progressBar->SetProgress(static_cast<float>(static_cast<double>(downloadedSizeTotal) / m_downloadTotalSize));
}

void CTransferBox::DoPulse()
{
    if (m_iconTimer.Get() >= 50)
    {
        m_iconTimer.Reset();
        m_iconImages[m_iconIndex]->SetVisible(false);
        m_iconIndex = (m_iconIndex + 1) % m_iconImages.size();
        m_iconImages[m_iconIndex]->SetVisible(true);
    }
}

bool CTransferBox::SetClientVisibility(bool visible)
{
    if (m_visible[TB_VISIBILITY_CLIENT_SCRIPT] == visible)
        return false;

    m_visible.set(TB_VISIBILITY_CLIENT_SCRIPT, visible);
    UpdateWindowVisibility();
    return true;
}

bool CTransferBox::SetServerVisibility(bool visible)
{
    if (m_visible[TB_VISIBILITY_SERVER_SCRIPT] == visible)
        return false;

    m_visible.set(TB_VISIBILITY_SERVER_SCRIPT, visible);
    UpdateWindowVisibility();
    return true;
}

bool CTransferBox::SetAlwaysVisible(bool visible)
{
    if (m_alwaysVisible == visible)
        return false;

    m_alwaysVisible = visible;
    UpdateWindowVisibility();
    return true;
}

void CTransferBox::UpdateWindowVisibility() const
{
    bool visible = m_visible.all() || (m_visible[TB_VISIBILITY_MTA] && m_alwaysVisible);
    m_window->SetVisible(visible);
}
