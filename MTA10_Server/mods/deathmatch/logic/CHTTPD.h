/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CHTTPD.h
*  PURPOSE:     Built-in HTTP webserver class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class implements the webserver, it uses EHS to do this

#ifndef CHTTPD_H
#define CHTTPD_H

#include <string>
#include <list>

#include "ehs/ehs.h"

class CHTTPD : public EHS 
{
public:

	                            CHTTPD      ( void ); // start the initial server
                                ~CHTTPD     ();

	ResponseCode HandleRequest ( HttpRequest * ipoHttpRequest,
								 HttpResponse * ipoHttpResponse );
	
    bool                        StartHTTPD ( const char* szIP, unsigned int port );
    inline void                 SetResource ( CResource * resource ) { m_resource = resource; }
    inline CResource *          GetResource ( void ) { return m_resource; }
    class CAccount *            CheckAuthentication ( HttpRequest * ipoHttpRequest );
    inline void                 SetDefaultResource ( const char * szResourceName ) { m_strDefaultResourceName = szResourceName ? szResourceName : ""; }
    ResponseCode                RequestLogin ( HttpResponse * ipoHttpResponse );
private:
    CResource *                 m_resource;
    CHTTPD *                    m_server;
    std::string                 m_strDefaultResourceName; // default resource name
    
	EHSServerParameters			m_Parameters;

    bool                        m_bStartedServer;

    class CAccount *            m_pGuestAccount;
};

#endif
