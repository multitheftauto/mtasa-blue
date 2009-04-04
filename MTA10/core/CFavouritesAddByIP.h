/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFavouritesAddByIP.h
*  PURPOSE:     Header file for add to favourites by IP dialog
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CFavouritesAddByIP;

#ifndef __CFAVOURITESADDBYIP_H
#define __CFAVOURITESADDBYIP_H

#include "CMainMenu.h"

class CFavouritesAddByIP
{
public:
                            CFavouritesAddByIP  ( void );
    virtual                ~CFavouritesAddByIP  ();

    void                    SetVisible          ( bool bVisible );
    bool                    IsVisible           ( void );

    void                    GetHost             ( std::string & strHost );
    unsigned short          GetPort             ( void );

    void                    Reset               ( void );

    void                    SetCallback         ( const GUI_CALLBACK & Callback );

private:
    bool                    OnButtonBackClick   ( CGUIElement* pElement );

    CGUIWindow*             m_pWindow;
    CGUIEdit*               m_pEditHost;
    CGUIEdit*               m_pEditPort;
    CGUILabel*              m_pLabelHost;
    CGUILabel*              m_pLabelPort;
    CGUIButton*             m_pButtonAdd;
    CGUIButton*             m_pButtonBack;
};

#endif