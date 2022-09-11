#include "useelement.h"
#include "parser.h"
#include "layoutcontext.h"

#include "gelement.h"
#include "svgelement.h"

namespace lunasvg {

UseElement::UseElement()
    : GraphicsElement(ElementId::Use)
{
}

Length UseElement::x() const
{
    auto& value = get(PropertyId::X);
    return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
}

Length UseElement::y() const
{
    auto& value = get(PropertyId::Y);
    return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
}

Length UseElement::width() const
{
    auto& value = get(PropertyId::Width);
    return Parser::parseLength(value, ForbidNegativeLengths, Length::HundredPercent);
}

Length UseElement::height() const
{
    auto& value = get(PropertyId::Height);
    return Parser::parseLength(value, ForbidNegativeLengths, Length::HundredPercent);
}

std::string UseElement::href() const
{
    auto& value = get(PropertyId::Href);
    return Parser::parseHref(value);
}

void UseElement::transferWidthAndHeight(Element* element) const
{
    auto& width = get(PropertyId::Width);
    auto& height = get(PropertyId::Height);

    element->set(PropertyId::Width, width, 0x0);
    element->set(PropertyId::Height, height, 0x0);
}

void UseElement::layout(LayoutContext* context, LayoutContainer* current) const
{
    if(isDisplayNone())
        return;

    auto ref = context->getElementById(href());
    if(ref == nullptr || context->hasReference(ref) || (current->id == LayoutId::ClipPath && !ref->isGeometry()))
        return;

    LayoutBreaker layoutBreaker(context, ref);
    auto group = std::make_unique<GElement>();
    group->parent = parent;
    group->properties = properties;

    LengthContext lengthContext(this);
    auto _x = lengthContext.valueForLength(x(), LengthMode::Width);
    auto _y = lengthContext.valueForLength(y(), LengthMode::Height);

    auto transform = get(PropertyId::Transform);
    transform += "translate(";
    transform += std::to_string(_x);
    transform += ' ';
    transform += std::to_string(_y);
    transform += ')';
    group->set(PropertyId::Transform, transform, 0x10);

    if(ref->id == ElementId::Svg || ref->id == ElementId::Symbol)
    {
        auto element = ref->cloneElement<SVGElement>();
        transferWidthAndHeight(element.get());
        group->addChild(std::move(element));
    }
    else
    {
        group->addChild(ref->clone());
    }

    group->layout(context, current);
}

std::unique_ptr<Node> UseElement::clone() const
{
    return cloneElement<UseElement>();
}

} // namespace lunasvg
