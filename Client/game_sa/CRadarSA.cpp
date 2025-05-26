/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRadarSA.cpp
 *  PURPOSE:     Game radar
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <CRect.h>
#include "CRadarSA.h"

CMarkerSA* Markers[MAX_MARKERS];

CRadarSA::CRadarSA()
{
    for (int i = 0; i < MAX_MARKERS; i++)
        Markers[i] = new CMarkerSA((CMarkerSAInterface*)(ARRAY_CMarker + i * sizeof(CMarkerSAInterface)));
}

CRadarSA::~CRadarSA()
{
    for (int i = 0; i < MAX_MARKERS; i++)
    {
        if (Markers[i])
            delete Markers[i];
    }
}

CMarker* CRadarSA::CreateMarker(CVector* vecPosition)
{
    CMarkerSA* marker;
    marker = (CMarkerSA*)GetFreeMarker();
    if (marker)
    {
        marker->Init();
        marker->SetPosition(vecPosition);
    }

    return marker;
}

CMarker* CRadarSA::GetFreeMarker()
{
    int Index;
    Index = 0;
    while ((Index < MAX_MARKERS) && (Markers[Index]->GetInterface()->bTrackingBlip))
    {
        Index++;
    }
    if (Index >= MAX_MARKERS)
        return NULL;
    else
        return Markers[Index];
}

void CRadarSA::DrawAreaOnRadar(float fX1, float fY1, float fX2, float fY2, const SharedUtil::SColor color)
{
    // Convert color to required abgr at the last moment
    unsigned long abgr = color.A << 24 | color.B << 16 | color.G << 8 | color.R;
    CRect         myRect(fX1, fY2, fX2, fY1);
    DWORD         dwFunc = FUNC_DrawAreaOnRadar;
    _asm
    {
        push    eax

        push    1           //bool
        lea     eax, abgr
        push    eax
        lea     eax, myRect
        push    eax
        call    dwFunc
        add     esp, 12

        pop     eax
    }
}
