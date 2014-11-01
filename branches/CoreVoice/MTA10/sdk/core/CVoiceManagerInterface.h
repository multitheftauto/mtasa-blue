/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVoiceManagerInterface.h
*  PURPOSE:     core based voice manager class header - interface to other modules
*  DEVELOPERS:  Cazomino05 <>
*
*****************************************************************************/

#ifndef __CVOICEMANAGERINTERFACE_H
#define __CVOICEMANAGERINTERFACE_H

#include "../../vendor/portaudio/portaudio.h"

class CVoiceManagerInterface
{
public:

    virtual void            RegisterCallback            ( PaStreamCallback * pCallback ) = 0;
    virtual void            Init                        ( void* user, unsigned int uiServerSampleRate, unsigned int uiBitrate ) = 0;

    virtual void            DeInit                      ( void ) = 0;

    virtual bool            IsInTestMode                ( void ) = 0;

    virtual void            Lock                        ( void ) = 0;
    virtual void            Unlock                      ( void ) = 0;
};

#endif