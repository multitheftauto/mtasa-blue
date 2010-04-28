/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/CSingleton.h
*  PURPOSE:     Base singleton template class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*  THIS FILE IS PROPERTY OF
*  Copyright (C) Scott Bilas, 2000. 
*  All rights reserved worldwide.
*
*  This software is provided "as is" without express or
*  implied warranties. You may freely copy and compile
*  this source into applications you distribute provided
*  that the copyright text below is included in the
*  resulting source code, for example:
*  "Portions Copyright (C) Scott Bilas, 2000"
*
*****************************************************************************/

#ifndef __CSINGLETON_H
#define __CSINGLETON_H

#include <cassert>

template <typename T> class CSingleton
{
protected:
    static T* m_pSingleton;

public:
    CSingleton( void )
    {
        assert( !m_pSingleton );
        m_pSingleton = static_cast< T* > ( this );
    }

   ~CSingleton( void )    
    {
        m_pSingleton = 0;
    }       
        
    static T& GetSingleton( void )
    {
        assert ( m_pSingleton );
        return ( *m_pSingleton );
    }

    static T* GetSingletonPtr( void )
    {
        return m_pSingleton;
    }
};

#endif