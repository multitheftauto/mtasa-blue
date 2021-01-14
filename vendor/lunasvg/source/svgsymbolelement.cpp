#include "svgsymbolelement.h"
#include "rendercontext.h"

namespace lunasvg {

SVGSymbolElement::SVGSymbolElement(SVGDocument* document)
    : SVGStyledElement(DOMElementIdSymbol, document),
      SVGFitToViewBox(this)
{
}

void SVGSymbolElement::render(RenderContext& context) const
{
    if(context.state().element->elementId() != DOMElementIdUse || style().isDisplayNone())
    {
        context.skipElement();
        return;
    }

    const RenderState& state = context.state();
    const Rect& viewPort = state.viewPort;

    SVGStyledElement::render(context);
    RenderState& newState = context.state();
    if(viewBox().isSpecified() && viewBox().property()->isValid())
    {
        newState.matrix.multiply(calculateViewBoxTransform(viewPort, viewBox().property()->value()));
        newState.viewPort = viewBox().property()->value();
    }
    else
    {
        newState.matrix.translate(viewPort.x, viewPort.y);
    }
}

SVGElementImpl* SVGSymbolElement::clone(SVGDocument* document) const
{
    SVGSymbolElement* e = new SVGSymbolElement(document);
    baseClone(*e);
    return e;
}

} // namespace lunasvg
