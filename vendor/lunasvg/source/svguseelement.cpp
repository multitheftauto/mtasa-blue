#include "svguseelement.h"
#include "svgdocumentimpl.h"

namespace lunasvg {

SVGUseElement::SVGUseElement(SVGDocument* document)
    : SVGGraphicsElement(DOMElementIdUse, document),
      SVGURIReference(this),
      m_x(DOMPropertyIdX, LengthModeWidth, AllowNegativeLengths),
      m_y(DOMPropertyIdY, LengthModeHeight, AllowNegativeLengths),
      m_width(DOMPropertyIdWidth, LengthModeWidth, ForbidNegativeLengths),
      m_height(DOMPropertyIdHeight, LengthModeHeight, ForbidNegativeLengths)
{
    m_height.setDefaultValue(hundredPercent());
    m_width.setDefaultValue(hundredPercent());

    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_width);
    addToPropertyMap(m_height);
}

void SVGUseElement::render(RenderContext& context) const
{
    if(m_width.isZero() || m_height.isZero() || style().isDisplayNone() || RenderBreaker::hasElement(this))
    {
        context.skipElement();
        return;
    }

    SVGElementImpl* ref = document()->impl()->resolveIRI(hrefValue());
    if(!ref || (context.mode()==RenderModeClipping && !(ref->isSVGGeometryElement() || ref->elementId()==DOMElementIdText)))
    {
        context.skipElement();
        return;
    }

    SVGGraphicsElement::render(context);
    RenderState& state = context.state();
    double _x = m_x.value(state);
    double _y = m_y.value(state);

    RenderContext newContext(this, context.mode());
    RenderState& newState = newContext.state();
    newState.element = state.element;
    newState.canvas = state.canvas;
    newState.matrix = state.matrix;
    newState.matrix.translate(_x, _y);
    newState.style.inheritFrom(state.style);
    newState.viewPort = state.viewPort;
    newState.color = state.color;
    newState.dpi = state.dpi;

    if(ref->elementId()==DOMElementIdSvg || ref->elementId()==DOMElementIdSymbol)
    {
        double _w = m_width.value(state);
        double _h = m_height.value(state);
        newState.viewPort = Rect(0, 0, _w, _h);
    }

    RenderBreaker::registerElement(this);
    newContext.render(ref, to<SVGElementHead>(ref)->tail);
    RenderBreaker::unregisterElement(this);

    state.matrix = newState.matrix;
    state.bbox = newState.bbox;
}

SVGElementImpl* SVGUseElement::clone(SVGDocument* document) const
{
    SVGUseElement* e = new SVGUseElement(document);
    baseClone(*e);
    return e;
}

} // namespace lunasvg
