/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Marcus Bauer <mabako@gmail.com>
*               Florian Busse <flobu@gmx.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

struct SKeyBindSection
{
    SKeyBindSection(char * szTitle)
    {
        auto uiLength = strlen(szTitle);
        this->currentIndex = 0;
        this->szTitle = new char[uiLength + 1];
        this->szOriginalTitle = new char[uiLength + 1];
        // convert to upper case
        for (unsigned int i = 0; i < uiLength; i++)
        {
            if (isalpha((uchar)szTitle[i]))
                this->szTitle[i] = toupper(szTitle[i]);
            else
                this->szTitle[i] = szTitle[i];
        }

        this->szTitle[uiLength] = '\0';

        strcpy(szOriginalTitle, szTitle);

        this->rowCount = 0;

        headerItem = NULL;
    }

    ~SKeyBindSection()
    {
        if (this->szTitle)
            delete[] this->szTitle;

        if (szOriginalTitle)
            delete[] szOriginalTitle;
    }
    int currentIndex; // temporarily stores the index while the list is being created
    char* szTitle;
    char* szOriginalTitle;
    int rowCount;
    class CGUIListItem * headerItem;
};


class CBindSettingsTab
{
public:
    CBindSettingsTab(CGUITab* pTab);
    inline bool         IsCapturingKey(void) { return m_bCaptureKey; }
    void SaveData();
    void LoadData() {}
    void                Initialize(void);
    void                ProcessKeyBinds(void);
    bool                ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void ClearBindsList() {
        m_pBindsList->Clear();
    }

private:
    bool                OnBindsDefaultClick(CGUIElement* pElement);
    bool                OnBindsListClick(CGUIElement* pElement);

    void                AddKeyBindSection(char * szSectionName);
    void                RemoveKeyBindSection(char * szSectionName);
    void                RemoveAllKeyBindSections(void);

    bool                OnMouseDoubleClick(CGUIMouseEventArgs Args);


    const static int    SecKeyNum = 3;     // Number of secondary keys
    unsigned int        m_uiCaptureKey;
    bool                m_bCaptureKey;
    

    CGUIListItem*	    m_pSelectedBind;




    std::list < SKeyBindSection *> m_pKeyBindSections;
    std::unique_ptr<CGUITab> m_pTab;

    std::unique_ptr<CGUIGridList>       m_pBindsList;
    std::unique_ptr<CGUIButton>         m_pBindsDefButton;
    CGUIHandle          m_hBind, m_hPriKey, m_hSecKeys[SecKeyNum];

};
