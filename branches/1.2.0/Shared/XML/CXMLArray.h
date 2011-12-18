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
    static unsigned long        GetCapacity         ( void );
    static unsigned long        GetUnusedAmount     ( void );

private:
    static void                 ExpandBy            ( unsigned long ulAmount );
    static void                 PushUniqueID        ( unsigned long ulID );

    static CStack < unsigned long, 1 >  m_IDStack;
    static std::vector < CXMLCommon* >                  m_Elements;
    static unsigned long                                m_ulCapacity;
};


#endif
