#include <StdInc.h>
#include "CBindSettingsTab.h"

extern SBindableGTAControl g_bcControls[];
extern SBindableKey g_bkKeys[];

CBindSettingsTab::CBindSettingsTab(CGUITab* pTab) : 
    m_pTab(pTab)
{
    CGUI *pManager = g_pCore->GetGUI();

    m_bCaptureKey = false;
    /**
    *  Binds tab
    **/
    CVector2D vecTemp, vecSize;
    m_pBindsList = std::unique_ptr<CGUIGridList>(pManager->CreateGridList(m_pTab.get(), false));
    m_pBindsList->SetPosition(CVector2D(10, 15));
    m_pBindsList->SetSize(CVector2D(620, 357));
    m_pBindsList->SetSorting(false);
    m_pBindsList->SetSelectionMode(SelectionModes::CellSingle);
    m_pBindsList->SetDoubleClickHandler(GUI_CALLBACK(&CBindSettingsTab::OnBindsListClick, this));

    m_pTab->GetParent()->GetSize(vecTemp);
    m_pBindsDefButton = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Load defaults")));
    m_pBindsDefButton->SetClickHandler(GUI_CALLBACK(&CBindSettingsTab::OnBindsDefaultClick, this));
    m_pBindsDefButton->AutoSize(NULL, 20.0f, 8.0f);
    m_pBindsDefButton->GetSize(vecSize);
    m_pBindsDefButton->SetPosition(CVector2D(vecTemp.fX - vecSize.fX - 12.0f, 387));
    m_pBindsDefButton->SetZOrderingEnabled(false);

    m_hBind = m_pBindsList->AddColumn(_("DESCRIPTION"), 0.35f);
    m_hPriKey = m_pBindsList->AddColumn(_("KEY"), 0.24f);
    for (int k = 0; k < SecKeyNum; k++)
        m_hSecKeys[k] = m_pBindsList->AddColumn(_("ALT. KEY"), 0.24f);
}


//
// Called when the user clicks on the bind 'Load Defaults' button.
//
bool CBindSettingsTab::OnBindsDefaultClick(CGUIElement* pElement)
{
    // Load the default binds
    CCore::GetSingleton().GetKeyBinds()->LoadDefaultBinds();

    // Clear the binds list
    m_pBindsList->Clear();

    // Re-initialize the binds list
    Initialize();

    return true;
}

