/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CExceptionInformation.h
*  PURPOSE:     Exception information interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CEXCEPTIONINFORMATION_H
#define __CEXCEPTIONINFORMATION_H

class CExceptionInformation
{
public:
    virtual unsigned int        GetCode                 ( void ) = 0;
    virtual void*               GetOffset               ( void ) = 0;
    virtual void*               GetReferencingOffset    ( void ) = 0;
    virtual bool                GetModule               ( char * szOutputBuffer, int nOutputNameLength ) = 0;

    virtual unsigned long       GetEAX                  ( void ) = 0;
    virtual unsigned long       GetEBX                  ( void ) = 0;
    virtual unsigned long       GetECX                  ( void ) = 0;
    virtual unsigned long       GetEDX                  ( void ) = 0;
    virtual unsigned long       GetESI                  ( void ) = 0;
    virtual unsigned long       GetEDI                  ( void ) = 0;
    virtual unsigned long       GetEBP                  ( void ) = 0;
    virtual unsigned long       GetESP                  ( void ) = 0;
    virtual unsigned long       GetEIP                  ( void ) = 0;
    virtual unsigned long       GetCS                   ( void ) = 0;
    virtual unsigned long       GetDS                   ( void ) = 0;
    virtual unsigned long       GetES                   ( void ) = 0;
    virtual unsigned long       GetFS                   ( void ) = 0;
    virtual unsigned long       GetGS                   ( void ) = 0;
    virtual unsigned long       GetSS                   ( void ) = 0;
    virtual unsigned long       GetEFlags               ( void ) = 0;
};

#endif
