/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CLocalServer.cpp
 *  PURPOSE:     Local server setup GUI
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../../../vendor/zip/unzip.h"
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#include "SharedUtil.AsyncTaskScheduler.h"

using std::list;

extern CCoreInterface*         g_pCore;
extern CLocalizationInterface* g_pLocalization;
extern CClientGame*            g_pClientGame;

// SResInfo - Item in list of potential resources - Used in GetResourceNameList()
struct SResInfo
{
    SString strAbsPath;
    SString strName;
    bool    bIsDir;
    bool    bPathIssue;
    SString strAbsPathDup;
};

CLocalServer::CLocalServer(const char* szConfig)
{
    m_strConfig = szConfig;
    m_pConfig = NULL;

    m_pGUI = g_pCore->GetGUI();

    m_pWindow = reinterpret_cast<CGUIWindow*>(m_pGUI->CreateWnd(NULL, _("HOST GAME")));
    m_pWindow->SetMovable(true);

    CVector2D resolution = m_pGUI->GetResolution();
    float     yoff = resolution.fY > 600 ? resolution.fY / 12 : 0.0f;
    m_pWindow->SetPosition(CVector2D(resolution.fX / 2 - 360.0f / 2, resolution.fY / 2 - 440.0f / 2 + yoff), false);
    m_pWindow->SetSize(CVector2D(360.0f, 440.0f));
    m_pWindow->SetSizingEnabled(false);
    m_pWindow->SetAlwaysOnTop(true);
    m_pWindow->SetVisible(true);

    m_pTabs = reinterpret_cast<CGUITabPanel*>(m_pGUI->CreateTabPanel(m_pWindow));
    m_pTabs->SetPosition(CVector2D(0.0f, 0.06f), true);
    m_pTabs->SetSize(CVector2D(1.0f, 0.85f), true);
    m_pTabGeneral = m_pTabs->CreateTab(_("General"));
    // m_pTabs->CreateTab ( "Gamemode" );
    m_pTabResources = m_pTabs->CreateTab(_("Resources"));

    m_pLabelName = reinterpret_cast<CGUILabel*>(m_pGUI->CreateLabel(m_pTabGeneral, _("Server name:")));
    m_pLabelName->SetPosition(CVector2D(0.03f, 0.07f), true);
    m_pLabelName->AutoSize(_("Server name:"));

    m_pEditName = reinterpret_cast<CGUIEdit*>(m_pGUI->CreateEdit(m_pTabGeneral, "Default MTA Server"));
    m_pEditName->SetPosition(CVector2D(0.4f, 0.06f), true);
    m_pEditName->SetSize(CVector2D(0.57f, 0.06f), true);
    m_pEditName->SetMaxLength(64);

    m_pLabelPass = reinterpret_cast<CGUILabel*>(m_pGUI->CreateLabel(m_pTabGeneral, _("Password:")));
    m_pLabelPass->SetPosition(CVector2D(0.03f, 0.14f), true);
    m_pLabelPass->AutoSize(_("Password:"));

    m_pEditPass = reinterpret_cast<CGUIEdit*>(m_pGUI->CreateEdit(m_pTabGeneral));
    m_pEditPass->SetPosition(CVector2D(0.4f, 0.13f), true);
    m_pEditPass->SetSize(CVector2D(0.57f, 0.06f), true);
    m_pEditPass->SetMaxLength(8);

    m_pLabelPlayers = reinterpret_cast<CGUILabel*>(m_pGUI->CreateLabel(m_pTabGeneral, _("Max players:")));
    m_pLabelPlayers->SetPosition(CVector2D(0.03f, 0.21f), true);
    m_pLabelPlayers->AutoSize(_("Max players:"));

    m_pEditPlayers = reinterpret_cast<CGUIEdit*>(m_pGUI->CreateEdit(m_pTabGeneral));
    m_pEditPlayers->SetPosition(CVector2D(0.4f, 0.20f), true);
    m_pEditPlayers->SetSize(CVector2D(0.17f, 0.06f), true);
    m_pEditPlayers->SetMaxLength(3);

    m_pLabelBroadcast = reinterpret_cast<CGUILabel*>(m_pGUI->CreateLabel(m_pTabGeneral, _("Broadcast:")));
    m_pLabelBroadcast->SetPosition(CVector2D(0.03f, 0.35f), true);
    m_pLabelBroadcast->AutoSize(_("Broadcast:"));

    m_pBroadcastLan = reinterpret_cast<CGUICheckBox*>(m_pGUI->CreateCheckBox(m_pTabGeneral, _("LAN"), true));
    m_pBroadcastLan->SetPosition(CVector2D(0.4f, 0.33f), true);
    m_pBroadcastLan->SetSize(CVector2D(0.45f, 0.08f), true);

    m_pBroadcastInternet = reinterpret_cast<CGUICheckBox*>(m_pGUI->CreateCheckBox(m_pTabGeneral, _("Internet"), true));
    m_pBroadcastInternet->SetPosition(CVector2D(0.4f, 0.38f), true);
    m_pBroadcastInternet->SetSize(CVector2D(0.45f, 0.08f), true);

    m_pButtonUpdate = reinterpret_cast<CGUIButton*>(m_pGUI->CreateButton(m_pTabGeneral, _("Update server resources")));
    m_pButtonUpdate->SetPosition(CVector2D(0.45f, 0.91f), true);
    m_pButtonUpdate->SetSize(CVector2D(0.52f, 0.06f), true);
    m_pButtonUpdate->SetZOrderingEnabled(false);

    m_pResourcesCur = reinterpret_cast<CGUIGridList*>(m_pGUI->CreateGridList(m_pTabResources, false));
    m_pResourcesCur->SetPosition(CVector2D(0.03f, 0.06f), true);
    m_pResourcesCur->SetSize(CVector2D(0.45f, 0.5f), true);
    m_pResourcesCur->SetSortingEnabled(false);
    m_pResourcesCur->SetSelectionMode(SelectionModes::CellSingle);
    m_hResourcesCur = m_pResourcesCur->AddColumn(_("Selected"), 0.80f);

    m_pResourceDel = reinterpret_cast<CGUIButton*>(m_pGUI->CreateButton(m_pTabResources, ">"));
    m_pResourceDel->SetPosition(CVector2D(0.03f, 0.65f), true);
    m_pResourceDel->SetSize(CVector2D(0.45f, 0.05f), true);
    m_pResourceDel->SetZOrderingEnabled(false);

    m_pResourceAdd = reinterpret_cast<CGUIButton*>(m_pGUI->CreateButton(m_pTabResources, "<"));
    m_pResourceAdd->SetPosition(CVector2D(0.03f, 0.58f), true);
    m_pResourceAdd->SetSize(CVector2D(0.45f, 0.05f), true);
    m_pResourceAdd->SetZOrderingEnabled(false);

    m_pResourcesAll = reinterpret_cast<CGUIGridList*>(m_pGUI->CreateGridList(m_pTabResources, false));
    m_pResourcesAll->SetPosition(CVector2D(0.52f, 0.06f), true);
    m_pResourcesAll->SetSize(CVector2D(0.45f, 0.9f), true);
    m_pResourcesAll->SetSortingEnabled(false);
    m_pResourcesAll->SetSelectionMode(SelectionModes::CellSingle);
    m_hResourcesAll = m_pResourcesAll->AddColumn(_("All"), 0.80f);

    m_pButtonStart = reinterpret_cast<CGUIButton*>(m_pGUI->CreateButton(m_pWindow, _("Start")));
    m_pButtonStart->SetPosition(CVector2D(0.33f, 0.93f), true);
    m_pButtonStart->SetSize(CVector2D(0.3f, 0.05f), true);
    m_pButtonStart->SetZOrderingEnabled(false);

    m_pButtonCancel = reinterpret_cast<CGUIButton*>(m_pGUI->CreateButton(m_pWindow, _("Cancel")));
    m_pButtonCancel->SetPosition(CVector2D(0.65f, 0.93f), true);
    m_pButtonCancel->SetSize(CVector2D(0.3f, 0.05f), true);
    m_pButtonCancel->SetZOrderingEnabled(false);

    m_pButtonUpdate->SetClickHandler(GUI_CALLBACK(&CLocalServer::OnUpdateResourcesButtonClick, this));
    m_pResourceAdd->SetClickHandler(GUI_CALLBACK(&CLocalServer::OnAddButtonClick, this));
    m_pResourceDel->SetClickHandler(GUI_CALLBACK(&CLocalServer::OnDelButtonClick, this));
    m_pButtonStart->SetClickHandler(GUI_CALLBACK(&CLocalServer::OnStartButtonClick, this));
    m_pButtonCancel->SetClickHandler(GUI_CALLBACK(&CLocalServer::OnCancelButtonClick, this));

    Load();
}

