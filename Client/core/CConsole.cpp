/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CConsole.cpp
 *  PURPOSE:     In-game console implementation
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using SharedUtil::CalcMTASAPath;
using std::string;

#define CONSOLE_INPUT_HISTORY_LENGTH 128
#define CONSOLE_SIZE 4096

#define MAX_CONSOLE_COMMAND_LENGTH 255

#define NATIVE_RES_X 1152.0f
#define NATIVE_RES_Y 864.0f

CConsole::CConsole(CGUI* pManager, CGUIElement* pParent)
{
    // Store the GUI manager
    m_pManager = pManager;

    // Create our history
    m_pConsoleHistory = new CEntryHistory(CONSOLE_INPUT_HISTORY_LENGTH);
    m_iHistoryIndex = -1;
    m_iAutoCompleteIndex = -1;
    m_bIsEnabled = true;

    m_fWindowSpacer = 9.0f;
    m_fWindowSpacerTop = 23.0f;
    m_fWindowX = 550.0f;
    m_fWindowY = 360.0f;
    m_fInputHeight = 29.0f;

    // Create GUI elements
    CreateElements();

    // Add the eventhandlers we use
    m_pWindow->SetCloseClickHandler(GUI_CALLBACK(&CConsole::OnCloseButtonClick, this));
    m_pWindow->SetSizedHandler(GUI_CALLBACK(&CConsole::OnWindowSize, this));

    m_pInput->SetTextAcceptedHandler(GUI_CALLBACK(&CConsole::Edit_OnTextAccepted, this));
    m_pInput->SetMaxLength(MAX_CONSOLE_COMMAND_LENGTH);

    m_pHistory->SetTextChangedHandler(GUI_CALLBACK(&CConsole::History_OnTextChanged, this));

    // Load the console history from a file
    m_pConsoleHistory->LoadFromFile(MTA_CONSOLE_INPUT_LOG_PATH);
}

CConsole::~CConsole()
{
    // Delete the GUI elements
    DestroyElements();

    // Delete our history
    delete m_pConsoleHistory;
}

void CConsole::Echo(const char* szText)
{
    // Add to add buffer
    m_strPendingAdd += szText;
    if (!m_strPendingAdd.EndsWith("\n"))
        m_strPendingAdd += "\n";

    // Trim add buffer
    if (m_strPendingAdd.length() > CONSOLE_SIZE)
    {
        m_strPendingAdd = m_strPendingAdd.Right(CONSOLE_SIZE);
        m_strPendingAdd = m_strPendingAdd.SplitRight("\n");
        m_pHistory->SetText("");
    }

    // Flush add buffer is window is visible
    if (m_pWindow->IsVisible())
        FlushPendingAdd();

    CConsoleLogger::GetSingleton().LinePrintf("[Output] : %s", szText);
}

void CConsole::Print(const char* szText)
{
    Echo(szText);
}

void CConsole::Printf(const char* szFormat, ...)
{
    // Parse the formatted string to a string we can echo
    char    szBuffer[1024];
    va_list ap;
    va_start(ap, szFormat);
    VSNPRINTF(szBuffer, 1024, szFormat, ap);
    va_end(ap);

    // Echo it
    Echo(szBuffer);
}

void CConsole::Clear()
{
    // Clear the history buffer.
    // This crashes if there is more than one line in the console
    if (m_pHistory)
    {
        m_pHistory->SetText("");
        m_strPendingAdd.clear();
        m_strSavedInputText.clear();
    }
}

bool CConsole::IsEnabled()
{
    return m_bIsEnabled;
}

void CConsole::SetEnabled(bool bEnabled)
{
    // Hide it if neccessary
    if (!bEnabled && m_pWindow->IsVisible())
    {
        SetVisible(false);
    }

    // Set the enabled bool
    m_bIsEnabled = bEnabled;

    if (bEnabled)
    {
        m_pWindow->Activate();
        m_pWindow->SetEnabled(true);
        m_pWindow->SetAlwaysOnTop(true);
    }
}

bool CConsole::IsVisible()
{
    return m_pWindow->IsVisible();
}

void CConsole::SetVisible(bool bVisible)
{
    // It must be enabled
    if (m_bIsEnabled)
    {
        CMultiplayer* pMultiplayer = CCore::GetSingleton().GetMultiplayer();
        pMultiplayer->DisablePadHandler(bVisible);

        m_pWindow->SetVisible(bVisible);

        // Focus the editbox if we show it, de-focus if we hide
        if (bVisible)
        {
            m_pInput->Activate();
            // Flush pending stuff if becoming visible
            FlushPendingAdd();
        }
        else
        {
            m_pInput->Deactivate();
        }
    }
}

void CConsole::Show()
{
    SetVisible(true);
}

