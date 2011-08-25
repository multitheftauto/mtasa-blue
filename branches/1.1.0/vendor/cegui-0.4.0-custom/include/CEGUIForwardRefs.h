/************************************************************************
	filename: 	CEGUIForwardRefs.h
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Forward declares all core system classes
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#ifndef _CEGUIForwardRefs_h_
#define _CEGUIForwardRefs_h_

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Forward reference declarations of all core GUI system classes
*************************************************************************/
class String;
class Vector3;
class Size;
class Exception;
class Rect;
class ColourRect;
class colour;
class Texture;
class Renderer;
class Image;
class Imageset;
class ImagesetManager;
class MouseCursor;
class Font;
class FontManager;
class EventArgs;
class Event;
class EventSet;
class Property;
class PropertySet;
class Window;
class WindowFactory;
class WindowManager;
class Scheme;
class SchemeManager;
class System;
class FactoryModule;
class ScriptModule;
class ResourceProvider;
class DefaultResourceProvider;
class PropertyReceiver;
class GlobalEventSet;
class XMLAttributes;
class XMLHandler;
class XMLParser;

/*************************************************************************
	Forward reference declarations for GUI element base classes
*************************************************************************/
class ButtonBase;
class RadioButton;
class Checkbox;
class PushButton;
class Titlebar;
class FrameWindow;
class Editbox;
class Listbox;
class Combobox;
class ListHeaderSegment;
class ListHeader;
class MultiColumnList;
class ProgressBar;
class Thumb;
class Scrollbar;
class Slider;
class Static;
class StaticText;
class StaticImage;
class ListboxItem;
class ListboxTextItem;
class ListboxNumberItem;
class ListboxImageItem;
class ComboDropList;
class MultiLineEditbox;
class Spinner;
class GUISheet;
class DragContainer;
class ScrollablePane;
class ScrolledContainer;
class Tooltip;
class ItemEntry;
class TextItem;
class MenuItem;
class ItemListBase;
class MenuBase;
class Menubar;
class PopupMenu;

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIForwardRefs_h_
