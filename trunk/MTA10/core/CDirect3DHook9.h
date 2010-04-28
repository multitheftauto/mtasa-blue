/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirect3DHook9.h
*  PURPOSE:     Header file for Direct3D 8 hooker class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDIRECT3DHOOK9_H
#define __CDIRECT3DHOOK9_H

#include "CDirectXHook.h"
#include "CSingleton.h"

class CDirect3DHook9 : public CDirectXHook, public CSingleton < CDirect3DHook9 >
{
public: 

                                    CDirect3DHook9      ( );
                                   ~CDirect3DHook9      ( );
    // Hook routines.
    static  IUnknown *  __stdcall   API_Direct3DCreate9 ( UINT SDKVersion );

    // Hook utilities.
            bool                    ApplyHook           ( );
            bool                    RemoveHook          ( );

            inline void             SetHookingEnabled   ( bool bEnabled ) { m_bHookingEnabled = bEnabled; }

    pDirect3DCreate     m_pfnDirect3DCreate9;

private:
    bool                m_bHookingEnabled;
};

#endif
