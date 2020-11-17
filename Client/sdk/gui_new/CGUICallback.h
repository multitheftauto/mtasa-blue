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

#pragma once

class GUINew::CGUIElement;

namespace GUINew{
    template <typename Ret, typename Arguments>
    class CGUICallbackInterface;

    template <typename Ret, typename Arguments>
    class CGUICallbackFree;

    template <class T, typename Ret, typename Arguments>
    class CGUICallbackMethod;

    template <typename Ret, typename Arguments>
    class CGUICallback;
}

// Define callbacks
#define GUI_CALLBACK            GUINew::CGUICallback < bool, GUINew::CGUIElement * >
#define GUI_CALLBACK_KEY        GUINew::CGUICallback < bool, GUINew::CGUIKeyEventArgs >
#define GUI_CALLBACK_MOUSE      GUINew::CGUICallback < bool, GUINew::CGUIMouseEventArgs >
#define GUI_CALLBACK_FOCUS      GUINew::CGUICallback < bool, GUINew::CGUIFocusEventArgs >

// Represents any callback
template <typename Ret, typename Arguments>
class GUINew::CGUICallbackInterface
{
public:
    virtual Ret operator()(Arguments) const = 0;
};

// Callback to a static function
template <typename Ret, typename Arguments>
class GUINew::CGUICallbackFree : public CGUICallbackInterface<Ret, Arguments>
{
    typedef Ret (*F)(Arguments);

public:
    CGUICallbackFree(F f) : m_pfnCallback(f) {}

    virtual Ret operator()(Arguments Args) const { return m_pfnCallback(Args); }

protected:
    F m_pfnCallback;
};

// Callback to a method
template <class T, typename Ret, typename Arguments>
class GUINew::CGUICallbackMethod : public CGUICallbackInterface<Ret, Arguments>
{
    typedef Ret (T::*F)(Arguments);

public:
    CGUICallbackMethod(F pfnMethod, T* pObj) : m_pfnMethod(pfnMethod), m_pObj(pObj) {}

    virtual Ret operator()(Arguments Args) const { return (m_pObj->*m_pfnMethod)(Args); }

protected:
    F  m_pfnMethod;
    T* m_pObj;
};

// Class representing any callback
template <typename Ret, typename Arguments>
class GUINew::CGUICallback
{
public:
    // Default constructor
    CGUICallback() { memset(m_Callback, 0, sizeof(m_Callback)); }

    // Construct from a static function pointer
    CGUICallback(Ret (*pF)(Arguments))
    {
        memset(m_Callback, 0, sizeof(m_Callback));
        if (pF)
        {
            new (m_Callback) CGUICallbackFree<Ret, Arguments>(pF);
        }
    }

    // Construct from a method with a 'this' pointer
    template <class T>
    CGUICallback(Ret (T::*pF)(Arguments), T* pObj)
    {
        memset(m_Callback, 0, sizeof(m_Callback));
        if (pF)
        {
            new (m_Callback) CGUICallbackMethod<T, Ret, Arguments>(pF, pObj);
        }
    }

    // Copy constructor
    CGUICallback(const CGUICallback<Ret, Arguments>& copy) { *this = copy; }

    void operator=(const CGUICallback<Ret, Arguments>& copy) { memcpy(m_Callback, copy.m_Callback, sizeof(m_Callback)); }

    // Call operator
    Ret operator()(Arguments Args) const { return (*(CGUICallbackInterface<Ret, Arguments>*)m_Callback)(Args); }

    operator bool() const { return *(DWORD*)m_Callback != NULL; }

protected:
    char m_Callback[4 + 4 + 20];            // vtable + pObj + pMemFn
};
