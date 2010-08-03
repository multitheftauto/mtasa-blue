/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CXfireQuery.h
*  PURPOSE:     Header file for the XFireQuery Server Information Class
*  DEVELOPERS:  Cazomino05 <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CCore;

#ifndef __CXFIREQUERY_H
#define __CXFIREQUERY_H

#define XFIRE_UPDATE_RATE 100 // Our XFire Polling rate (Please remember this is how fast the server is queried and the broadcast rate for XFire is seperate.)

class CXfireServerInfo : public CServerListItem
{
public:
    CXfireServerInfo ()
    {
        m_Socket = NULL;
    }
    //For XFire so we don't have an un-used socket laying around and so queries don't fail the second time they are ran
    void SocketClose ( void )
    {
        closesocket ( m_Socket );
        m_Socket = NULL;
    }
    //For XFire to check if our socket is closed
    bool IsSocketClosed ( void )
    {
        return m_Socket == NULL;
    }
    static bool ReadString              ( std::string &strRead, const char * szBuffer, unsigned int &i, unsigned int nLength );
    bool ParseQuery              ( const char * szBuffer, unsigned int nLength );
    void Query                   ( void );
    std::string Pulse            ( void );
};
#endif