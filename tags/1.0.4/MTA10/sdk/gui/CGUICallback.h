/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUICallback.h
*  PURPOSE:     Event callback interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUICALLBACK_H
#define __CGUICALLBACK_H

class CGUIElement;

// Define callbacks
#define GUI_CALLBACK            CGUICallback < bool, CGUIElement * >
#define GUI_CALLBACK_KEY        CGUICallback < bool, CGUIKeyEventArgs >
#define GUI_CALLBACK_MOUSE      CGUICallback < bool, CGUIMouseEventArgs >

// Represents any callback
template < typename Ret, typename Arguments >
class CGUICallbackInterface
{
public:
    virtual Ret     operator ()                 ( Arguments ) const = 0;
};


// Callback to a static function
template < typename Ret, typename Arguments >
class CGUICallbackFree : public CGUICallbackInterface < Ret, Arguments > 
{
    typedef Ret ( *F )( Arguments );

public:
    CGUICallbackFree ( F f ) : m_pfnCallback ( f ) {}

    virtual Ret operator() ( Arguments Args ) const
    {
        return m_pfnCallback ( Args );
    }

protected:
    F    m_pfnCallback;
};


// Callback to a method
template < class T, typename Ret, typename Arguments >
class CGUICallbackMethod : public CGUICallbackInterface < Ret, Arguments >
{
    typedef Ret ( T::*F )( Arguments );

public:
    CGUICallbackMethod ( F pfnMethod, T* pObj ) : m_pfnMethod ( pfnMethod ), m_pObj ( pObj ) {}

    virtual Ret operator () ( Arguments Args ) const
    {
        return ( m_pObj->*m_pfnMethod )( Args );
    }

protected:
    F   m_pfnMethod;
    T*  m_pObj;
};


// Class representing any callback
template < typename Ret, typename Arguments >
class CGUICallback
{
public:
    // Default constructor
    CGUICallback ( void )
    {
        memset ( m_Callback, 0, sizeof(m_Callback) );
    }

    // Construct from a static function pointer
    CGUICallback ( Ret ( *pF )( Arguments ) )
    {
        if ( pF )
        {
            new(m_Callback) CGUICallbackFree < Ret, Arguments > ( pF );
        }
    }

    // Construct from a method with a 'this' pointer
    template < class T >
    CGUICallback ( Ret ( T::*pF )( Arguments ), T* pObj )
    {
        if ( pF )
        {
            new(m_Callback) CGUICallbackMethod < T, Ret, Arguments > ( pF, pObj );
        }
    }

    // Copy constructor
    CGUICallback ( const CGUICallback < Ret, Arguments > & copy )
    {
        *this = copy;
    }

    void operator = ( const CGUICallback < Ret, Arguments > & copy )
    {
        memcpy ( m_Callback, copy.m_Callback, sizeof(m_Callback) );
    }

    // Call operator
    Ret operator () ( Arguments Args ) const
    {
        return ( *(CGUICallbackInterface < Ret, Arguments > *)m_Callback )( Args );
    }

    operator bool () const
    {
        return *(DWORD *)m_Callback != NULL;
    }

protected:
    char m_Callback [ 4 + 4 + 20 ];       // vtable + pObj + pMemFn
};

#endif

