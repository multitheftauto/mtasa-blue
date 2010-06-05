/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CCallback.h
*  PURPOSE:     C++ Callback templates
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CCALLCALLBACK_H
#define __CCALLCALLBACK_H

// Represents any callback
template < typename Ret, typename Arguments >
class CCallbackInterface
{
public:
    virtual         ~CCallbackInterface         ( void ) {};

    virtual Ret     operator ()                 ( Arguments ) const = 0;
};


// Callback to a static function
template < typename Ret, typename Arguments >
class CCallbackFree : public CCallbackInterface < Ret, Arguments > 
{
    typedef Ret ( *CallbackFunction )( Arguments );

public:
    CCallbackFree ( CallbackFunction f ) : m_fnCallback (f) {}

    virtual Ret operator() ( Arguments Args ) const
    {
        return m_fnCallback ( Args );
    }

protected:
    CallbackFunction    m_fnCallback;
};


// Callback to a method
template < class T, typename Ret, typename Arguments >
class CCallbackMethod : public CCallbackInterface < Ret, Arguments >
{
    typedef Ret ( T::*F )( Arguments );

public:
    CCallbackMethod ( F pfnMethod, T* pClass ) : m_pfnMethod ( pfnMethod ), m_pClass ( pClass ) {};

    virtual Ret operator () ( Arguments Args ) const
    {
        return ( m_pClass->*m_pfnMethod )( Args );
    }

protected:
    F   m_pfnMethod;
    T*  m_pClass;
};


// Class representing any callback
template < typename Ret, typename Arguments >
class CCallback
{
    typedef Ret ( *CallbackFunction )( Arguments );

public:
    // Construct from a static function pointer
    CCallback ( CallbackFunction pfnCallback )
    {
        m_pCallback = new CCallbackFree < Ret, Arguments > ( pfnCallback );
    }

    // Construct from a method with a 'this' pointer
    template < class T >
    CCallback ( Ret ( T::*f )( Arguments ), T* pClass )
    {
        m_pCallback = new CCallbackMethod < T, Ret, Arguments > ( f, pClass );
    }

    // Copy constructor
    CCallback ( const CCallback < Ret, Arguments > & copy ) : m_pCallback ( copy.m_pCallback ) {}

    // Destructor
    ~CCallback ( void )
    {
        delete m_pCallback;
    }

    // Call operator
    Ret operator () ( Arguments Args ) const
    {
        return ( *m_pCallback )( Args );
    }

protected:
    CCallbackInterface < Ret, Arguments >*      m_pCallback;
};

#endif

