/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLArrayImpl.h
*  PURPOSE:     XML array class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLARRAY_H
#define __CXMLARRAY_H

class CXMLCommon;

class CXMLArray
{
public:
    static void                 Initialize          ( void );

    static unsigned long        PopUniqueID         ( CXMLCommon* pEntry );
    static void                 PushUniqueID        ( CXMLCommon* pEntry );

    static CXMLCommon*          GetEntry            ( unsigned long ulID );

private:
    static void                 PushUniqueID        ( unsigned long ulID );

    static CStack < unsigned long, MAX_XML, INVALID_XML_ID > m_IDStack;
    static CXMLCommon*          m_Elements [MAX_XML];
};


#endif
