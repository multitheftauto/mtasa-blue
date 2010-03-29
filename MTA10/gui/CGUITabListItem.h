/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUITabListItem.h
*  PURPOSE:     Tab-able Element Interface
*  DEVELOPERS:  Marcus Bauer <mabako@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUITABLISTITEM_H
#define __CGUITABLISTITEM_H

class CGUITabListItem
{
public:
    virtual bool ActivateOnTab ( void ) = 0;
};

#endif
