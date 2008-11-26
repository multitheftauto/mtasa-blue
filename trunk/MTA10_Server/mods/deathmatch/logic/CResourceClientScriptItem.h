/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceClientScriptItem.h
*  PURPOSE:     Resource client-side script item class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCECLIENTSCRIPTITEM_H
#define CRESOURCECLIENTSCRIPTITEM_H

#include "CResourceFile.h"

class CResourceClientScriptItem : public CResourceFile
{
    
public:

                                        CResourceClientScriptItem       ( class CResource * resource, const char * szShortName, const char * szResourceFileName );
                                        ~CResourceClientScriptItem      ( void );

    bool                                Start                           ( void );
    bool                                Stop                            ( void );
    
private:

};

#endif
