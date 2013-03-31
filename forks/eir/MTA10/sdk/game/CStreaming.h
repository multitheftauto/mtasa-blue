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

typedef void (__cdecl*streamingRequestCallback_t)( unsigned short id );
typedef void (__cdecl*streamingLoadCallback_t)( unsigned short id );
typedef void (__cdecl*streamingFreeCallback_t)( unsigned short id );

class CStreaming
{
public:
    virtual void            RequestModel                ( unsigned short id, unsigned int flags ) = 0;
    virtual void            FreeModel                   ( unsigned short id ) = 0;
    virtual void            LoadAllRequestedModels      ( bool onlyPriority = false ) = 0;
    virtual bool            HasModelLoaded              ( unsigned int id ) = 0;
    virtual bool            IsModelLoading              ( unsigned int id ) = 0;
    virtual void            WaitForModel                ( unsigned int id ) = 0;
    virtual void            RequestAnimations           ( int idx, unsigned int flags ) = 0;
    virtual bool            HaveAnimationsLoaded        ( int idx ) = 0;
    virtual bool            HasVehicleUpgradeLoaded     ( int model ) = 0;
    virtual void            RequestSpecialModel         ( unsigned short model, const char *tex, unsigned int channel ) = 0;

    virtual void            SetRequestCallback          ( streamingRequestCallback_t callback ) = 0;
    virtual void            SetLoadCallback             ( streamingLoadCallback_t callback ) = 0;
    virtual void            SetFreeCallback             ( streamingFreeCallback_t callback ) = 0;
};

#endif //__CStreaming_H
