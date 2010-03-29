/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CSetCursorPosHookInterface.h
*  PURPOSE:     Cursor hooking interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSETCURSORPOSHOOKINTERFACE_H
#define __CSETCURSORPOSHOOKINTERFACE_H

class CSetCursorPosHookInterface
{
public:

    virtual void            ApplyHook               ( void ) = 0;
    virtual void            RemoveHook              ( void ) = 0;

    virtual void            DisableSetCursorPos     ( void ) = 0;
    virtual void            EnableSetCursorPos      ( void ) = 0;
    virtual bool            IsSetCursorPosEnabled   ( void ) = 0;

    virtual BOOL            CallSetCursorPos        ( int X, int Y ) = 0;

};

#endif
