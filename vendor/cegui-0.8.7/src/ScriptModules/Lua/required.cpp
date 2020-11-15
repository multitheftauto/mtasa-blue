/***********************************************************************
	created:  16/3/2005
	author:   Tomas Lindquist Olsen

	purpose:  Implementation of helper functions
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
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
#include "required.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Functions for getting Thumb range pairs as two return values
*************************************************************************/

// returns horizontal range as two values for lua
void ceguiLua_Thumb_getHorzRange(Thumb* wnd, float* min, float* max)
{
	std::pair<float,float> range_pair = wnd->getHorzRange();
	*min = range_pair.first;
	*max = range_pair.second;
}


// returns vertical range as two values for lua
void ceguiLua_Thumb_getVertRange(Thumb* wnd, float* min, float* max)
{
	std::pair<float,float> range_pair = wnd->getVertRange();
	*min = range_pair.first;
	*max = range_pair.second;
}


/*************************************************************************
	Functions for creating list box items
*************************************************************************/

// allocates and returns a new ListboxTextItem
ListboxTextItem* ceguiLua_createListboxTextItem(const String& text, uint item_id, void* item_data, bool disabled, bool auto_delete)
{
	return new ListboxTextItem(text,item_id,item_data,disabled,auto_delete);
}

/*************************************************************************
Functions for creating tree items
*************************************************************************/

// allocates and returns a new TreeItem
TreeItem* ceguiLua_createTreeItem(const String& text, uint item_id, void* item_data, bool disabled, bool auto_delete)
{
	return new TreeItem(text,item_id,item_data,disabled,auto_delete);
}


/************************************************************************
    OutStream
*************************************************************************/
void ceguiLua_FileStream_open(FileStream* os, const char* filename)
{
    os->open(filename, std::ios::binary);
}

/************************************************************************
    EventConnection helper class implementation
*************************************************************************/
EventConnection::EventConnection(RefCounted<BoundSlot> slot) :
    d_slot(slot)
{}

//----------------------------------------------------------------------------//
bool EventConnection::connected() const
{
    return d_slot->connected();
}

//----------------------------------------------------------------------------//
void EventConnection::disconnect()
{
    d_slot->disconnect();
}

//----------------------------------------------------------------------------//

/************************************************************************
    ceguiLua_PropertyHelper class implementation
*************************************************************************/
float ceguiLua_PropertyHelper::stringToFloat(const String& str)
{
    return PropertyHelper<float>::fromString(str);
}

//----------------------------------------------------------------------------//
unsigned int ceguiLua_PropertyHelper::stringToUint(const String& str)
{
    return PropertyHelper<uint>::fromString(str);
}

//----------------------------------------------------------------------------//
bool ceguiLua_PropertyHelper::stringToBool(const String& str)
{
    return PropertyHelper<bool>::fromString(str);
}

//----------------------------------------------------------------------------//
Sizef ceguiLua_PropertyHelper::stringToSize(const String& str)
{
    return PropertyHelper<Sizef >::fromString(str);
}

//----------------------------------------------------------------------------//
Vector2f ceguiLua_PropertyHelper::stringToVector2(const String& str)
{
    return PropertyHelper<Vector2f >::fromString(str);
}

//----------------------------------------------------------------------------//
Rectf ceguiLua_PropertyHelper::stringToRect(const String& str)
{
    return PropertyHelper<Rectf >::fromString(str);
}

//----------------------------------------------------------------------------//
const Image* ceguiLua_PropertyHelper::stringToImage(const String& str)
{
    return PropertyHelper<const Image*>::fromString(str);
}

//----------------------------------------------------------------------------//
Colour ceguiLua_PropertyHelper::stringToColour(const String& str)
{
    return PropertyHelper<Colour>::fromString(str);
}

//----------------------------------------------------------------------------//
ColourRect ceguiLua_PropertyHelper::stringToColourRect(const String& str)
{
    return PropertyHelper<ColourRect>::fromString(str);
}

//----------------------------------------------------------------------------//
UDim ceguiLua_PropertyHelper::stringToUDim(const String& str)
{
    return PropertyHelper<UDim>::fromString(str);
}

//----------------------------------------------------------------------------//
UVector2 ceguiLua_PropertyHelper::stringToUVector2(const String& str)
{
    return PropertyHelper<UVector2>::fromString(str);
}

//----------------------------------------------------------------------------//
USize ceguiLua_PropertyHelper::stringToUSize(const String& str)
{
    return PropertyHelper<USize>::fromString(str);
}

//----------------------------------------------------------------------------//
URect ceguiLua_PropertyHelper::stringToURect(const String& str)
{
    return PropertyHelper<URect>::fromString(str);
}

//----------------------------------------------------------------------------//
AspectMode ceguiLua_PropertyHelper::stringToAspectMode(const String& str)
{
    return PropertyHelper<AspectMode>::fromString(str);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::floatToString(float val)
{
    return PropertyHelper<float>::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::uintToString(unsigned int val)
{
    return PropertyHelper<uint>::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::boolToString(bool val)
{
    return PropertyHelper<bool>::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::sizeToString(const Sizef& val)
{
    return PropertyHelper<Sizef >::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::vector2ToString(const Vector2f& val)
{
    return PropertyHelper<Vector2f >::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::rectToString(const Rectf& val)
{
    return PropertyHelper<Rectf >::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::imageToString(const Image* val)
{
    return PropertyHelper<const Image*>::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::colourToString(const Colour& val)
{
    return PropertyHelper<Colour>::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::colourRectToString(const ColourRect& val)
{
    return PropertyHelper<ColourRect>::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::udimToString(const UDim& val)
{
    return PropertyHelper<UDim>::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::uvector2ToString(const UVector2& val)
{
    return PropertyHelper<UVector2>::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::usizeToString(const USize& val)
{
    return PropertyHelper<USize>::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::urectToString(const URect& val)
{
    return PropertyHelper<URect>::toString(val);
}

//----------------------------------------------------------------------------//
String ceguiLua_PropertyHelper::aspectModeToString(AspectMode val)
{
    return PropertyHelper<AspectMode>::toString(val);
}

//----------------------------------------------------------------------------//

} // namespace CEGUI
