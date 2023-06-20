#include "element.h"
#include "svgelement.h"

namespace lunasvg {

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
    for(auto& property : properties) {
        if(property.id == id) {
            if(specificity >= property.specificity) {
                property.specificity = specificity;
                property.value = value;
            }

            return;
        }
    }

    Property property{specificity, id, value};
    properties.push_back(std::move(property));
}

static const std::string EmptyString;

const std::string& Element::get(PropertyID id) const
{
    for(auto& property : properties) {
        if(property.id == id) {
            return property.value;
        }
    }

    return EmptyString;
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
    for(auto& property : properties) {
        if(property.id == id) {
            return true;
        }
    }

    return false;
}

Element* Element::previousElement() const
{
    if(parent == nullptr)
        return nullptr;

    Element* element = nullptr;
    auto it = parent->children.begin();
    auto end = parent->children.end();
    for(; it != end; ++it) {
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
    for(; it != end; ++it) {
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
    for(auto& child : children) {
        child->layout(context, current);
    }
}

Rect Element::currentViewport() const
{
    if(parent == nullptr) {
        auto element = static_cast<const SVGElement*>(this);
        if(element->has(PropertyID::ViewBox))
            return element->viewBox(); 
        return Rect{0, 0, 300, 150};
    }

    if(parent->id == ElementID::Svg) {
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
