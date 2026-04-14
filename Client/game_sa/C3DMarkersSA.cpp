/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C3DMarkersSA.cpp
 *  PURPOSE:     3D Marker entity manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "C3DMarkersSA.h"

C3DMarkersSA::C3DMarkersSA()
{
    for (int i = 0; i < MAX_3D_MARKERS; i++)
    {
        Markers[i] = new C3DMarkerSA((C3DMarkerSAInterface*)(ARRAY_3D_MARKERS + i * sizeof(C3DMarkerSAInterface)));
    }
}

C3DMarkersSA::~C3DMarkersSA()
{
    for (int i = 0; i < MAX_3D_MARKERS; i++)
    {
        delete Markers[i];
    }
}

C3DMarker* C3DMarkersSA::CreateMarker(DWORD Identifier, T3DMarkerType dwType, CVector* vecPosition, float fSize, float fPulseFraction, BYTE r, BYTE g, BYTE b,
                                      BYTE a)
{
    /*
    static C3dMarker *PlaceMarker(unsigned int nIdentifier, unsigned short nType,
    CVector &vecPosition, float fSize, unsigned char r, unsigned char g, unsigned char b, unsigned char a,
    unsigned short nPeriod, float fPulseFrac, short nRotRate, float normalX = 0.0f,
    float normalY = 0.0f, float normalZ = 0.0f, bool zCheck = FALSE);
    */
    WORD wType = (WORD)dwType;
    dwType = (T3DMarkerType)wType;
    bool bZCheck = true;

    // Pass a copy of the position to PlaceMarker, not the original pointer.
    CVector  vecPositionCopy = *vecPosition;
    CVector* pVecPosCopy = &vecPositionCopy;

    DWORD dwFunc = FUNC_PlaceMarker;
    DWORD dwReturn = 0;
    // clang-format off
    dwReturn = gta_call_address<decltype(dwReturn)>(dwFunc, Identifier, dwType, pVecPosCopy, fSize, r, g, b, a, 0, fPulseFraction, 0, 0, 0, 0, bZCheck);
    // clang-format on

    if (dwReturn)
    {
        for (int i = 0; i < MAX_3D_MARKERS; i++)
        {
            if (Markers[i]->GetInterface() == (C3DMarkerSAInterface*)dwReturn)
            {
                // Markers[i]->Reset(); // debug stuff
                return Markers[i];
            }
        }
    }

    return NULL;
}

C3DMarker* C3DMarkersSA::FindFreeMarker()
{
    for (int i = 0; i < MAX_3D_MARKERS; i++)
    {
        if (!Markers[i]->IsActive())
            return Markers[i];
    }
    return NULL;
}

C3DMarker* C3DMarkersSA::FindMarker(DWORD Identifier)
{
    for (int i = 0; i < MAX_3D_MARKERS; i++)
    {
        if (Markers[i]->GetIdentifier() == Identifier)
            return Markers[i];
    }
    return NULL;
}

void C3DMarkersSA::ReinitMarkers()
{
    using Function_ShutdownMarkers = void(__cdecl*)();
    auto shutdownMarkers = reinterpret_cast<Function_ShutdownMarkers>(0x722710);

    using Function_InitMarkers = void(__cdecl*)();
    auto initMarkers = reinterpret_cast<Function_InitMarkers>(0x724E40);

    shutdownMarkers();
    initMarkers();
}