void CConsole::Hide()
{
    SetVisible(false);
}

bool CConsole::IsInputActive()
{
    return IsVisible() && m_pInput->IsActive();
}

void CConsole::ActivateInput()
{
    m_pInput->Activate();
}

bool CConsole::OnCloseButtonClick(CGUIElement* pElement)
{
    SetVisible(false);
    return true;
}

bool CConsole::History_OnTextChanged(CGUIElement* pElement)
{
    // Set the carat to the end of the text
    // TODO: Only do this if the console is scrolled to the bottom
    m_pHistory->EnsureCaratIsVisible();

    // Roger that
    return true;
}

bool CConsole::Edit_OnTextAccepted(CGUIElement* pElement)
{
    string strInput;
    string strHistory;
    string strCmd;
    string strCmdLine;

    // Get the text object from our input window.
    strInput = m_pInput->GetText();

    // If the input isn't empty and isn't identical to the previous entry in history, add it to the history
    if (!strInput.empty() && (m_pConsoleHistory->Empty() || m_pConsoleHistory->GetLast() != strInput))
        m_pConsoleHistory->Add(strInput.c_str());

    // Clear the input text.
    m_pInput->SetText("");
    ResetHistoryChanges();

    // Add the text to the history buffer.
    // Echo ( strInput.c_str () );

    // Write to console log
    CConsoleLogger::GetSingleton().LinePrintf("[Input]  : %s", strInput.c_str());

    // Parse out the command name and command line.
    GetCommandInfo(strInput.c_str(), strCmd, strCmdLine);

    // Execute the command.
    CCommands::GetSingleton().Execute(strCmd.c_str(), strCmdLine.c_str());

    // Success
    return true;
}

void CConsole::GetCommandInfo(const string& strIn, string& strCmdOut, string& strCmdLineOut)
{
    int pos;

    pos = strIn.find(' ');

    if (pos > 0)
    {
        strCmdOut = strIn.substr(0, pos).c_str();
        strCmdLineOut = strIn.substr(pos + 1, strIn.size() - 1).c_str();
    }
    else
    {
        strCmdOut = strIn.c_str();
        strCmdLineOut.clear();
    }
}

void CConsole::GracefullySetEditboxText(const char* szText)
{
    m_pInput->SetText(szText);

    // Reset caret so input is scrolled back if it had a long text before
    m_pInput->SetCaretAtStart();

    // Caret can't be set back to end yet because GUI will not detect any
    // state change and thus will not redraw anything. Just wait for next
    // rendering phase instead and then move the caret back.
    //
    // There is a minimal caret flickering, it can't be prevented without
    // some sort of custom rendering logic.
    m_pInput->SetRenderingEndedHandler(GUI_CALLBACK(&CConsole::GracefullyMoveEditboxCaret, this));
}

bool CConsole::GracefullyMoveEditboxCaret(CGUIElement* pElement)
{
    m_pInput->SetCaretAtEnd();

    // Done, unhook itself
    m_pInput->SetRenderingEndedHandler(nullptr);

    return true;
}

void CConsole::ResetHistoryChanges()
{
    // Reset history selection, any history changes and our saved input
    m_iHistoryIndex = -1;
    m_pConsoleHistory->ResetChanges();
    m_strSavedInputText.clear();
}

void CConsole::SelectHistoryEntry(int iEntry)
{
    int iPreviousHistoryIndex = m_iHistoryIndex;

    // Check if we're in bounds, otherwise clear selection
    if (!m_pConsoleHistory->Empty() && iEntry >= 0 && iEntry < m_pConsoleHistory->Size())
        m_iHistoryIndex = iEntry;
    else
        m_iHistoryIndex = -1;

    SString strInputText = m_pInput->GetText();

    // Save current input as a temporary input value
    if (iPreviousHistoryIndex == -1)
        m_strSavedInputText = strInputText;
    else
        m_pConsoleHistory->Get(iPreviousHistoryIndex)->temp = strInputText;

    // If we haven't selected any history entry, use our saved input text
    if (m_iHistoryIndex == -1)
        GracefullySetEditboxText(m_strSavedInputText.c_str());
    else
    {
        SString& strSelectedInputHistoryEntry = m_pConsoleHistory->Get(m_iHistoryIndex)->temp;
        GracefullySetEditboxText(strSelectedInputHistoryEntry.c_str());
    }
}

bool CConsole::SetNextHistoryText()
{
    // If we can't take input or we're at the end of the list, stop here
    if (!m_pInput->IsActive() || m_iHistoryIndex >= m_pConsoleHistory->Size() - 1)
        return false;

    // Select the previous entry
    SelectHistoryEntry(m_iHistoryIndex + 1);

    return true;
}