CLocalServer::~CLocalServer()
{
    if (m_pConfig)
        delete m_pConfig;
    delete m_pWindow;
}

bool CLocalServer::OnAddButtonClick(CGUIElement* pElement)
{
    CGUIListItem* pItem = m_pResourcesAll->GetSelectedItem();
    if (pItem)
    {
        m_pResourcesCur->SetItemText(m_pResourcesCur->AddRow(), m_hResourcesCur, &pItem->GetText().c_str()[3]);
        m_pResourcesAll->RemoveRow(m_pResourcesAll->GetItemRowIndex(pItem));
    }
    return true;
}

bool CLocalServer::OnDelButtonClick(CGUIElement* pElement)
{
    CGUIListItem* pItem = m_pResourcesCur->GetSelectedItem();
    if (pItem)
    {
        m_pResourcesAll->SetItemText(m_pResourcesAll->AddRow(), m_hResourcesAll, &pItem->GetText().c_str()[3]);
        m_pResourcesCur->RemoveRow(m_pResourcesCur->GetItemRowIndex(pItem));
    }
    return true;
}

bool CLocalServer::OnUpdateResourcesButtonClick(CGUIElement* pElement)
{
    SString url = "https://mirror-cdn.multitheftauto.com/mtasa/resources/mtasa-resources-latest.zip";
    SString path = CalcMTASAPath(PathJoin("server", "mods", "deathmatch", "mtasa-resources-latest.zip"));
    MakeSureDirExists(ExtractPath(path));

    if (m_pButtonUpdate)
    {
        m_pButtonUpdate->SetText(_("Downloading..."));
        m_pButtonUpdate->SetEnabled(false);
    }

    g_pClientGame->GetAsyncTaskScheduler()->PushTask(
        [url, path]() -> bool
        {
            HRESULT result = URLDownloadToFileA(NULL, url.c_str(), path.c_str(), 0, NULL);
            if (result != S_OK)
                return false;

            SString destPath = CalcMTASAPath(PathJoin("server", "mods", "deathmatch", "resources"));
            unzFile zipFile = unzOpen(path.c_str());

            if (!zipFile)
            {
                FileDelete(path);
                return false;
            }

            if (unzGoToFirstFile(zipFile) != UNZ_OK)
            {
                unzClose(zipFile);
                FileDelete(path);
                return false;
            }

            do
            {
                char fileName[256];
                unz_file_info fileInfo;

                if (unzGetCurrentFileInfo(zipFile, &fileInfo, fileName, sizeof(fileName), nullptr, 0, nullptr, 0) != UNZ_OK)
                    continue;

                SString filePath = PathJoin(destPath, fileName);
                if (fileName[fileInfo.size_filename - 1] == '/')
                {
                    MakeSureDirExists(filePath.c_str());
                    continue;
                }

                MakeSureDirExists(ExtractPath(filePath).c_str());
                if (unzOpenCurrentFile(zipFile) != UNZ_OK)
                    continue;

                // Note: "wb" mode overwrites existing files if they have the same name
                // Any other custom resources on disk that aren't in the zip are left untouched.
                FILE* file = fopen(filePath.c_str(), "wb");
                if (file)
                {
                    char buffer[8192];
                    int read = 0;

                    while ((read = unzReadCurrentFile(zipFile, buffer, sizeof(buffer))) > 0)
                    {
                        fwrite(buffer, 1, read, file);
                    }

                    fclose(file);
                }

                unzCloseCurrentFile(zipFile);
            } while (unzGoToNextFile(zipFile) != UNZ_END_OF_LIST_OF_FILE);

            unzClose(zipFile);
            FileDelete(path);

            return true;
        },
        [this](bool success) {
            if (m_pButtonUpdate)
            {
                m_pButtonUpdate->SetText(_("Update server resources"));
                m_pButtonUpdate->SetEnabled(true);
            }

            if (success)
            {
                Load();
                g_pCore->ShowMessageBox(_("Local Server"), _("Server resources updated successfully!"), MB_BUTTON_OK | MB_ICON_INFO);
            }
            else
                g_pCore->ShowMessageBox(_("Error") + _E("CC90"), _("Failed to download or extract resources."), MB_BUTTON_OK | MB_ICON_ERROR);
        }
    );

    return true;
}

