/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientPerfStat.LuaMemory.cpp
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

namespace
{
    //
    // CLuaMainMemory
    //
    class CLuaMainMemory
    {
    public:
        CLuaMainMemory() { memset(this, 0, sizeof(*this)); }

        int Delta;
        int Current;
        int Max;
        int OpenXMLFiles;
        int Refs;
        int TimerCount;
        int ElementCount;
    };

    typedef std::map<CLuaMain*, CLuaMainMemory> CLuaMainMemoryMap;
    class CAllLuaMemory
    {
    public:
        CLuaMainMemoryMap LuaMainMemoryMap;
    };
}            // namespace

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
class CClientPerfStatLuaMemoryImpl : public CClientPerfStatLuaMemory
{
public:
    CClientPerfStatLuaMemoryImpl();
    virtual ~CClientPerfStatLuaMemoryImpl();

    // CClientPerfStatModule
    virtual const SString& GetCategoryName();
    virtual void           DoPulse();
    virtual void           GetStats(CClientPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter);

    // CClientPerfStatLuaMemory
    virtual void OnLuaMainCreate(CLuaMain* pLuaMain);
    virtual void OnLuaMainDestroy(CLuaMain* pLuaMain);

    // CClientPerfStatLuaMemoryImpl
    void GetLuaMemoryStats(CClientPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter);
    void UpdateLuaMemory(CLuaMain* pLuaMain, int iMemUsed);