bool CConsole::SetPreviousHistoryText()
{
    // If we can't take input, stop here
    if (!m_pInput->IsActive())
        return false;

    // Select the next entry, or the default entry
    if (m_pConsoleHistory->Size() > 0 && m_iHistoryIndex > 0)
        SelectHistoryEntry(m_iHistoryIndex - 1);
    else
        SelectHistoryEntry(-1);

    return true;
}

void CConsole::ResetAutoCompleteMatch()
{
    m_iAutoCompleteIndex = -1;
}

void CConsole::SetNextAutoCompleteMatch()
{
    // Update match list if required
    if (m_iAutoCompleteIndex == -1)
    {
        m_AutoCompleteList.clear();

        // Get current input
        string strInput = m_pInput->GetText();

        if (strInput.length() > 0)
        {
            // Step through the history
            int iIndex = -1;
            while (CEntryHistoryItem* pEntryHistoryItem = m_pConsoleHistory->Get(++iIndex))
            {
                const char* szItem = *pEntryHistoryItem;
                if (strlen(szItem) < 3)            // Skip very short lines
                    continue;

                // Save the index of any matches
                if (strnicmp(szItem, strInput.c_str(), strInput.length()) == 0)
                {
                    if (m_AutoCompleteList.size())            // Dont add duplicates of the previously added line
                    {
                        CEntryHistoryItem* pPrevEntryHistoryItem = m_pConsoleHistory->Get(m_AutoCompleteList.at(m_AutoCompleteList.size() - 1));
                        if (!pPrevEntryHistoryItem)
                            continue;
                        const char* szPrevItem = *pPrevEntryHistoryItem;
                        if (strcmp(szItem, szPrevItem) == 0)
                            continue;
                    }

                    m_AutoCompleteList.push_back(iIndex);
                }
            }
        }
    }

    // Stop if no matches
    if (m_AutoCompleteList.size() == 0)
        return;

    // Step to next match
    m_iAutoCompleteIndex = (m_iAutoCompleteIndex + 1) % m_AutoCompleteList.size();

    // Grab the item and set the input text to it
    CEntryHistoryItem* pHistoryEntryItem = m_pConsoleHistory->Get(m_AutoCompleteList.at(m_iAutoCompleteIndex));
    if (!pHistoryEntryItem)
        return;
    const char* szItem = *pHistoryEntryItem;
    if (szItem)
        GracefullySetEditboxText(szItem);
}

void CConsole::CreateElements(CGUIElement* pParent)
{
    // Adjust window size to resolution
    CVector2D ScreenSize = m_pManager->GetResolution();
    m_fWindowX *= ScreenSize.fX / NATIVE_RES_X;
    m_fWindowY *= ScreenSize.fY / NATIVE_RES_Y;

    // Create window
    m_pWindow = reinterpret_cast<CGUIWindow*>(m_pManager->CreateWnd(pParent, _("CONSOLE")));
    m_pWindow->SetAlwaysOnTop(true);

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    float     yoff = resolution.fY > 600 ? resolution.fY / 12 : 0.0f;
    m_pWindow->SetPosition(CVector2D(resolution.fX / 2 - m_fWindowX / 2, resolution.fY / 2 - m_fWindowY / 2 + yoff), false);

    //    m_pWindow->SetPosition ( CVector2D ( 0.20f, 0.20f ), true );
    m_pWindow->SetSize(CVector2D(m_fWindowX, m_fWindowY));
    m_pWindow->SetMinimumSize(CVector2D(m_fWindowX, m_fWindowY));
    m_pWindow->SetSizingEnabled(true);

    /** History widget
        size x: SPACER - [WINDOW WIDTH] - SPACER
        size y: SPACER (TOP) - [WINDOW HEIGHT] - SPACER/2 - INPUT HEIGHT - SPACER
    */
    CVector2D HistorySize = CVector2D(m_fWindowX - m_fWindowSpacer * 2.0f, m_fWindowY - m_fWindowSpacer * 1.5f - m_fWindowSpacerTop - m_fInputHeight);
    m_pHistory = reinterpret_cast<CGUIMemo*>(m_pManager->CreateMemo(m_pWindow));
    m_pHistory->SetPosition(CVector2D(m_fWindowSpacer, m_fWindowSpacerTop));
    CVector2D RelHistorySize = m_pWindow->AbsoluteToRelative(HistorySize);
    m_pHistory->SetSize(HistorySize);
    m_pHistory->SetReadOnly(true);

    /** Input widget
        pos x: SPACER
        pos y: SPACER (TOP) + HISTORY HEIGHT + SPACER
    */
    m_pInput = reinterpret_cast<CGUIEdit*>(m_pManager->CreateEdit(m_pWindow));
    m_pInput->SetPosition(CVector2D(m_fWindowSpacer, HistorySize.fY + m_fWindowSpacer * 0.5f + m_fWindowSpacerTop));
    m_pInput->SetWidth(HistorySize.fX);
    m_pInput->SetHeight(m_fInputHeight);
}

