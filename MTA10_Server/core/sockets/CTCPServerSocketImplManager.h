/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/sockets/CTCPServerSocketImplManager.h
*  PURPOSE:     TCP server socket manager class
*  DEVELOPERS:  Oli <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCPSERVERSOCKETIMPLMANAGER_H
#define __CTCPSERVERSOCKETIMPLMANAGER_H

#include <list>

class CTCPServerSocketImplManager
{
public:
                                            CTCPServerSocketImplManager         ( void );
    virtual                                 ~CTCPServerSocketImplManager        ( void );

    void                                    AddServer                           ( class CTCPServerSocketImpl* pServer );
    void                                    RemoveServer                        ( class CTCPServerSocketImpl* pServer );

private:
    std::list < class CTCPServerSocketImpl* >    m_Servers;

};

#endif

