/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CBassManagerInterface.h
*  PURPOSE:     core based manager class header - interface to other modules
*  DEVELOPERS:  Cazomino05 <>
*
*****************************************************************************/

#ifndef __CBASSMANAGERINTERFACE_H
#define __CBASSMANAGERINTERFACE_H

class CBassManagerInterface
{
public:
    virtual void    SetUserAgent                ( SString strConnectedServer ) = 0;
};

#endif