// Saves the keybinds
void CBindSettingsTab::ProcessKeyBinds(void)
{
    CKeyBindsInterface *pKeyBinds = CCore::GetSingleton().GetKeyBinds();

    SString strResource;

    // Loop through every row in the binds list
    for (int i = 0; i < m_pBindsList->GetRowCount(); i++)
    {
        // Get the type and keys
        unsigned int uiType = reinterpret_cast <unsigned int> (m_pBindsList->GetItemData(i, m_hBind));
        const char* szPri = m_pBindsList->GetItemText(i, m_hPriKey);
        const SBindableKey* pPriKey = pKeyBinds->GetBindableFromKey(szPri);
        const SBindableKey* pSecKeys[SecKeyNum];
        for (int k = 0; k < SecKeyNum; k++)
        {
            const char* szSec = m_pBindsList->GetItemText(i, m_hSecKeys[k]);
            pSecKeys[k] = pKeyBinds->GetBindableFromKey(szSec);
        }
        // If it is a resource name
        if (uiType == 255)
        {
            strResource = m_pBindsList->GetItemText(i, m_hBind);
        }
        // If the type is control
        else if (uiType == KEY_BIND_GTA_CONTROL)
        {
            // Get the previous bind
            CGTAControlBind* pBind = reinterpret_cast <CGTAControlBind*> (m_pBindsList->GetItemData(i, m_hPriKey));
            if (pBind)
            {
                // If theres a primary key, change it
                if (pPriKey)
                    pBind->boundKey = pPriKey;
                else
                    // If not remove the bind
                    pKeyBinds->Remove(pBind);
            }

            for (int k = 0; k < SecKeyNum; k++)
            {
                CGTAControlBind* pSecBind = reinterpret_cast <CGTAControlBind*> (m_pBindsList->GetItemData(i, m_hSecKeys[k]));
                if (pSecBind)
                {
                    // If theres a secondary key, change it
                    if (pSecKeys[k])
                    {
                        if (pSecBind->boundKey != pSecKeys[k])
                        {
                            if (!pKeyBinds->GTAControlExists(pSecKeys[k], pSecBind->control))
                            {
                                pSecBind->boundKey = pSecKeys[k];
                            }
                            else
                            {
                                // If not remove the bind
                                pKeyBinds->Remove(pSecBind);
                            }
                        }
                    }
                    else
                    {
                        // If not remove the bind
                        pKeyBinds->Remove(pSecBind);
                    }
                }
                else if (pSecKeys[k] && pBind)
                {
                    SBindableGTAControl* pControl = pBind->control;
                    if (!pKeyBinds->GTAControlExists(pSecKeys[k], pControl))
                        pKeyBinds->AddGTAControl(pSecKeys[k], pControl);
                }
            }

        }
        // If the type is an empty control (wasn't bound before)
        else if (uiType == KEY_BIND_UNDEFINED)
        {
            // Grab the stored control
            SBindableGTAControl* pControl = reinterpret_cast <SBindableGTAControl*> (m_pBindsList->GetItemData(i, m_hPriKey));
            if (pPriKey)
                // If theres a new key for primary, add a new bind
                pKeyBinds->AddGTAControl(pPriKey, pControl);
            for (int k = 0; k < SecKeyNum; k++)
                if (pSecKeys[k])
                    // If theres a new key for secondary, add a new bind
                    pKeyBinds->AddGTAControl(pSecKeys[k], pControl);
        }
        // If the type is a command
        else if (uiType == KEY_BIND_COMMAND)
        {
            SString strCmdArgs = m_pBindsList->GetItemText(i, m_hBind);

            SString strCommand, strArguments;
            strCmdArgs.Split(": ", &strCommand, &strArguments);

            const char* szCommand = strCommand;
            const char* szArguments = strArguments.empty() ? NULL : strArguments.c_str();

            /** Primary keybinds **/
            CCommandBind* pBind = reinterpret_cast <CCommandBind*> (m_pBindsList->GetItemData(i, m_hPriKey));
            // If a keybind for this command already exists
            if (pBind)
            {
                // If the user specified a valid primary key
                if (pPriKey)
                {
                    // If the primary key is different than the original one
                    if (pPriKey != pBind->boundKey) {
                        // Did we have any keys with the same "up" state?
                        CCommandBind* pUpBind = pKeyBinds->FindMatchingUpBind(pBind);
                        if (pUpBind)
                        {
                            pKeyBinds->UserChangeCommandBoundKey(pUpBind, pPriKey);
                        }

                        pKeyBinds->UserChangeCommandBoundKey(pBind, pPriKey);
                    }
                }
                // If the primary key field was empty, we can remove the keybind
                else {
                    // Remove any matching "up" state binds we may have
                    CCommandBind* pUpBind = pKeyBinds->FindMatchingUpBind(pBind);
                    if (pUpBind)
                    {
                        pKeyBinds->UserRemoveCommandBoundKey(pUpBind);
                    }
                    pKeyBinds->UserRemoveCommandBoundKey(pBind);
                }
            }
            // If there was no keybind for this command, create it
            else if (pPriKey)
            {
                if (strResource.empty())
                    pKeyBinds->AddCommand(pPriKey, szCommand, szArguments);
                else
                    pKeyBinds->AddCommand(pPriKey->szKey, szCommand, szArguments, true, strResource.c_str());
            }

            /** Secondary keybinds **/
            for (int k = 0; k < SecKeyNum; k++)
            {
                pBind = reinterpret_cast <CCommandBind*> (m_pBindsList->GetItemData(i, m_hSecKeys[k]));
                // If this is a valid bind in the keybinds list
                if (pBind)
                {
                    // And our secondary key field was not empty
                    if (pSecKeys[k])
                    {
                        if (pSecKeys[k] != pBind->boundKey)
                        {
                            // Did we have any keys with the same "up" state?
                            CCommandBind* pUpBind = pKeyBinds->FindMatchingUpBind(pBind);
                            if (pUpBind)
                            {
                                pKeyBinds->UserChangeCommandBoundKey(pUpBind, pSecKeys[k]);
                            }
                            pKeyBinds->UserChangeCommandBoundKey(pBind, pSecKeys[k]);
                        }
                    }
                    // If the secondary key field was empty, we should remove the keybind
                    else
                    {
                        // Remove any matching "up" state binds we may have
                        CCommandBind* pUpBind = pKeyBinds->FindMatchingUpBind(pBind);
                        if (pUpBind)
                        {
                            pKeyBinds->UserRemoveCommandBoundKey(pUpBind);
                        }
                        pKeyBinds->UserRemoveCommandBoundKey(pBind);
                    }
                }
                // If this key bind didn't exist, create it
                else if (pSecKeys[k])
                {
                    if (strResource.empty())
                        pKeyBinds->AddCommand(pSecKeys[k], szCommand, szArguments);
                    else
                        pKeyBinds->AddCommand(pSecKeys[k]->szKey, szCommand, szArguments, true, strResource.c_str());

                    // Also add a matching "up" state if applicable
                    CCommandBind* pUpBind = pKeyBinds->GetBindFromCommand(szCommand, NULL, true, pPriKey->szKey, true, false);
                    if (pUpBind)
                    {
                        pKeyBinds->AddCommand(pSecKeys[k]->szKey, szCommand, pUpBind->szArguments, false, pUpBind->szResource);
                    }
                }
            }
        }
        else
        {
            assert(0);
        }
    }
}