bool CLocalServer::OnStartButtonClick(CGUIElement* pElement)
{
    Save();
    // Connect

    g_pClientGame->StartLocalGame(CClientGame::SERVER_TYPE_LOCAL, m_pEditPass->GetText().c_str());

    return true;
}

bool CLocalServer::OnCancelButtonClick(CGUIElement* pElement)
{
    g_pCore->GetModManager()->RequestUnload();
    return true;
}

bool CLocalServer::Load()
{
    // Get server module root
    SString strServerPath = CalcMTASAPath(PathJoin("server", "mods", "deathmatch"));

    m_pConfig = g_pCore->GetXML()->CreateXML(PathJoin(strServerPath, m_strConfig));
    if (m_pConfig && m_pConfig->Parse())
    {
        CXMLNode* pRoot = m_pConfig->GetRootNode();
        CXMLNode* pServerName = pRoot->FindSubNode("servername", 0);
        if (pServerName)
            m_pEditName->SetText(pServerName->GetTagContent().c_str());
        CXMLNode* pServerPass = pRoot->FindSubNode("password", 0);
        if (pServerPass)
            m_pEditPass->SetText(pServerPass->GetTagContent().c_str());
        CXMLNode* pServerPlayers = pRoot->FindSubNode("maxplayers", 0);
        if (pServerPlayers)
            m_pEditPlayers->SetText(pServerPlayers->GetTagContent().c_str());

        // Read the startup resources
        list<CXMLNode*>::const_iterator iter = pRoot->ChildrenBegin();
        for (; iter != pRoot->ChildrenEnd(); ++iter)
        {
            CXMLNode* pNode = reinterpret_cast<CXMLNode*>(*iter);
            if (pNode->GetTagName().compare("resource") == 0)
            {
                CXMLAttribute* src = pNode->GetAttributes().Find("src");
                if (src && src->GetValue()[1])
                {
                    m_pResourcesCur->SetItemText(m_pResourcesCur->AddRow(), m_hResourcesCur, src->GetValue().c_str());
                }
            }
        }
    }

    // Get list of resource names
    std::vector<SString> resourceNameList;
    GetResourceNameList(resourceNameList, strServerPath);

    // Put resource names into the GUI
    for (std::vector<SString>::iterator iter = resourceNameList.begin(); iter != resourceNameList.end(); ++iter)
        HandleResource(*iter);

    return true;
}

