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
#define GUI_CALLBACK			CGUICallback < bool, CGUIElement * >
#define GUI_CALLBACK_KEY		CGUICallback < bool, CGUIKeyEventArgs >
#define GUI_CALLBACK_MOUSE		CGUICallback < bool, CGUIMouseEventArgs >

// Represents any callback
template < typename Ret, typename Arguments >
class CGUICallbackInterface
{
public:
	virtual CGUICallbackInterface* Copy ( void ) = 0;
    virtual Ret     operator ()                 ( Arguments ) const = 0;
};


// Callback to a static function
template < typename Ret, typename Arguments >
class CGUICallbackFree : public CGUICallbackInterface < Ret, Arguments > 
{
    typedef Ret ( *CallbackFunction )( Arguments );

public:
    CGUICallbackFree ( CallbackFunction f ) : m_fnCallback (f) {}

    virtual Ret operator() ( Arguments Args ) const
    {
        return m_fnCallback ( Args );
    }

	virtual CGUICallbackInterface < Ret, Arguments > * Copy ( )
	{
		return new CGUICallbackFree ( m_fnCallback );
	}

protected:
    CallbackFunction    m_fnCallback;
};


// Callback to a method
template < class T, typename Ret, typename Arguments >
class CGUICallbackMethod : public CGUICallbackInterface < Ret, Arguments >
{
    typedef Ret ( T::*F )( Arguments );

public:
    CGUICallbackMethod ( F pfnMethod, T* pClass ) : m_pfnMethod ( pfnMethod ), m_pClass ( pClass ) {};

    virtual Ret operator () ( Arguments Args ) const
    {
        return ( m_pClass->*m_pfnMethod )( Args );
    }

	virtual CGUICallbackInterface<Ret,Arguments>* Copy ( void )
	{
		return new CGUICallbackMethod ( m_pfnMethod, m_pClass );
	}

protected:
    F   m_pfnMethod;
    T*  m_pClass;
};


// Class representing any callback
template < typename Ret, typename Arguments >
class CGUICallback
{
    typedef Ret ( *CallbackFunction )( Arguments );

public:
    // Default constructor
    CGUICallback ( void )
    {
        m_pCallback = NULL;
    }

    // Construct from a static function pointer
    CGUICallback ( CallbackFunction pfnCallback )
    {
		m_pCallback = pfnCallback ? new CGUICallbackFree < Ret, Arguments > ( pfnCallback ) : NULL;
    }

    // Construct from a method with a 'this' pointer
    template < class T >
    CGUICallback ( Ret ( T::*f )( Arguments ), T* pClass )
    {
        m_pCallback = new CGUICallbackMethod < T, Ret, Arguments > ( f, pClass );
    }

    // Copy constructor
	CGUICallback ( const CGUICallback < Ret, Arguments > & copy )
	{
        m_pCallback = NULL;
        *this = copy;
	}

    void operator = ( const CGUICallback < Ret, Arguments > & copy )
    {
        if ( &copy == this )
            return;

        if ( m_pCallback )
        {
            delete m_pCallback;
            m_pCallback = NULL;
        }

        if ( copy.m_pCallback )
        {
            m_pCallback = copy.m_pCallback->Copy();
        }
        else
        {
            m_pCallback = NULL;
        }
    }

    // Destructor
    ~CGUICallback ( void )
    {
        if ( m_pCallback )
        {
            delete m_pCallback;
        }
    }

    // Call operator
    Ret operator () ( Arguments Args ) const
    {
        if ( m_pCallback )
        {
            return ( *m_pCallback )( Args );
        }
		return false;
    }

    operator bool () const
    {
        return m_pCallback != NULL;
    }

protected:
    CGUICallbackInterface < Ret, Arguments >*      m_pCallback;
};

#endif

