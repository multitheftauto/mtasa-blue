/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceScriptItem.h
*  PURPOSE:     Resource server-side script item class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCESCRIPTITEM_H
#define CRESOURCESCRIPTITEM_H

#include "CResourceFile.h"

class CResourceScriptItem : public CResourceFile
{
    
public:

                                        CResourceScriptItem             ( class CResource * resource, const char * szShortName, const char * szResourceFileName, CXMLAttributes * xmlAttributes );
                                        ~CResourceScriptItem            ( void );

    bool                                Start                           ( void );
    bool                                Stop                            ( void );
    
private:

};

#endif

