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

                                CResourceClientScriptItem       ( class CResource * resource, const char * szShortName, const char * szResourceFileName, CXMLAttributes * xmlAttributes );
                                ~CResourceClientScriptItem      ( void );

    bool                        Start                           ( void );
    bool                        Stop                            ( void );

    bool                        IsNoClientCache                 () const { return m_bIsNoClientCache; }
    const SString&              GetSourceCode                   () const { return m_sourceCode; }

    ResponseCode                Request                         ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse );
    
private:
    bool                        m_bIsNoClientCache;
    SString                     m_sourceCode;
};

#endif
