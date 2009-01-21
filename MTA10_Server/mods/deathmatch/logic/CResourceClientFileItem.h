/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceClientFileItem.h
*  PURPOSE:     Resource client-side file item class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCECLIENTFILEITEM_H
#define CRESOURCECLIENTFILEITEM_H

#include "CResourceFile.h"

class CResourceClientFileItem : public CResourceFile
{
    
public:

                                        CResourceClientFileItem         ( class CResource * resource, const char * szShortName, const char * szResourceFileName, bool bValidateContent );
                                        ~CResourceClientFileItem        ( void );

    bool                                Start                           ( void );
    bool                                Stop                            ( void );
    
private:

};

#endif
