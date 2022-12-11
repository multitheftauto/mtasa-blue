#include "graphicselement.h"
#include "parser.h"

namespace lunasvg {

GraphicsElement::GraphicsElement(ElementId id)
    : StyledElement(id)
{
}

Transform GraphicsElement::transform() const
{
    auto& value = get(PropertyId::Transform);
    return Parser::parseTransform(value);
}

} // namespace lunasvg
