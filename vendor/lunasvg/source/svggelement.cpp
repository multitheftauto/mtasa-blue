#include "svggelement.h"

namespace lunasvg {

SVGGElement::SVGGElement(SVGDocument* document)
    : SVGGraphicsElement(DOMElementIdG, document)
{
}

void SVGGElement::render(RenderContext& context) const
{
    if(style().isDisplayNone())
    {
        context.skipElement();
        return;
    }

    SVGGraphicsElement::render(context);
}

SVGElementImpl* SVGGElement::clone(SVGDocument* document) const
{
    SVGGElement* e = new SVGGElement(document);
    baseClone(*e);
    return e;
}

} // namespace lunasvg
