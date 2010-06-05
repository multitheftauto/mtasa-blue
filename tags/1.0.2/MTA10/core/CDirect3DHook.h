/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirect3DHook.h
*  PURPOSE:     Header file for global Direct3D hooking declarations
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDIRECT3DHOOK_H
#define __CDIRECT3DHOOK_H

#include <windows.h>

typedef IUnknown * ( __stdcall * pDirect3DCreate ) ( int SDKVersion );

class CDirect3DHook 
{
    public:

    // Hook utilities.
    virtual bool        ApplyHook           ( ) =0;
    virtual bool        RemoveHook          ( ) =0;

    IUnknown *  m_pDevice;
};

#endif
