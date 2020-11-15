/***********************************************************************
	created:  16/3/2005
	author:   Tomas Lindquist Olsen

	purpose:  Header that includes the necessary stuff needed for the Lua bindings
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/CEGUI.h"
#include "CEGUI/ScriptModules/Lua/ScriptModule.h"
#include "CEGUI/ScriptModules/Lua/Functor.h"
#include "CEGUI/RenderingContext.h"
#include "CEGUI/RenderingWindow.h"
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/Vertex.h"
#include "CEGUI/RenderEffect.h"
#include "CEGUI/PropertyHelper.h"
#include <fstream>

#define __operator_increment    operator++
#define __operator_decrement    operator--
#define __operator_dereference  operator*

#define LuaFunctorSubscribeEvent CEGUI::LuaFunctor::SubscribeEvent

//This is used to keep compilers happy
#define CEGUIDeadException(e) &

#if defined(_MSC_VER) && !defined(snprintf)
#   define snprintf   _snprintf
#endif

// map the utf8string funcs to c-string funcs
#define tolua_pushutf8string(x,y) tolua_pushstring(x,y)
#define tolua_isutf8string tolua_isstring
#define tolua_isutf8stringarray tolua_isstringarray
#define tolua_pushfieldutf8string(L,lo,idx,s) tolua_pushfieldstring(L, lo, idx, s)
#define tolua_toutf8string tolua_tostring
#define tolua_tofieldutf8string tolua_tofieldstring

typedef CEGUI::String string;
typedef CEGUI::encoded_char* utf8string;

namespace CEGUI
{

/*************************************************************************
    Helper class to enable us to represent an Event::Connection object
    (which is actually a RefCounted<BoundSlot> object)
*************************************************************************/
class EventConnection
{
    RefCounted<BoundSlot> d_slot;

public:
    EventConnection(RefCounted<BoundSlot> slot);
    bool connected() const;
    void disconnect();
};

/*************************************************************************
	Functions for getting Thumb range pairs as two return values
*************************************************************************/
void ceguiLua_Thumb_getHorzRange(Thumb* wnd, float* min, float* max);
void ceguiLua_Thumb_getVertRange(Thumb* wnd, float* min, float* max);


/*************************************************************************
	Functions for creating list box items
*************************************************************************/
ListboxTextItem* ceguiLua_createListboxTextItem(const String& text, uint item_id, void* item_data, bool disabled, bool auto_delete);

/*************************************************************************
Functions for creating tree items
*************************************************************************/
TreeItem* ceguiLua_createTreeItem(const String& text, uint item_id, void* item_data, bool disabled, bool auto_delete);

/************************************************************************
    Stuff needed to make the iterators work
*************************************************************************/
typedef PropertySet::PropertyIterator PropertyIterator;
typedef EventSet::EventIterator EventIterator;
typedef WindowManager::WindowIterator WindowIterator;
typedef WindowFactoryManager::WindowFactoryIterator WindowFactoryIterator;
typedef WindowFactoryManager::FalagardMappingIterator FalagardMappingIterator;
typedef WindowFactoryManager::FalagardWindowMapping FalagardWindowMapping;
typedef SchemeManager::SchemeIterator SchemeIterator;
typedef FontManager::FontIterator FontIterator;
typedef ImageManager::ImageIterator ImageIterator;
typedef ImageManager::ImagePair ImagePair;

template <typename T>
inline PropertyIterator ceguiLua_getPropertyIterator(const T* self)
{
    return static_cast<const PropertySet*>(self)->getPropertyIterator();
}

template <typename T>
inline EventIterator ceguiLua_getEventIterator(const T* self)
{
    return static_cast<const EventSet*>(self)->getEventIterator();
}


/************************************************************************
    OutStream
*************************************************************************/
typedef std::ofstream FileStream;
void ceguiLua_FileStream_open(FileStream*, const char* filename);


/************************************************************************
    wrapper class for PropertyHelper to de-templatise it.
*************************************************************************/
class ceguiLua_PropertyHelper
{
public:
    static float stringToFloat(const String& str);
    static unsigned int stringToUint(const String& str);
    static bool stringToBool(const String& str);
    static Sizef stringToSize(const String& str);
    static Vector2f stringToVector2(const String& str);
    static Rectf stringToRect(const String& str);
    static const Image* stringToImage(const String& str);
    static Colour stringToColour(const String& str);
    static ColourRect stringToColourRect(const String& str);
    static UDim stringToUDim(const String& str);
    static UVector2 stringToUVector2(const String& str);
    static USize stringToUSize(const String& str);
    static URect stringToURect(const String& str);
    static AspectMode stringToAspectMode(const String& str);

    static String floatToString(float val);
    static String uintToString(unsigned int val);
    static String boolToString(bool val);
    static String sizeToString(const Sizef& val);
    static String vector2ToString(const Vector2f& val);
    static String rectToString(const Rectf& val);
    static String imageToString(const Image* val);
    static String colourToString(const Colour& val);
    static String colourRectToString(const ColourRect& val);
    static String udimToString(const UDim& val);
    static String uvector2ToString(const UVector2& val);
    static String usizeToString(const USize& val);
    static String urectToString(const URect& val);
    static String aspectModeToString(AspectMode val);
};

}

// the binding file generates alot of
// warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#if defined(_MSC_VER)
#   pragma warning(disable : 4800)
#endif