bool CBindSettingsTab::OnBindsListClick(CGUIElement* pElement)
{
    CGUIListItem *pItem = m_pBindsList->GetSelectedItem();
    if (pItem)
    {
        CGUIListItem *pItemBind = m_pBindsList->GetItem(m_pBindsList->GetItemRowIndex(pItem), m_hBind);

        // Proceed if the user didn't select the "Bind"-column
        if (pItem != pItemBind)
        {
            m_uiCaptureKey = 0;
            m_pSelectedBind = pItem;
            m_bCaptureKey = true;

            // Determine if the primary or secondary column was selected
            if (m_pBindsList->GetItemColumnIndex(pItem) == 1/*m_hPriKey  Note: handle is not the same as index */) {
                // Create a messagebox to notify the user
                //SString strText = SString::Printf ( "Press a key to bind to '%s'", pItemBind->GetText ().c_str () );
                SString strText = _("Press a key to bind, or escape to clear");
                CCore::GetSingleton().ShowMessageBox(_("Binding a primary key"), strText, MB_ICON_QUESTION);
            }
            else {
                // Create a messagebox to notify the user
                //sSString strText = SString::Printf ( "Press a key to bind to '%s'", pItemBind->GetText ().c_str () );
                SString strText = _("Press a key to bind, or escape to clear");
                CCore::GetSingleton().ShowMessageBox(_("Binding a secondary key"), strText, MB_ICON_QUESTION);
            }
        }
    }

    return true;
}

#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B
#endif
#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP 0x020C
#endif

bool CBindSettingsTab::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_bCaptureKey)
    {
        CKeyBindsInterface *pKeyBinds = CCore::GetSingleton().GetKeyBinds();

        if (uMsg != WM_KEYDOWN && uMsg != WM_KEYUP && uMsg != WM_SYSKEYDOWN &&
            uMsg != WM_SYSKEYUP && uMsg != WM_LBUTTONDOWN && uMsg != WM_LBUTTONUP &&
            uMsg != WM_RBUTTONDOWN && uMsg != WM_RBUTTONUP && uMsg != WM_MBUTTONDOWN &&
            uMsg != WM_MBUTTONUP && uMsg != WM_XBUTTONDOWN && uMsg != WM_XBUTTONUP &&
            uMsg != WM_MOUSEWHEEL)
        {
            return false;
        }

        // Ignore the first key, which is the one that initiated the capture process
        if (m_uiCaptureKey == 0)
        {
            m_uiCaptureKey++;
            return false;
        }

        bool bState;
        const SBindableKey* pKey = pKeyBinds->GetBindableFromMessage(uMsg, wParam, lParam, bState);

        // If escape was pressed, don't create a bind
        if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
        {
            m_pSelectedBind->SetText(CORE_SETTINGS_NO_KEY);
        }
        else if (pKey)
        {
            m_pSelectedBind->SetText(pKey->szKey);
        }
        else
        {
            return false;
        }

        // Remove the messagebox we created earlier
        CCore::GetSingleton().RemoveMessageBox();

        // Make sure the list gets redrawed/updated
        m_pBindsList->Activate();

        m_bCaptureKey = false;
        return true;
    }
    return false;
}


