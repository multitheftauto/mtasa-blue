/************************************************************************
	filename: 	CEGUI.h
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Main system include for client code
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
#ifndef _CEGUI_h_
#define _CEGUI_h_

// base stuff
#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIRect.h"
#include "CEGUIExceptions.h"
#include "CEGUITexture.h"
#include "CEGUIRenderer.h"
#include "CEGUIImageset.h"
#include "CEGUIImagesetManager.h"
#include "CEGUILogger.h"
#include "CEGUIMouseCursor.h"
#include "CEGUIFont.h"
#include "CEGUIFontManager.h"
#include "CEGUIEventArgs.h"
#include "CEGUIEvent.h"
#include "CEGUIEventSet.h"
#include "CEGUIGlobalEventSet.h"
#include "CEGUIProperty.h"
#include "CEGUIPropertySet.h"
#include "CEGUIWindow.h"
#include "CEGUIWindowFactory.h"
#include "CEGUIWindowFactoryManager.h"
#include "CEGUIWindowManager.h"
#include "CEGUIScheme.h"
#include "CEGUISchemeManager.h"
#include "CEGUISystem.h"
#include "CEGUIScriptModule.h"

// mid-level renderables
#include "CEGUIRenderableFrame.h"
#include "CEGUIRenderableImage.h"

// gui elements
#include "elements/CEGUIButtonBase.h"
#include "elements/CEGUIPushButton.h"
#include "elements/CEGUICheckbox.h"
#include "elements/CEGUIRadioButton.h"
#include "elements/CEGUITitlebar.h"
#include "elements/CEGUIFrameWindow.h"
#include "elements/CEGUIProgressBar.h"
#include "elements/CEGUIEditbox.h"
#include "elements/CEGUIThumb.h"
#include "elements/CEGUISlider.h"
#include "elements/CEGUIScrollbar.h"
#include "elements/CEGUIStatic.h"
#include "elements/CEGUIListbox.h"
#include "elements/CEGUICombobox.h"
#include "elements/CEGUIListHeader.h"
#include "elements/CEGUIMultiColumnList.h"
#include "elements/CEGUIStaticText.h"
#include "elements/CEGUIStaticImage.h"
#include "elements/CEGUIGUISheet.h"
#include "elements/CEGUIListboxTextItem.h"
#include "elements/CEGUIListboxNumberItem.h"
#include "elements/CEGUIListboxImageItem.h"
#include "elements/CEGUIComboDropList.h"
#include "elements/CEGUIMultiLineEditbox.h"
#include "elements/CEGUITabButton.h"
#include "elements/CEGUITabPane.h"
#include "elements/CEGUITabControl.h"
#include "elements/CEGUISpinner.h"
#include "elements/CEGUIDragContainer.h"
#include "elements/CEGUIScrolledContainer.h"
#include "elements/CEGUIScrollablePane.h"
#include "elements/CEGUITooltip.h"
#include "elements/CEGUIItemEntry.h"
#include "elements/CEGUITextItem.h"
#include "elements/CEGUIMenuItem.h"
#include "elements/CEGUIItemListBase.h"
#include "elements/CEGUIMenuBase.h"
#include "elements/CEGUIMenubar.h"
#include "elements/CEGUIPopupMenu.h"

#endif	// end of guard _CEGUI_h_
