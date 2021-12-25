#include "defselement.h"

namespace lunasvg {

DefsElement::DefsElement()
    : GraphicsElement(ElementId::Defs)
{
}

std::unique_ptr<Node> DefsElement::clone() const
{
    return cloneElement<DefsElement>();
}

} // namespace lunasvg