void CBindSettingsTab::Initialize(void)
{
    // Add binds and sections
    bool bPrimaryKey = true;
    int iBind = 0, iRowGame;

    // Add the rows
    CKeyBinds* pKeyBinds = reinterpret_cast <CKeyBinds*> (CCore::GetSingleton().GetKeyBinds());
    iRowGame = m_pBindsList->AddRow();
    m_pBindsList->SetItemText(iRowGame, m_hBind, _("GTA GAME CONTROLS"), false, true);
    m_pBindsList->SetItemText(m_pBindsList->AddRow(), m_hBind, CORE_SETTINGS_HEADER_SPACER, false, true);
    m_pBindsList->SetItemText(m_pBindsList->AddRow(), m_hBind, _("MULTIPLAYER CONTROLS"), false, true);
    //iRows = CORE_SETTINGS_HEADERS;            // (game keys), (multiplayer keys)
    int iGameRowCount = 1;
    int iMultiplayerRowCount = 2;

    std::list < SKeyBindSection * > ::const_iterator iters = m_pKeyBindSections.begin();
    for (; iters != m_pKeyBindSections.end(); iters++)
    {
        (*iters)->currentIndex = -1;
        (*iters)->rowCount = 0;
    }

    pKeyBinds->SortCommandBinds();

    // Loop through all the available controls
    int i;
    for (i = 0; *g_bcControls[i].szControl != NULL; i++);
    for (i--; i >= 0; i--)
    {
        SBindableGTAControl* pControl = &g_bcControls[i];

        // Loop through the binds for a matching control
        unsigned int uiMatchCount = 0;
        for (auto& pBind : pKeyBinds->GetBinds())
        {
            // Is it a control bind
            if (pBind->GetType() == KEY_BIND_GTA_CONTROL)
            {
                const CGTAControlBind* pGTABind = static_cast <const CGTAControlBind*> (pBind.get());
                if (pGTABind->control == pControl)
                {
                    // Primary key?
                    if (uiMatchCount == 0)
                    {
                        // Add bind to the list
                        iBind = m_pBindsList->InsertRowAfter(iRowGame);
                        m_pBindsList->SetItemText(iBind, m_hBind, _(pControl->szDescription));
                        m_pBindsList->SetItemText(iBind, m_hPriKey, pGTABind->boundKey->szKey);
                        for (int k = 0; k < SecKeyNum; k++)
                            m_pBindsList->SetItemText(iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY);
                        m_pBindsList->SetItemData(iBind, m_hBind, (void*)KEY_BIND_GTA_CONTROL);
                        m_pBindsList->SetItemData(iBind, m_hPriKey, (void*)&pGTABind);
                        iGameRowCount++;
                    }
                    // Secondary keys?
                    else
                        for (int k = 0; k < SecKeyNum; k++)
                            if (uiMatchCount == k + 1)
                            {
                                m_pBindsList->SetItemText(iBind, m_hSecKeys[k], pGTABind->boundKey->szKey);
                                m_pBindsList->SetItemData(iBind, m_hSecKeys[k], (void*)&pGTABind);
                            }
                    uiMatchCount++;
                }
            }
        }
        // If we didnt find any matches
        if (uiMatchCount == 0)
        {
            iBind = m_pBindsList->InsertRowAfter(iRowGame);
            m_pBindsList->SetItemText(iBind, m_hBind, pControl->szDescription);
            m_pBindsList->SetItemText(iBind, m_hPriKey, CORE_SETTINGS_NO_KEY);
            for (int k = 0; k < SecKeyNum; k++)
                m_pBindsList->SetItemText(iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY);
            m_pBindsList->SetItemData(iBind, m_hBind, (void*)KEY_BIND_UNDEFINED);
            m_pBindsList->SetItemData(iBind, m_hPriKey, pControl);
            iGameRowCount++;
        }
    }

    struct SListedCommand
    {
        int iIndex;
        const CCommandBind* pBind;
        unsigned int uiMatchCount;
    };

    SListedCommand* listedCommands = new SListedCommand[pKeyBinds->Count(KEY_BIND_COMMAND) + pKeyBinds->Count(KEY_BIND_FUNCTION)];
    unsigned int uiNumListedCommands = 0;
    std::map < std::string, int > iResourceItems;
    // Loop through all the bound commands
    for (auto& pBind : pKeyBinds->GetBinds())
    {
        // keys bound to a console command or a function (we don't show keys bound 
        // from gta controls by scripts as these are clearly not user editable)
        if (pBind->GetType() == KEY_BIND_COMMAND)
        {
            const CCommandBind* pCommandBind = reinterpret_cast <const CCommandBind*> (pBind.get());
            if (pCommandBind->bHitState)
            {
                bool bFoundMatches = false;
                // Loop through the already listed array of commands for matches
                for (unsigned int i = 0; i < uiNumListedCommands; i++)
                {
                    SListedCommand* pListedCommand = &listedCommands[i];
                    const CCommandBind* pListedBind = pListedCommand->pBind;
                    if (!strcmp(pListedBind->szCommand, pCommandBind->szCommand))
                    {
                        if (!pListedBind->szArguments || (pCommandBind->szArguments && !strcmp(pListedBind->szArguments, pCommandBind->szArguments)))
                        {
                            // If we found a 1st match, add it to the secondary section
                            bFoundMatches = true;
                            for (int k = 0; k < SecKeyNum; k++)
                                if (pListedCommand->uiMatchCount == k)
                                {
                                    m_pBindsList->SetItemText(pListedCommand->iIndex, m_hSecKeys[k], pCommandBind->boundKey->szKey);
                                    m_pBindsList->SetItemData(pListedCommand->iIndex, m_hSecKeys[k], (void*)&pCommandBind);
                                }
                            pListedCommand->uiMatchCount++;
                        }
                    }
                }

                // If there weren't any matches
                if (!bFoundMatches)
                {
                    unsigned int row = iGameRowCount + 1;
                    // Combine command and arguments
                    SString strDescription;
                    bool bSkip = false;
                    if (pCommandBind->szResource)
                    {
                        if (pCommandBind->bActive)
                        {
                            const char* szResource = pCommandBind->szResource;
                            std::string strResource = szResource;
                            if (iResourceItems.count(strResource) == 0)
                            {
                                iBind = m_pBindsList->AddRow(true);
                                m_pBindsList->SetItemText(iBind, m_hBind, CORE_SETTINGS_HEADER_SPACER, false, true);

                                iBind = m_pBindsList->AddRow(true);
                                m_pBindsList->SetItemText(iBind, m_hBind, szResource, false, true);
                                m_pBindsList->SetItemData(iBind, m_hBind, (void*)255);
                                iResourceItems.insert(make_pair(strResource, iBind));
                            }
                            row = iResourceItems[strResource];
                            iMultiplayerRowCount++;
                        }
                        else
                            continue;
                    }
                    if (pCommandBind->szArguments && pCommandBind->szArguments[0] != '\0')
                    {
                        strDescription.Format("%s: %s", pCommandBind->szCommand, pCommandBind->szArguments);
                        iMultiplayerRowCount++;
                    }
                    else
                    {
                        strDescription = pCommandBind->szCommand;
                        iMultiplayerRowCount++;
                    }

                    if (!bSkip)
                    {
                        // Add the bind to the list
                        iBind = m_pBindsList->AddRow(true);
                        m_pBindsList->SetItemText(iBind, m_hBind, strDescription);
                        m_pBindsList->SetItemText(iBind, m_hPriKey, pCommandBind->boundKey->szKey);
                        for (int k = 0; k < SecKeyNum; k++)
                            m_pBindsList->SetItemText(iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY);
                        m_pBindsList->SetItemData(iBind, m_hBind, (void*)KEY_BIND_COMMAND);
                        m_pBindsList->SetItemData(iBind, m_hPriKey, (void*)pCommandBind);

                        // Add it to the already-listed array
                        SListedCommand* pListedCommand = &listedCommands[uiNumListedCommands];
                        pListedCommand->iIndex = iBind;
                        pListedCommand->pBind = pCommandBind;
                        pListedCommand->uiMatchCount = 0;
                        uiNumListedCommands++;
                    }
                }
            }
        }
    }

    delete[] listedCommands;
}


void CBindSettingsTab::RemoveKeyBindSection(char * szSectionName)
{
    std::list < SKeyBindSection* >::iterator iter = m_pKeyBindSections.begin();
    for (; iter != m_pKeyBindSections.end(); iter++)
    {
        if (strcmp((*iter)->szOriginalTitle, szSectionName) == 0)
        {
            delete *iter;
            m_pKeyBindSections.erase(iter);
            break;
        }
    }
}

void CBindSettingsTab::RemoveAllKeyBindSections(void)
{
    std::list < SKeyBindSection* >::const_iterator iter = m_pKeyBindSections.begin();
    for (; iter != m_pKeyBindSections.end(); iter++)
    {
        SKeyBindSection * section = (*iter);
        delete section;
    }
    m_pKeyBindSections.clear();
}

void CBindSettingsTab::AddKeyBindSection(char* szSectionName)
{
    m_pKeyBindSections.push_back(new SKeyBindSection(szSectionName));
}

void CBindSettingsTab::SaveData(void)
{

}

bool CBindSettingsTab::OnMouseDoubleClick(CGUIMouseEventArgs Args)
{
    if (Args.pWindow == m_pBindsList.get()) {
        OnBindsListClick(m_pBindsList.get());
        return true;
    }
    return false;
}
