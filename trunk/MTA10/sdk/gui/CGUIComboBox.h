/*********************************************************
*
*  Multi Theft Auto :: Codename "Blue" (version 1.1)
*  Copyright © 2003-2006 MTA.  All Rights Reserved.
*
*  Grand Theft Auto is © 1997-2005 Rockstar North
*
*  THE FOLLOWING SOURCES ARE CONSIDIERED HIGHLY CONFIDENTIAL
*  AND ARE TO BE VIEWED ONLY BY MEMBERS OF THE MULTI THEFT
*  AUTO STAFF.  ANY VIOLATION OF THESE TERMS CAN LEAD TO
*  CRIMINAL PROSECUTION AND/OR LARGE FINES UNDER UNITED
*  STATES LAW.
*
**********************************************************/

#ifndef __CGUICOMBOBOX_H
#define __CGUICOMBOBOX_H

#include "CGUIElement.h"
#include "CGUICallback.h"

class CGUIComboBox : public CGUIElement
{
public:
    virtual                     ~CGUIComboBox       ( void ) {};

    virtual void                SetReadOnly         ( bool bRead ) = 0;
    virtual CGUIListItem*       AddItem             ( const char* szText ) = 0;
    virtual CGUIListItem*       GetSelectedItem     ( void ) = 0;
    virtual void                Clear               ( void ) = 0;
    virtual void                Click               ( void ) = 0;

    virtual void                SetOnClickHandler   ( const GUI_CALLBACK & Callback ) = 0;
};

#endif
