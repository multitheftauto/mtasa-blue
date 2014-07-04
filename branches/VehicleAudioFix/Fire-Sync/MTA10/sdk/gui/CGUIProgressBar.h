/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUIProgressBar.h
*  PURPOSE:     Progress bar widget interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIPROGRESSBAR_H
#define __CGUIPROGRESSBAR_H

#include "CGUIElement.h"
#include <string>

class CGUIProgressBar : public CGUIElement
{
public:
    virtual                         ~CGUIProgressBar           ( void ) {};

    virtual void                    SetProgress                 ( float fProgress ) = 0;
    virtual float                   GetProgress                 ( void ) = 0;
};

#endif
