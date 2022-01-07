#include "symbolelement.h"
#include "parser.h"

namespace lunasvg {

SymbolElement::SymbolElement()
    : StyledElement(ElementId::Symbol)
{
}

Length SymbolElement::x() const
{
    auto& value = get(PropertyId::X);
    return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
}

Length SymbolElement::y() const
{
    auto& value = get(PropertyId::Y);
    return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
}

Length SymbolElement::width() const
{
    auto& value = get(PropertyId::Width);
    return Parser::parseLength(value, ForbidNegativeLengths, Length::HundredPercent);
}

Length SymbolElement::height() const
{
    auto& value = get(PropertyId::Height);
    return Parser::parseLength(value, ForbidNegativeLengths, Length::HundredPercent);
}

Rect SymbolElement::viewBox() const
{
    auto& value = get(PropertyId::ViewBox);
    return Parser::parseViewBox(value);
}

PreserveAspectRatio SymbolElement::preserveAspectRatio() const
{
    auto& value = get(PropertyId::PreserveAspectRatio);
    return Parser::parsePreserveAspectRatio(value);
}

std::unique_ptr<Node> SymbolElement::clone() const
{
    return cloneElement<SymbolElement>();
}

} // namespace lunasvg
