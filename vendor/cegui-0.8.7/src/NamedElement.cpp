/***********************************************************************
    created:    30/10/2011
    author:     Martin Preisler

    purpose:    Implements the NamedElement class
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
#include <queue>

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include "CEGUI/NamedElement.h"
#include "CEGUI/Logger.h"
#include "CEGUI/Exceptions.h"

// Start of CEGUI namespace section
namespace CEGUI
{

const String NamedElement::EventNamespace("NamedElement");

const String NamedElement::EventNameChanged("NameChanged");

//----------------------------------------------------------------------------//
NamedElement::NamedElement(const String& name):
    d_name(name)
{
    addNamedElementProperties();
}

//----------------------------------------------------------------------------//
NamedElement::~NamedElement()
{}

//----------------------------------------------------------------------------//
void NamedElement::setName(const String& name)
{
    if (d_name == name)
        return;

    if (getParentElement())
    {
        NamedElement* parent = dynamic_cast<NamedElement*>(getParentElement());

        if (parent && parent->isChild(name))
        {
            CEGUI_THROW(AlreadyExistsException("Failed to rename "
                "NamedElement at: " + getNamePath() + " as: " + name + ". A Window "
                "with that name is already attached as a sibling."));
        }
    }

    // log this under informative level
    Logger::getSingleton().logEvent("Renamed element at: " + getNamePath() +
                                    " as: " + name, Informative);

    d_name = name;

    NamedElementEventArgs args(this);
    onNameChanged(args);
}

//----------------------------------------------------------------------------//
String NamedElement::getNamePath() const
{
    String path("");

    Element* parent_element = getParentElement();
    NamedElement* parent_named_element = dynamic_cast<NamedElement*>(parent_element);

    if (parent_element)
    {
        if (parent_named_element)
            path = parent_named_element->getNamePath() + '/';
        else
            path = "<not a named element>/";
    }

    path += getName();

    return path;
}

//----------------------------------------------------------------------------//
bool NamedElement::isChild(const String& name_path) const
{
    return getChildByNamePath_impl(name_path) != 0;
}

//----------------------------------------------------------------------------//
bool NamedElement::isChildRecursive(const String& name) const
{
    return getChildByNameRecursive_impl(name) != 0;
}

//----------------------------------------------------------------------------//
bool NamedElement::isAncestor(const String& name) const
{
    Element const* current = this;

    while (true)
    {
        const Element* parent = current->getParentElement();

        if (!parent)
            return false;

        const NamedElement* named_parent = dynamic_cast<const NamedElement*>(parent);

        if (named_parent && named_parent->getName() == name)
            return true;

        current = parent;
    }
}

//----------------------------------------------------------------------------//
NamedElement* NamedElement::getChildElement(const String& name_path) const
{
    NamedElement* e = getChildByNamePath_impl(name_path);

    if (e)
        return e;

    CEGUI_THROW(UnknownObjectException("The Element object "
        "referenced by '" + name_path + "' is not attached to Element at '"
        + getNamePath() + "'."));
}

//----------------------------------------------------------------------------//
NamedElement* NamedElement::getChildElementRecursive(const String& name_path) const
{
    return getChildByNameRecursive_impl(name_path);
}

//----------------------------------------------------------------------------//
void NamedElement::removeChild(const String& name_path)
{
    NamedElement* e = getChildByNamePath_impl(name_path);

    if (e)
        removeChild(e);
    else
        CEGUI_THROW(UnknownObjectException("The Element object "
            "referenced by '" + name_path + "' is not attached to Element at '"
            + getNamePath() + "'."));
}

//----------------------------------------------------------------------------//
void NamedElement::addChild_impl(Element* element)
{
    NamedElement* named_element = dynamic_cast<NamedElement*>(element);

    if (named_element)
    {
        const NamedElement* const existing = getChildByNamePath_impl(named_element->getName());

        if (existing && named_element != existing)
            CEGUI_THROW(AlreadyExistsException("Failed to add "
                "Element named: " + named_element->getName() + " to element at: " +
                getNamePath() + " since an Element with that name is already "
                "attached."));
    }

    Element::addChild_impl(element);
}

//----------------------------------------------------------------------------//
NamedElement* NamedElement::getChildByNamePath_impl(const String& name_path) const
{
    const size_t sep = name_path.find_first_of('/');
    const String base_child(name_path.substr(0, sep));

    const size_t child_count = d_children.size();

    for (size_t i = 0; i < child_count; ++i)
    {
        Element* child = getChildElementAtIdx(i);
        NamedElement* named_child = dynamic_cast<NamedElement*>(child);

        if (!named_child)
            continue;

        if (named_child->getName() == base_child)
        {
            if (sep != String::npos && sep < name_path.length() - 1)
                return named_child->getChildByNamePath_impl(name_path.substr(sep + 1));
            else
                return named_child;
        }
    }

    return 0;
}

//----------------------------------------------------------------------------//
NamedElement* NamedElement::getChildByNameRecursive_impl(const String& name) const
{
    const size_t child_count = d_children.size();

    std::queue<Element*> ElementsToSearch;

    for (size_t i = 0; i < child_count; ++i) // load all children into the queue
    {
        Element* child = getChildElementAtIdx(i);
        ElementsToSearch.push(child);
    }

    while (!ElementsToSearch.empty()) // breadth-first search for the child to find
    {
        Element* child = ElementsToSearch.front();
        ElementsToSearch.pop();

        NamedElement* named_child = dynamic_cast<NamedElement*>(child);
        if (named_child)
        {
            if (named_child->getName() == name)
            {
                return named_child;
            }
        }

        const size_t element_child_count = child->getChildCount();
        for(size_t i = 0; i < element_child_count; ++i)
        {
            ElementsToSearch.push(child->getChildElementAtIdx(i));
        }
    }

    return 0;
}

//----------------------------------------------------------------------------//
void NamedElement::addNamedElementProperties()
{
    const String propertyOrigin("NamedElement");

    // "Name" is already stored in <Window> element
    CEGUI_DEFINE_PROPERTY_NO_XML(NamedElement, String,
        "Name", "Property to get/set the name of the Element. Make sure it's unique in its parent if any.",
        &NamedElement::setName, &NamedElement::getName, ""
    );

    CEGUI_DEFINE_PROPERTY_NO_XML(NamedElement, String,
        "NamePath", "Property to get the absolute name path of this Element.",
        0, &NamedElement::getNamePath, ""
    );
}

//----------------------------------------------------------------------------//
void NamedElement::onNameChanged(NamedElementEventArgs& e)
{
    fireEvent(EventNameChanged, e, EventNamespace);
}

} // End of  CEGUI namespace section
