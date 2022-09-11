#include "styleelement.h"

namespace lunasvg {

StyleElement::StyleElement()
    : Element(ElementId::Style)
{
}

std::unique_ptr<Node> StyleElement::clone() const
{
    return cloneElement<StyleElement>();
}

} // namespace lunasvg
