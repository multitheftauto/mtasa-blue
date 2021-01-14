#include "svgstopelement.h"
#include "svgcolor.h"
#include "rendercontext.h"

namespace lunasvg {

SVGStopElement::SVGStopElement(SVGDocument* document)
    : SVGStyledElement(DOMElementIdStop, document),
      m_offset(DOMPropertyIdOffset)
{
    addToPropertyMap(m_offset);
}

double SVGStopElement::offsetValue() const
{
    return m_offset.isSpecified() ? m_offset.property()->value() : 0.0;
}

void SVGStopElement::render(RenderContext& context) const
{
    context.skipElement();
}

Rgb SVGStopElement::stopColorWithOpacity() const
{
    if(!style().isSpecified())
        return KRgbBlack;

    Rgb color;
    if(const CSSPropertyBase* item = style().property()->getItem(CSSPropertyIdStop_Color))
    {
        const SVGPropertyBase* property = !item->isInherited() ? item->property() : findInheritedProperty(CSSPropertyIdStop_Color);
        if(property)
        {
            const SVGColor* stopColor = to<SVGColor>(property);
            color = stopColor->colorType() == ColorTypeCurrentColor ? currentColor() : stopColor->value();
        }
    }

    if(const CSSPropertyBase* item = style().property()->getItem(CSSPropertyIdStop_Opacity))
    {
        const SVGPropertyBase* property = !item->isInherited() ? item->property() : findInheritedProperty(CSSPropertyIdStop_Opacity);
        if(property)
        {
            const SVGNumber* stopOpacity = to<SVGNumber>(property);
            color.a = std::uint8_t(stopOpacity->value() * 255.0);
        }
    }

    return color;
}

SVGElementImpl* SVGStopElement::clone(SVGDocument* document) const
{
    SVGStopElement* e = new SVGStopElement(document);
    baseClone(*e);
    return e;
}

} // namespace lunasvg