    SString                  m_strCategoryName;
    CAllLuaMemory            AllLuaMemory;
    std::map<CLuaMain*, int> m_LuaMainMap;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CClientPerfStatLuaMemoryImpl* g_pClientPerfStatLuaMemoryImp = NULL;

CClientPerfStatLuaMemory* CClientPerfStatLuaMemory::GetSingleton()
{
    if (!g_pClientPerfStatLuaMemoryImp)
        g_pClientPerfStatLuaMemoryImp = new CClientPerfStatLuaMemoryImpl();
    return g_pClientPerfStatLuaMemoryImp;
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::CClientPerfStatLuaMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatLuaMemoryImpl::CClientPerfStatLuaMemoryImpl()
{
    m_strCategoryName = "Lua memory";
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::CClientPerfStatLuaMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatLuaMemoryImpl::~CClientPerfStatLuaMemoryImpl()
{
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CClientPerfStatLuaMemoryImpl::GetCategoryName()
{
    return m_strCategoryName;
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::OnLuaMainCreate
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::OnLuaMainCreate(CLuaMain* pLuaMain)
{
    MapSet(m_LuaMainMap, pLuaMain, 1);
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::OnLuaMainDestroy
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::OnLuaMainDestroy(CLuaMain* pLuaMain)
{
    MapRemove(m_LuaMainMap, pLuaMain);
    MapRemove(AllLuaMemory.LuaMainMemoryMap, pLuaMain);
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::LuaMemory
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::UpdateLuaMemory(CLuaMain* pLuaMain, int iMemUsed)
{
    CLuaMainMemory* pLuaMainMemory = MapFind(AllLuaMemory.LuaMainMemoryMap, pLuaMain);
    if (!pLuaMainMemory)
    {
        MapSet(AllLuaMemory.LuaMainMemoryMap, pLuaMain, CLuaMainMemory());
        pLuaMainMemory = MapFind(AllLuaMemory.LuaMainMemoryMap, pLuaMain);
    }

    pLuaMainMemory->Delta += iMemUsed - pLuaMainMemory->Current;
    pLuaMainMemory->Current = iMemUsed;
    pLuaMainMemory->Max = std::max(pLuaMainMemory->Max, pLuaMainMemory->Current);

    pLuaMainMemory->OpenXMLFiles = pLuaMain->GetXMLFileCount();
    pLuaMainMemory->Refs = pLuaMain->m_CallbackTable.size();
    pLuaMainMemory->TimerCount = pLuaMain->GetTimerCount();
    pLuaMainMemory->ElementCount = pLuaMain->GetElementCount();
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::DoPulse()
{
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::GetStats(CClientPerfStatResult* pResult, const std::map<SString, int>& optionMap, const SString& strFilter)
{
    pResult->Clear();
    GetLuaMemoryStats(pResult, optionMap, strFilter);
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatLuaMemoryImpl::GetLuaMemoryStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatLuaMemoryImpl::GetLuaMemoryStats(CClientPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter)
{
    //
    // Set option flags
    //
    bool bHelp = MapContains(strOptionMap, "h");
    bool bAccurate = MapContains(strOptionMap, "a");

    //
    // Process help
    //
    if (bHelp)
    {
        pResult->AddColumn("Lua memory help");
        pResult->AddRow()[0] = "Option h - This help";
        pResult->AddRow()[0] = "Option a - More accurate memory usage - Warning: Can slow server a little";
        return;
    }

    // Fetch mem stats from Lua
    {
        for (std::map<CLuaMain*, int>::iterator iter = m_LuaMainMap.begin(); iter != m_LuaMainMap.end(); ++iter)
        {
            CLuaMain* pLuaMain = iter->first;
            if (pLuaMain->GetVM())
            {
                if (bAccurate)
                    lua_gc(pLuaMain->GetVM(), LUA_GCCOLLECT, 0);

                int iMemUsed = lua_getgccount(pLuaMain->GetVM());
                UpdateLuaMemory(pLuaMain, iMemUsed);
            }
        }
    }

    pResult->AddColumn("name");
    pResult->AddColumn("change");
    pResult->AddColumn("current");
    pResult->AddColumn("max");
    pResult->AddColumn("XMLFiles");
    pResult->AddColumn("refs");
    pResult->AddColumn("Timers");
    pResult->AddColumn("Elements");
    pResult->AddColumn("TextItems");
    pResult->AddColumn("DxFonts");
    pResult->AddColumn("GuiFonts");
    pResult->AddColumn("Textures");
    pResult->AddColumn("Shaders");
    pResult->AddColumn("RenderTargets");
    pResult->AddColumn("ScreenSources");
    pResult->AddColumn("WebBrowsers");
    pResult->AddColumn("VectorGraphics");

    // Calc totals
    if (strFilter == "")
    {
        int calcedCurrent = 0;
        int calcedDelta = 0;
        int calcedMax = 0;
        for (CLuaMainMemoryMap::iterator iter = AllLuaMemory.LuaMainMemoryMap.begin(); iter != AllLuaMemory.LuaMainMemoryMap.end(); ++iter)
        {
            CLuaMainMemory& LuaMainMemory = iter->second;
            calcedCurrent += LuaMainMemory.Current;
            calcedDelta += LuaMainMemory.Delta;
            calcedMax += LuaMainMemory.Max;
        }

        // Add row
        SString* row = pResult->AddRow();

        int c = 0;
        row[c++] = "Lua VM totals";

        if (labs(calcedDelta) >= 1)
        {
            row[c] = SString("%d KB", calcedDelta);
            calcedDelta = 0;
        }
        c++;

        row[c++] = SString("%d KB", calcedCurrent);
        row[c++] = SString("%d KB", calcedMax);

        // Some extra 'all VM' things
        c += 4;
        int TextItemCount = g_pClientGame->GetManager()->GetDisplayManager()->Count();
        int DxFontCount = g_pClientGame->GetManager()->GetRenderElementManager()->GetDxFontCount();
        int GuiFontCount = g_pClientGame->GetManager()->GetRenderElementManager()->GetGuiFontCount();
        int TextureCount = g_pClientGame->GetManager()->GetRenderElementManager()->GetTextureCount();
        int ShaderCount = g_pClientGame->GetManager()->GetRenderElementManager()->GetShaderCount();
        int RenderTargetCount = g_pClientGame->GetManager()->GetRenderElementManager()->GetRenderTargetCount();
        int ScreenSourceCount = g_pClientGame->GetManager()->GetRenderElementManager()->GetScreenSourceCount();
        int WebBrowserCount = g_pClientGame->GetManager()->GetRenderElementManager()->GetWebBrowserCount();
        int VectorGraphicCount = g_pClientGame->GetManager()->GetRenderElementManager()->GetVectorGraphicCount();
        TextItemCount = std::max(TextItemCount - 4, 0);            // Remove count for radar items
        row[c++] = !TextItemCount ? "-" : SString("%d", TextItemCount);
        row[c++] = !DxFontCount ? "-" : SString("%d", DxFontCount);
        row[c++] = !GuiFontCount ? "-" : SString("%d", GuiFontCount);
        row[c++] = !TextureCount ? "-" : SString("%d", TextureCount);
        row[c++] = !ShaderCount ? "-" : SString("%d", ShaderCount);
        row[c++] = !RenderTargetCount ? "-" : SString("%d", RenderTargetCount);
        row[c++] = !ScreenSourceCount ? "-" : SString("%d", ScreenSourceCount);
        row[c++] = !WebBrowserCount ? "-" : SString("%d", WebBrowserCount);
        row[c++] = !VectorGraphicCount ? "-" : SString("%d", VectorGraphicCount);
    }

    // For each VM
    for (CLuaMainMemoryMap::iterator iter = AllLuaMemory.LuaMainMemoryMap.begin(); iter != AllLuaMemory.LuaMainMemoryMap.end(); ++iter)
    {
        CLuaMainMemory& LuaMainMemory = iter->second;
        const SString   strResName = iter->first->GetScriptName();

        // Apply filter
        if (strFilter != "" && strResName.find(strFilter) == SString::npos)
            continue;

        // Add row
        SString* row = pResult->AddRow();

        int c = 0;
        row[c++] = strResName;

        if (labs(LuaMainMemory.Delta) >= 1)
        {
            row[c] = SString("%d KB", LuaMainMemory.Delta);
            LuaMainMemory.Delta = 0;
        }
        c++;

        row[c++] = SString("%d KB", LuaMainMemory.Current);
        row[c++] = SString("%d KB", LuaMainMemory.Max);
        row[c++] = !LuaMainMemory.OpenXMLFiles ? "-" : SString("%d", LuaMainMemory.OpenXMLFiles);
        row[c++] = !LuaMainMemory.Refs ? "-" : SString("%d", LuaMainMemory.Refs);
        row[c++] = !LuaMainMemory.TimerCount ? "-" : SString("%d", LuaMainMemory.TimerCount);
        row[c++] = !LuaMainMemory.ElementCount ? "-" : SString("%d", LuaMainMemory.ElementCount);
    }
}
