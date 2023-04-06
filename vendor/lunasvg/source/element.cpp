#include "element.h"
#include "parser.h"
#include "svgelement.h"

namespace lunasvg {

void PropertyList::set(PropertyID id, const std::string& value, int specificity)
{
    auto property = get(id);
    if(property == nullptr)
    {
        Property property{id, value, specificity};
        m_properties.push_back(std::move(property));
        return;
    }

    if(property->specificity > specificity)
        return;

    property->specificity = specificity;
    property->value = value;
}

Property* PropertyList::get(PropertyID id) const
{
    auto data = m_properties.data();
    auto end = data + m_properties.size();
    while(data < end)
    {
        if(data->id == id)
            return const_cast<Property*>(data);
        ++data;
    }

    return nullptr;
}

void PropertyList::add(const Property& property)
{
    set(property.id, property.value, property.specificity);
}

void PropertyList::add(const PropertyList& properties)
{
    auto it = properties.m_properties.begin();
    auto end = properties.m_properties.end();
    for(;it != end;++it)
        add(*it);
}

void Node::layout(LayoutContext*, LayoutContainer*) const
{
}

std::unique_ptr<Node> TextNode::clone() const
{
    auto node = std::make_unique<TextNode>();
    node->text = text;
    return std::move(node);
}

Element::Element(ElementID id)
    : id(id)
{
}

void Element::set(PropertyID id, const std::string& value, int specificity)
{
    properties.set(id, value, specificity);
}

static const std::string EmptyString;

const std::string& Element::get(PropertyID id) const
{
    auto property = properties.get(id);
    if(property == nullptr)
        return EmptyString;

    return property->value;
}

static const std::string InheritString{"inherit"};

const std::string& Element::find(PropertyID id) const
{
    auto element = this;
    do {
        auto& value = element->get(id);
        if(!value.empty() && value != InheritString)
            return value;
        element = element->parent;
    } while(element);

    return EmptyString;
}

bool Element::has(PropertyID id) const
{
    return properties.get(id);
}

Element* Element::previousElement() const
{
    if(parent == nullptr)
        return nullptr;

    Element* element = nullptr;
    auto it = parent->children.begin();
    auto end = parent->children.end();
    for(;it != end;++it)
    {
        auto node = it->get();
        if(node->isText())
            continue;

        if(node == this)
            return element;
        element = static_cast<Element*>(node);
    }

    return nullptr;
}

Element* Element::nextElement() const
{
    if(parent == nullptr)
        return nullptr;

    Element* element = nullptr;
    auto it = parent->children.rbegin();
    auto end = parent->children.rend();
    for(;it != end;++it)
    {
        auto node = it->get();
        if(node->isText())
            continue;

        if(node == this)
            return element;
        element = static_cast<Element*>(node);
    }

    return nullptr;
}

Node* Element::addChild(std::unique_ptr<Node> child)
{
    child->parent = this;
    children.push_back(std::move(child));
    return &*children.back();
}

void Element::layoutChildren(LayoutContext* context, LayoutContainer* current) const
{
    for(auto& child : children)
        child->layout(context, current);
}

Rect Element::currentViewport() const
{
    if(parent == nullptr)
    {
        auto element = static_cast<const SVGElement*>(this);
        if(element->has(PropertyID::ViewBox))
            return element->viewBox(); 
        return Rect{0, 0, 300, 150};
    }

    if(parent->id == ElementID::Svg)
    {
        auto element = static_cast<SVGElement*>(parent);
        if(element->has(PropertyID::ViewBox))
            return element->viewBox();

        LengthContext lengthContext(element);
        auto _x = lengthContext.valueForLength(element->x(), LengthMode::Width);
        auto _y = lengthContext.valueForLength(element->y(), LengthMode::Height);
        auto _w = lengthContext.valueForLength(element->width(), LengthMode::Width);
        auto _h = lengthContext.valueForLength(element->height(), LengthMode::Height);
        return Rect{_x, _y, _w, _h};
    }

    return parent->currentViewport();
}

} // namespace lunasvg
