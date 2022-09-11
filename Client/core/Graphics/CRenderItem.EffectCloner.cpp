/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItem.EffectMan.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRenderItem.EffectCloner.h"
#include "CRenderItem.EffectTemplate.h"

////////////////////////////////////////////////////////////////
//
// CEffectCloner::CEffectCloner
//
//
//
////////////////////////////////////////////////////////////////
CEffectCloner::CEffectCloner(CRenderItemManager* pManager)
{
    m_TidyupTimer.SetMaxIncrement(500, true);
    m_pManager = pManager;
}

////////////////////////////////////////////////////////////////
//
// CEffectCloner::CreateD3DEffect
//
//
//
////////////////////////////////////////////////////////////////
CEffectWrap* CEffectCloner::CreateD3DEffect(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus, bool bDebug,
                                            const EffectMacroList& macros)
{
    SEffectInvariant invariant{ConformPathForSorting(strFile), macros};

    // Do we have a match with the initial path
    CEffectTemplate* pEffectTemplate = MapFindRef(m_ValidMap, invariant);
    if (pEffectTemplate)
    {
        // Have files changed since create?
        if (pEffectTemplate->HaveFilesChanged())
        {
            // EffectTemplate is no good for cloning now, so move it to the old list
            MapRemove(m_ValidMap, invariant);
            m_OldList.push_back(pEffectTemplate);
            pEffectTemplate = NULL;
        }
    }

    static uint uiCallCount = 0;
    uiCallCount++;

    // Need to create new EffectTemplate?
    if (!pEffectTemplate)
    {
        SString strReport;
        HRESULT hr;
        for (uint i = 0; i < 2; i++)
        {
            pEffectTemplate = NewEffectTemplate(m_pManager, strFile, strRootPath, bIsRawData, strOutStatus, bDebug, macros, hr);
            if (pEffectTemplate || hr != E_OUTOFMEMORY || i > 0)
            {
                if (pEffectTemplate && i > 0)
                    strReport += "[Free unused resources success]";
                break;
            }
            // Remove unused effects from memory any try again
            strReport += "[E_OUTOFMEMORY]";
            MaybeTidyUp(true);
        }
        if (!pEffectTemplate)
        {
            strReport += SString("[failed %08x %s]", hr, *strOutStatus);
        }
        else
        {
            // Add to active map
            m_ValidMap[std::move(invariant)] = pEffectTemplate;
        }

        if (!strReport.empty())
        {
            strReport += SString("[effects cur:%d created:%d dest:%d]", g_pDeviceState->MemoryState.Effect.iCurrentCount,
                                 g_pDeviceState->MemoryState.Effect.iCreatedCount, g_pDeviceState->MemoryState.Effect.iDestroyedCount);
            AddReportLog(7544, SString("NewEffectTemplate (call:%d) %s %s", uiCallCount, *strReport, *strFile));
        }
        if (!pEffectTemplate)
            return NULL;
    }

    //
    // Now we have a valid EffectTemplate to clone the effect from
    //

    CEffectWrap* pEffectWrap = pEffectTemplate->CloneD3DEffect(strOutStatus);
    return pEffectWrap;
}

////////////////////////////////////////////////////////////////
//
// CEffectCloner::ReleaseD3DEffect
//
// Remove all refs to the d3d effect
//
////////////////////////////////////////////////////////////////
void CEffectCloner::ReleaseD3DEffect(CEffectWrap* pEffectWrap)
{
    // Remove from pEffectTemplate. This will also release the CEffectWrap
    pEffectWrap->m_pEffectTemplate->UnCloneD3DEffect(pEffectWrap);
}

////////////////////////////////////////////////////////////////
//
// CEffectCloner::DoPulse
//
//
//
////////////////////////////////////////////////////////////////
void CEffectCloner::DoPulse()
{
    MaybeTidyUp();
}

////////////////////////////////////////////////////////////////
//
// CEffectCloner::MaybeTidyUp
//
// Tidy up if been a little while since last time
//
////////////////////////////////////////////////////////////////
void CEffectCloner::MaybeTidyUp(bool bForceDrasticMeasures)
{
    if (!bForceDrasticMeasures && m_TidyupTimer.Get() < 1000)
        return;

    m_TidyupTimer.Reset();

    // Everything in Old List can go if not being used
    for (uint i = 0; i < m_OldList.size(); i++)
    {
        CEffectTemplate* pEffectTemplate = m_OldList[i];
        if (pEffectTemplate->GetTicksSinceLastUsed() > (bForceDrasticMeasures ? 0 : 1))
        {
            OutputDebugLine("[Shader] CEffectCloner::MaybeTidyUp: Releasing old EffectTemplate");
            SAFE_RELEASE(pEffectTemplate);
            ListRemoveIndex(m_OldList, i--);
        }
    }

    // Complex calculation to guess how long to leave an effect unused before deleting
    // 0=30 mins  100=25 mins  200=16 mins  300=1 sec
    float fTicksAlpha = UnlerpClamped(0, m_ValidMap.size(), 300);
    int   iTicks = static_cast<int>((1 - fTicksAlpha * fTicksAlpha) * 30 * 60 * 1000) + 1000;

#ifdef MTA_DEBUG
    iTicks /= 60;            // Mins to seconds for debug
#endif

    // Valid Effect not used for a little while can go
    for (auto iter = m_ValidMap.begin(); iter != m_ValidMap.end();)
    {
        CEffectTemplate* pEffectTemplate = iter->second;
        if (pEffectTemplate->GetTicksSinceLastUsed() > (bForceDrasticMeasures ? 0 : iTicks))
        {
            OutputDebugLine("[Shader] CEffectCloner::MaybeTidyUp: Releasing valid EffectTemplate");
            SAFE_RELEASE(pEffectTemplate);
            m_ValidMap.erase(iter++);
        }
        else
            ++iter;
    }

    if (bForceDrasticMeasures)
    {
        CGraphics::GetSingleton().GetDevice()->EvictManagedResources();
    }
}