//
// Scan resource directories
//
void CLocalServer::GetResourceNameList(std::vector<SString>& outResourceNameList, const SString& strModPath)
{
    // Make list of potential active resources
    std::map<SString, SResInfo> resInfoMap;

    // Initial search dir
    std::vector<SString> resourcesPathList;
    resourcesPathList.push_back("resources");

    // SString strModPath = g_pServerInterface->GetModManager ()->GetModPath ();
    for (uint i = 0; i < resourcesPathList.size(); i++)
    {
        // Enumerate all files and directories
        SString              strResourcesRelPath = resourcesPathList[i];
        SString              strResourcesAbsPath = PathJoin(strModPath, strResourcesRelPath, "/");
        std::vector<SString> itemList = FindFiles(strResourcesAbsPath, true, true);

        // Check each item
        for (uint i = 0; i < itemList.size(); i++)
        {
            SString strName = itemList[i];

            // Ignore items that start with a dot
            if (strName[0] == '.')
                continue;

            bool bIsDir = DirectoryExists(PathJoin(strResourcesAbsPath, strName));

            // Recurse into [directories]
            if (bIsDir && (strName.BeginsWith("#") || (strName.BeginsWith("[") && strName.EndsWith("]"))))
            {
                resourcesPathList.push_back(PathJoin(strResourcesRelPath, strName));
                continue;
            }

            // Extract file extension
            SString strExt;
            if (!bIsDir)
                ExtractExtension(strName, &strName, &strExt);

            // Ignore files that are not .zip
            if (!bIsDir && strExt != "zip")
                continue;

            // Ignore items that have dot or space in the name
            if (strName.Contains(".") || strName.Contains(" "))
            {
                CLogger::LogPrintf("WARNING: Not loading resource '%s' as it contains illegal characters\n", *strName);
                continue;
            }

            // Ignore dir items with no meta.xml (assume it's the result of saved files from a zipped resource)
            if (bIsDir && !FileExists(PathJoin(strResourcesAbsPath, strName, "meta.xml")))
                continue;

            // Add potential resource to list
            SResInfo newInfo;
            newInfo.strAbsPath = strResourcesAbsPath;
            newInfo.strName = strName;
            newInfo.bIsDir = bIsDir;
            newInfo.bPathIssue = false;

            // Check for duplicate
            if (SResInfo* pDup = MapFind(resInfoMap, strName))
            {
                // Is path the same?
                if (newInfo.strAbsPath == pDup->strAbsPath)
                {
                    if (newInfo.bIsDir)
                    {
                        // If non-zipped item, replace already existing zipped item on the same path
                        assert(!pDup->bIsDir);
                        *pDup = newInfo;
                    }
                }
                else
                {
                    // Don't load resource if there are duplicates on different paths
                    pDup->bPathIssue = true;
                    pDup->strAbsPathDup = newInfo.strAbsPath;
                }
            }
            else
            {
                // No duplicate found
                MapSet(resInfoMap, strName, newInfo);
            }
        }
    }

    // Print important errors
    for (std::map<SString, SResInfo>::const_iterator iter = resInfoMap.begin(); iter != resInfoMap.end(); ++iter)
    {
        const SResInfo& info = iter->second;
        if (info.bPathIssue)
        {
            CLogger::ErrorPrintf("Not processing resource '%s' as it has duplicates on different paths:\n", *info.strName);
            CLogger::LogPrintfNoStamp("                  Path #1: \"%s\"\n", *PathJoin(PathMakeRelative(strModPath, info.strAbsPath), info.strName));
            CLogger::LogPrintfNoStamp("                  Path #2: \"%s\"\n", *PathJoin(PathMakeRelative(strModPath, info.strAbsPathDup), info.strName));
        }
        else
        {
            outResourceNameList.push_back(info.strName);
        }
    }
}

