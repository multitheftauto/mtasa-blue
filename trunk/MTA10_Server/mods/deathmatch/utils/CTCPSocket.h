/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CTCPSocket.h
*  PURPOSE:     TCP socket base interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTCPSOCKET_H
#define __CTCPSOCKET_H

class CTCPSocket
{
public:
    virtual             ~CTCPSocket         ( void ) {};

    virtual int         ReadBuffer          ( void* pOutput, int iSize ) = 0;
    virtual int         WriteBuffer         ( const void* pInput, int iSize ) = 0;
};

#endif
