/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C3DMarkersSA.cpp
 *  PURPOSE:     3D Marker entity manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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

C3DMarker* C3DMarkersSA::CreateMarker(DWORD Identifier, e3DMarkerType dwType, CVector* vecPosition, float fSize, float fPulseFraction, BYTE r, BYTE g, BYTE b,
                                      BYTE a)
{
    /*
    static C3dMarker *PlaceMarker(unsigned int nIdentifier, unsigned short nType,
    CVector &vecPosition, float fSize, unsigned char r, unsigned char g, unsigned char b, unsigned char a,
    unsigned short nPeriod, float fPulseFrac, short nRotRate, float normalX = 0.0f,
    float normalY = 0.0f, float normalZ = 0.0f, bool zCheck = FALSE);
    */
    WORD wType = dwType;
    dwType = (e3DMarkerType)wType;
    bool bZCheck = true;

    DWORD dwFunc = FUNC_PlaceMarker;
    DWORD dwReturn = 0;
    _asm
    {
        push    bZCheck     // zCheck  ##SA##
        push    0           // normalZ ##SA##
        push    0           // normalY ##SA##
        push    0           // normalX ##SA##
        push    0           // rotate rate
        push    fPulseFraction      // pulse
        push    0           // period
        push    a           // alpha
        push    b           // blue
        push    g           // green
        push    r           // red
        push    fSize       // size
        push    vecPosition // position
        push    dwType      // type
        push    Identifier  // identifier
        call    dwFunc
        mov     dwReturn, eax
        add     esp, 0x3C
    }

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
