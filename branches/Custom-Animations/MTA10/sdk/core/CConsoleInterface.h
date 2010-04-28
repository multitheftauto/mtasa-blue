/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CConsoleInterface.h
*  PURPOSE:     Console interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONSOLEINTERFACE_H
#define __CCONSOLEINTERFACE_H

class CConsoleInterface
{
public:
    virtual void        Echo            ( const char* szText ) = 0;
    virtual void        Print           ( const char* szText ) = 0;
    virtual void        Printf          ( const char* szFormat, ... ) = 0;

    virtual void        Clear           ( void ) = 0;

    virtual bool        IsEnabled       ( void ) = 0;
    virtual void        SetEnabled      ( bool bEnabled ) = 0;

    virtual bool        IsVisible       ( void ) = 0;
    virtual void        SetVisible      ( bool bVisible ) = 0;
    virtual void        Show            ( void ) = 0;
    virtual void        Hide            ( void ) = 0;
};

#endif
