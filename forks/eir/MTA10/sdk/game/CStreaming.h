/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CStreaming.h
*  PURPOSE:     Game streaming interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CStreaming_H
#define __CStreaming_H

// Set the default model id type
// This should be the most efficient way to travel model ids.
// It can either be unsigned int or unsigned short.
// To provide compatibility with GTA:SA, we use unsigned int.
// Some say (http://stackoverflow.com/questions/5069489/performance-of-built-in-types-char-vs-short-vs-int-vs-float-vs-double)
// int were faster than short. Okay.
// The rule of thumb is: use "unsigned short" for storing model ids (cache efficiency), use "modelId_t" for model ids on stack.
typedef unsigned int modelId_t;

typedef void (__cdecl*streamingRequestCallback_t)( modelId_t id );
typedef void (__cdecl*streamingLoadCallback_t)( modelId_t id );
typedef void (__cdecl*streamingFreeCallback_t)( modelId_t id );

class CStreaming
{
public:
    virtual void            RequestModel                ( modelId_t id, unsigned int flags ) = 0;
    virtual void            FreeModel                   ( modelId_t id ) = 0;
    virtual void            LoadAllRequestedModels      ( bool onlyPriority = false, const char *debugLoc = NULL ) = 0;
    virtual bool            HasModelLoaded              ( modelId_t id ) = 0;
    virtual bool            IsModelLoading              ( modelId_t id ) = 0;
    virtual void            WaitForModel                ( modelId_t id ) = 0;
    virtual void            RequestAnimations           ( int idx, unsigned int flags ) = 0;
    virtual bool            HaveAnimationsLoaded        ( int idx ) = 0;
    virtual bool            HasVehicleUpgradeLoaded     ( int model ) = 0;
    virtual void            RequestSpecialModel         ( modelId_t model, const char *tex, unsigned int channel ) = 0;

    virtual void            SetRequestCallback          ( streamingRequestCallback_t callback ) = 0;
    virtual void            SetLoadCallback             ( streamingLoadCallback_t callback ) = 0;
    virtual void            SetFreeCallback             ( streamingFreeCallback_t callback ) = 0;
};


#endif //__CStreaming_H
