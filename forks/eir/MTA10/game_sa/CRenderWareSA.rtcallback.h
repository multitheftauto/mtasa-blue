/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rtcallback.h
*  PURPOSE:     Conglomeration of all known GTA:SA render callbacks
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _ENGINE_RENDER_CALLBACKS_
#define _ENGINE_RENDER_CALLBACKS_

namespace RenderCallbacks
{
    void        SetEnvMapRenderingEnabled( bool enabled );
    bool        IsEnvMapRenderingEnabled( void );

    // Basic rendering API.
    void        SetRenderingEnabled( bool enabled );
    bool        IsRenderingEnabled( void );

    // Rendering mode API.
    void        SetAlphaSortingEnabled( bool enabled );
    bool        IsAlphaSortingEnabled( void );

    void        SetAlphaSortingParams( bool doOpaque, bool doTranslucent, bool doDepth );
    void        GetAlphaSortingParams( bool& doOpaque, bool& doTranslucent, bool& doDepth );

    void        SetVehicleAlphaSortingEnabled( bool enabled );
    bool        IsVehicleAlphaSortingEnabled( void );

    void        SetVehicleAlphaSortingParams( bool doOpaque, bool doTranslucent, bool doDepth );
    void        GetVehicleAlphaSortingParams( bool& doOpaque, bool& doTranslucent, bool& doDepth );
    void        SetVehicleAlphaClamp( unsigned int clamp );
};

// Module initialization routines.
void RenderCallbacks_Init( void );
void RenderCallbacks_Shutdown( void );

void RenderCallbacks_Reset( void );

#endif //_ENGINE_RENDER_CALLBACKS_