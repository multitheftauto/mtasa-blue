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

typedef IDirect3D9 * ( __stdcall * pDirect3DCreate )    ( int SDKVersion );

class CDirect3DHook9 : public CSingleton < CDirect3DHook9 >
{
public: 

                                    CDirect3DHook9      ( );
                                   ~CDirect3DHook9      ( );
    // Hook routines.
    static IDirect3D9* __stdcall   API_Direct3DCreate9 ( UINT SDKVersion );

    // Hook utilities.
            bool                    ApplyHook           ( );
            bool                    RemoveHook          ( );


    pDirect3DCreate     m_pfnDirect3DCreate9;
};

#endif
