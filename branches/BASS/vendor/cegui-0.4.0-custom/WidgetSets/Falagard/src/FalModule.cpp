/************************************************************************
    filename:   FalModule.cpp
    created:    Wed Jun 22 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "StdInc.h"
#include "FalModule.h"
#include "CEGUIExceptions.h"
#include "CEGUIWindowFactoryManager.h"
// includes for types we create
#include "FalButton.h"
#include "FalCheckbox.h"
#include "FalComboDropList.h"
#include "FalCombobox.h"
#include "FalEditbox.h"
#include "FalFrameWindow.h"
#include "FalListHeader.h"
#include "FalListHeaderSegment.h"
#include "FalListbox.h"
#include "FalMenuBar.h"
#include "FalMenuItem.h"
#include "FalMultiColumnList.h"
#include "FalMultiLineEditbox.h"
#include "FalPopupMenu.h"
#include "FalProgressBar.h"
#include "FalRadioButton.h"
#include "FalScrollablePane.h"
#include "FalScrollbar.h"
#include "FalSlider.h"
#include "FalSpinner.h"
#include "FalStaticImage.h"
#include "FalStaticText.h"
#include "FalSystemButton.h"
#include "FalTabButton.h"
#include "FalTabControl.h"
#include "FalTabPane.h"
#include "FalThumb.h"
#include "FalTitlebar.h"
#include "FalTooltip.h"

/*************************************************************************
    Static factory objects
*************************************************************************/
static CEGUI::FalagardButtonFactory             s_ButtonFactory;
static CEGUI::FalagardCheckboxFactory           s_CheckboxFactory;
static CEGUI::FalagardComboDropListFactory      s_ComboDropListFactory;
static CEGUI::FalagardComboboxFactory           s_ComboboxFactory;
static CEGUI::FalagardEditboxFactory            s_EditboxFactory;
static CEGUI::FalagardFrameWindowFactory        s_FrameWindowFactory;
static CEGUI::FalagardListHeaderFactory         s_ListHeaderFactory;
static CEGUI::FalagardListHeaderSegmentFactory  s_ListHeaderSegmentFactory;
static CEGUI::FalagardListboxFactory            s_ListboxFactory;
static CEGUI::FalagardMenubarFactory            s_MenubarFactory;
static CEGUI::FalagardMenuItemFactory           s_MenuItemFactory;
static CEGUI::FalagardMultiColumnListFactory    s_MultiColumnListFactory;
static CEGUI::FalagardMultiLineEditboxFactory   s_MultiLineEditboxFactory;
static CEGUI::FalagardPopupMenuFactory          s_PopupMenuFactory;
static CEGUI::FalagardProgressBarFactory        s_ProgressBarFactory;
static CEGUI::FalagardRadioButtonFactory        s_RadioButtonFactory;
static CEGUI::FalagardScrollablePaneFactory     s_ScrollablePaneFactory;
static CEGUI::FalagardScrollbarFactory          s_ScrollbarFactory;
static CEGUI::FalagardSliderFactory             s_SliderFactory;
static CEGUI::FalagardSpinnerFactory            s_SpinnerFactory;
static CEGUI::FalagardStaticImageFactory        s_StaticImageFactory;
static CEGUI::FalagardStaticTextFactory         s_StaticTextFactory;
static CEGUI::FalagardSystemButtonFactory       s_SystemButtonFactory;
static CEGUI::FalagardTabButtonFactory          s_TabButtonFactory;
static CEGUI::FalagardTabControlFactory         s_TabControlFactory;
static CEGUI::FalagardTabPaneFactory            s_TabPaneFactory;
static CEGUI::FalagardThumbFactory              s_ThumbFactory;
static CEGUI::FalagardTitlebarFactory           s_TitlebarFactory;
static CEGUI::FalagardTooltipFactory            s_TooltipFactory;


using namespace CEGUI;

struct mapEntry
{
    const CEGUI::utf8* d_name;
    CEGUI::WindowFactory* d_factory;
};