bool CLocalServer::Save()
{
    if (m_pConfig && m_pConfig->GetRootNode())
    {
        StoreConfigValue("servername", (m_pEditName->GetText().length() > 0) ? m_pEditName->GetText().c_str() : "MTA Local Server");
        StoreConfigValue("maxplayers", (atoi(m_pEditPlayers->GetText().c_str())) ? m_pEditPlayers->GetText().c_str() : "32");
        StoreConfigValue("donotbroadcastlan", (m_pBroadcastLan->IsActive()) ? "0" : "1");
        StoreConfigValue("ase", (m_pBroadcastInternet->IsActive()) ? "1" : "0");
        StoreConfigValue("password", m_pEditPass->GetText().c_str());

        // Remove old resources from the config
        CXMLNode*                       pRoot = m_pConfig->GetRootNode();
        list<CXMLNode*>::const_iterator iter = pRoot->ChildrenBegin();
        for (; iter != pRoot->ChildrenEnd(); ++iter)
        {
            CXMLNode* pNode = reinterpret_cast<CXMLNode*>(*iter);
            if (pNode->GetTagName().compare("resource") == 0)
            {
                pRoot->DeleteSubNode(pNode);
                iter = pRoot->ChildrenBegin();
            }
        }

        // Add new resources to the config
        for (int i = 0; i < m_pResourcesCur->GetRowCount(); i++)
        {
            CXMLNode* pResourceNode = pRoot->CreateSubNode("resource");
            pResourceNode->GetAttributes().Create("src")->SetValue(m_pResourcesCur->GetItemText(i, 1));
            pResourceNode->GetAttributes().Create("startup")->SetValue("1");
            pResourceNode->GetAttributes().Create("protected")->SetValue("0");
        }
        m_pConfig->Write();
    }
    return true;
}

void CLocalServer::StoreConfigValue(const char* szNode, const char* szValue)
{
    CXMLNode* pRoot = m_pConfig->GetRootNode();
    CXMLNode* pNode = pRoot->FindSubNode(szNode, 0);
    if (pNode)
    {
        pNode->SetTagContent(szValue);
    }
    else
    {
        pNode = pRoot->CreateSubNode(szNode);
        pNode->SetTagContent(szValue);
    }
}

void CLocalServer::HandleResource(const char* szResource)
{
    for (int i = 0; i < m_pResourcesCur->GetRowCount(); i++)
    {
        if (strcmp(szResource, m_pResourcesCur->GetItemText(i, 1)) == 0)
            return;
    }
    m_pResourcesAll->SetItemText(m_pResourcesAll->AddRow(), m_hResourcesAll, szResource);
}