void CConsole::DestroyElements()
{
    if (m_pWindow)
        delete m_pWindow;
}

CVector2D CConsole::GetPosition()
{
    if (m_pWindow)
    {
        return m_pWindow->GetPosition();
    }
    return CVector2D();
}

void CConsole::SetPosition(CVector2D& vecPosition)
{
    if (m_pWindow)
    {
        m_pWindow->SetPosition(vecPosition);
    }
}

CVector2D CConsole::GetSize()
{
    if (m_pWindow)
    {
        return m_pWindow->GetSize();
    }
    return CVector2D();
}

void CConsole::SetSize(CVector2D& vecSize)
{
    if (m_pWindow)
    {
        // OnWindowSize should do the rest
        m_pWindow->SetSize(vecSize);
    }
}

bool CConsole::OnWindowSize(CGUIElement* pElement)
{
    CVector2D vecSize;
    m_pWindow->GetSize(vecSize);
    m_fWindowX = vecSize.fX;
    m_fWindowY = vecSize.fY;

    CVector2D HistorySize = CVector2D(m_fWindowX - m_fWindowSpacer * 2.0f, m_fWindowY - m_fWindowSpacer * 1.5f - m_fWindowSpacerTop - m_fInputHeight);
    m_pHistory->SetPosition(CVector2D(m_fWindowSpacer, m_fWindowSpacerTop));
    CVector2D RelHistorySize = m_pWindow->AbsoluteToRelative(HistorySize);
    m_pHistory->SetSize(HistorySize);

    m_pInput->SetPosition(CVector2D(m_fWindowSpacer, HistorySize.fY + m_fWindowSpacer * 0.5f + m_fWindowSpacerTop));
    m_pInput->SetWidth(HistorySize.fX);
    m_pInput->SetHeight(m_fInputHeight);

    return true;
}

// Send saved console adds to the actual gui window
void CConsole::FlushPendingAdd()
{
    if (!m_strPendingAdd.empty())
    {
        // Grab the scroll and the max scroll
        float fScroll = m_pHistory->GetVerticalScrollPosition();
        float fMaxScroll = m_pHistory->GetScrollbarDocumentSize() - m_pHistory->GetScrollbarPageSize();

        // Grab selection
        uint uiSelectionStart = m_pHistory->GetSelectionStart();
        uint uiSelectionEnd = m_pHistory->GetSelectionEnd();
        uint uiSelectionLength = m_pHistory->GetSelectionLength();

        // Make new buffer
        SString strBuffer = m_pHistory->GetText();
        strBuffer += m_strPendingAdd;
        m_strPendingAdd.clear();

        // Trim new buffer
        uint uiBufferLength = strBuffer.length();

        if (uiBufferLength > CONSOLE_SIZE)
        {
            strBuffer = strBuffer.Right(CONSOLE_SIZE);
            strBuffer = strBuffer.SplitRight("\n");

            // Fix text selection coords after trimming
            if (uiSelectionLength > 0)
            {
                uint uiBufferLengthDiff = uiBufferLength - strBuffer.length();

                // Beware of underflows, all cases must be properly handled
                if (uiSelectionEnd < uiBufferLengthDiff)
                {
                    // Whole selection would be out of the screen now, so technically
                    // it does not exist anymore
                    uiSelectionStart = 0;
                    uiSelectionEnd = 0;
                    uiSelectionLength = 0;
                }
                else if (uiSelectionStart < uiBufferLengthDiff)
                {
                    // Start of selection would be out of the screen now,
                    // so it must be shortened
                    uiSelectionLength -= uiBufferLengthDiff - uiSelectionStart;
                    uiSelectionStart = 0;
                    uiSelectionEnd -= uiBufferLengthDiff;
                }
                else            // Both start and end of selection are greater than length difference
                {
                    // Whole selection would still be visible on the screen,
                    // just a simple movement is needed
                    uiSelectionStart -= uiBufferLengthDiff;
                    uiSelectionEnd -= uiBufferLengthDiff;
                }
            }
        }

        // Set new buffer
        m_pHistory->SetText(strBuffer);

        // If not at the end, keep the scrollbar position
        if (fScroll < fMaxScroll)
            m_pHistory->SetVerticalScrollPosition(fScroll);

        // Restore text selection if any
        if (uiSelectionLength > 0)
            m_pHistory->SetSelection(uiSelectionStart, uiSelectionEnd);
    }
}
