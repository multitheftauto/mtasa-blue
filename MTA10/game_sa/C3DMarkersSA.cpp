/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/C3DMarkersSA.cpp
*  PURPOSE:     3D Marker entity manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

C3DMarkersSA::C3DMarkersSA()
{
    DEBUG_TRACE("C3DMarkersSA::C3DMarkersSA()");
    for(int i = 0;i< MAX_3D_MARKERS;i++)
    {
        this->Markers[i] = new C3DMarkerSA((C3DMarkerSAInterface *)(ARRAY_3D_MARKERS + i * sizeof(C3DMarkerSAInterface)));
    }
}


C3DMarkersSA::~C3DMarkersSA ()
{
    for ( int i = 0; i < MAX_3D_MARKERS; i++ )
    {
        delete Markers [i];
    }
}


C3DMarker * C3DMarkersSA::CreateMarker(DWORD Identifier, e3DMarkerType dwType, CVector * vecPosition, FLOAT fSize, FLOAT fPulseFraction, BYTE r, BYTE g, BYTE b, BYTE a )
{
    DEBUG_TRACE("C3DMarkersSA::CreateMarker(DWORD Identifier, e3DMarkerType dwType, CVector * vecPosition, FLOAT fSize, FLOAT fPulseFraction)");
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
/*
    DWORD dwFunc = 0x0726D40;
    DWORD dwReturn = 0;
    _asm
    {
        push    0           // uses collision
        push    5           // rotate rate
        push    0x3F800000  // pulse (1.0)
        push    1024        // period
        push    255         // alpha
        push    0           // blue
        push    255         // green
        push    255         // red
        push    0x40000000      // size (2.0)
        push    vecPosition // position
        push    Identifier  // identifier
        call    dwFunc
        mov     dwReturn, eax
        add     esp, 0x2C
    }
    */
    if(dwReturn)
    {
        for(int i = 0; i < MAX_3D_MARKERS; i++)
        {
            if(Markers[i]->GetInterface() == (C3DMarkerSAInterface *)dwReturn)
            {
                //Markers[i]->Reset(); // debug stuff
                return Markers[i];
            }
        }
    }

    return NULL;
}

C3DMarker * C3DMarkersSA::FindFreeMarker()
{
    DEBUG_TRACE("C3DMarker * C3DMarkersSA::FindFreeMarker()");
    for(int i = 0; i<MAX_3D_MARKERS;i++)
    {
        if(!Markers[i]->IsActive()) 
            return Markers[i];
    }
    return NULL;
}

C3DMarker * C3DMarkersSA::FindMarker( DWORD Identifier )
{
    DEBUG_TRACE("C3DMarker * C3DMarkersSA::FindMarker( DWORD Identifier )");
    for(int i = 0; i<MAX_3D_MARKERS;i++)
    {
        if(Markers[i]->GetIdentifier() == Identifier) 
            return Markers[i];
    }
    return NULL;
}
