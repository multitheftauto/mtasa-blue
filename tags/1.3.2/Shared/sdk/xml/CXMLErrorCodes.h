/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/xml/CXMLErrorCodes.h
*  PURPOSE:     XML error code enumerations
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLERRORCODES_H
#define __CXMLERRORCODES_H

class CXMLErrorCodes
{
public:
    enum Code
    {
        NoError                            = 0,
        NoFileSpecified                    = 1,
        OtherError                         = 2
    };
};

#endif
