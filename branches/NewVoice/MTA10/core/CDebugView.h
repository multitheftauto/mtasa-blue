/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDebugView.h
*  PURPOSE:     Header file for debug view class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDEBUGVIEW_H
#define __CDEBUGVIEW_H

#include "CGUI.h"
#include <gui/CGUI.h>
#include "CChat.h"

#define DEBUGVIEW_WIDTH         576                             // Debugview default width
#define DEBUGVIEW_TEXT_COLOR    CColor( 235, 221, 178, 255 )    // Debugview default text color

class CDebugView : public CChat
{
public:
                CDebugView              ( CGUI* pManager, const CVector2D & vecPosition );

    void        Draw                    ( void );
    void        Output                  ( char* szText, bool bColorCoded );
    void        Outputf                 ( bool bColorCoded, char* szText, ... );
};

#endif