mapEntry factoriesMap[] =
{
    {FalagardButton::WidgetTypeName, &s_ButtonFactory},
    {FalagardCheckbox::WidgetTypeName, &s_CheckboxFactory},
    {FalagardComboDropList::WidgetTypeName, &s_ComboDropListFactory},
    {FalagardCombobox::WidgetTypeName, &s_ComboboxFactory},
    {FalagardEditbox::WidgetTypeName, &s_EditboxFactory},
    {FalagardFrameWindow::WidgetTypeName, &s_FrameWindowFactory},
    {FalagardListHeader::WidgetTypeName, &s_ListHeaderFactory},
    {FalagardListHeaderSegment::WidgetTypeName, &s_ListHeaderSegmentFactory},
    {FalagardListbox::WidgetTypeName, &s_ListboxFactory},
    {FalagardMenubar::WidgetTypeName, &s_MenubarFactory},
    {FalagardMenuItem::WidgetTypeName, &s_MenuItemFactory},
    {FalagardMultiColumnList::WidgetTypeName, &s_MultiColumnListFactory},
    {FalagardMultiLineEditbox::WidgetTypeName, &s_MultiLineEditboxFactory},
    {FalagardPopupMenu::WidgetTypeName, &s_PopupMenuFactory},
    {FalagardProgressBar::WidgetTypeName, &s_ProgressBarFactory},
    {FalagardRadioButton::WidgetTypeName, &s_RadioButtonFactory},
    {FalagardScrollablePane::WidgetTypeName, &s_ScrollablePaneFactory},
    {FalagardScrollbar::WidgetTypeName, &s_ScrollbarFactory},
    {FalagardSlider::WidgetTypeName, &s_SliderFactory},
    {FalagardSpinner::WidgetTypeName, &s_SpinnerFactory},
    {FalagardStaticImage::WidgetTypeName, &s_StaticImageFactory},
    {FalagardStaticText::WidgetTypeName, &s_StaticTextFactory},
    {FalagardSystemButton::WidgetTypeName, &s_SystemButtonFactory},
    {FalagardTabButton::WidgetTypeName, &s_TabButtonFactory},
    {FalagardTabControl::WidgetTypeName, &s_TabControlFactory},
    {FalagardTabPane::WidgetTypeName, &s_TabPaneFactory},
    {FalagardThumb::WidgetTypeName, &s_ThumbFactory},
    {FalagardTitlebar::WidgetTypeName, &s_TitlebarFactory},
    {FalagardTooltip::WidgetTypeName, &s_TooltipFactory},
    {0,0}
};

/*************************************************************************
	Plugin access interface
*************************************************************************/
extern "C" void registerFactory(const CEGUI::String& type_name)
{
    mapEntry* entry = factoriesMap;

    while (entry->d_name)
    {
        if (entry->d_name == type_name)
        {
            doSafeFactoryRegistration(entry->d_factory);
            return;
        }

        ++entry;
    }

    throw UnknownObjectException((utf8*)"::registerFactory - The window factory for type '" + type_name + "' is not known in this module.");
}

CEGUI::uint registerAllFactoriesF(void)
{
    CEGUI::uint count = 0;
    mapEntry* entry = factoriesMap;

    while (entry->d_name)
    {
		doSafeFactoryRegistration(entry->d_factory);

        ++entry;
        ++count;
    }

    return count;
}

void doSafeFactoryRegistration(WindowFactory* factory)
{
	assert(factory != 0);

	WindowFactoryManager& wfm = WindowFactoryManager::getSingleton();

	// is this factory is already registered
	if (wfm.isFactoryPresent(factory->getTypeName()))
	{
		// log the fact that this type already appears to be registered
		Logger::getSingleton().logEvent(
			"Falagard widget factory '" + factory->getTypeName() + "' appears to be already registered, skipping.",
			Informative);
	}
	// factory not already registered,
	else
	{
		// add this factory to those available
		wfm.addFactory(factory);
	}
}
