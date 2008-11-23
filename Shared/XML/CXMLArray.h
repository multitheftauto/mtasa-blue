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

#define MAX_XML 100000

class CXMLArray
{
public:
    static void                 Initialize          ( void );

    static unsigned long        PopUniqueID         ( CXMLCommon* pEntry );
    static void                 PushUniqueID        ( CXMLCommon* pEntry );

    static CXMLCommon*          GetEntry            ( unsigned long ulID );

private:
    static void                 PushUniqueID        ( unsigned long ulID );

    static unsigned long        PopStack            ( void );
    static void                 PushStack           ( unsigned long ulID );

    static unsigned long        m_ulIDStack [MAX_XML];
    static unsigned long        m_ulStackPosition;

    static CXMLCommon*          m_Elements [MAX_XML];
};


#endif
