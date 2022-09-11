#include "stopelement.h"
#include "parser.h"

namespace lunasvg {

StopElement::StopElement()
    : StyledElement(ElementId::Stop)
{
}

double StopElement::offset() const
{
    auto& value = get(PropertyId::Offset);
    return Parser::parseNumberPercentage(value, 1.0);
}

Color StopElement::stopColorWithOpacity() const
{
    auto color = stop_color();
    color.a = stop_opacity();
    return color;
}

std::unique_ptr<Node> StopElement::clone() const
{
    return cloneElement<StopElement>();
}

} // namespace lunasvg
