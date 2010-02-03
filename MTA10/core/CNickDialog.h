/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFavouritesAddByIP.h
*  PURPOSE:     Header file for add to favourites by IP dialog
*  DEVELOPERS:  Mr.Hankey <mrhankey88@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CNickDialog;

#ifndef __CNICKDIALOG_H
#define __CNICKDIALOG_H

#include "CMainMenu.h"

class CNickDialog
{
public:
                            CNickDialog(void);
    virtual                 ~CNickDialog(void);

    void                    SetVisible          ( bool bVisible );
    bool                    IsVisible           ( void );

    void                    Reset               ( void );

private:
    bool                    OnButtonOKClick   ( CGUIElement* pElement );
    bool                    OnButtonBackClick   ( CGUIElement* pElement );

    CGUIWindow*             m_pNickWindow;
    CGUIEdit*               m_pEditNick;
    CGUILabel*              m_pLabelNick;
    CGUIButton*             m_pButtonOK;
    CGUIButton*             m_pButtonBack;
};

#endif
