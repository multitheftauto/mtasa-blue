#include "svgmarkerelement.h"
#include "rendercontext.h"
#include "point.h"

namespace lunasvg {

#define PI 3.14159265358979323846

SVGMarkerElement::SVGMarkerElement(SVGDocument* document)
    : SVGStyledElement(DOMElementIdMarker, document),
      SVGFitToViewBox(this),
      m_refX(DOMPropertyIdRefX, LengthModeWidth, AllowNegativeLengths),
      m_refY(DOMPropertyIdRefY, LengthModeHeight, AllowNegativeLengths),
      m_markerWidth(DOMPropertyIdMarkerWidth, LengthModeWidth, ForbidNegativeLengths),
      m_markerHeight(DOMPropertyIdMarkerHeight, LengthModeHeight, ForbidNegativeLengths),
      m_orient(DOMPropertyIdOrient),
      m_markerUnits(DOMPropertyIdMaskUnits)
{
    m_markerWidth.setDefaultValue(threePixels());
    m_markerHeight.setDefaultValue(threePixels());

    addToPropertyMap(m_refX);
    addToPropertyMap(m_refY);
    addToPropertyMap(m_markerWidth);
    addToPropertyMap(m_markerHeight);
    addToPropertyMap(m_orient);
    addToPropertyMap(m_markerUnits);
}

void SVGMarkerElement::renderMarker(RenderState& state, const Point& origin, double angle) const
{
    if(RenderBreaker::hasElement(this))
        return;

    RenderContext newContext(this, RenderModeDisplay);
    RenderState& newState = newContext.state();
    newState.element = this;
    newState.canvas = state.canvas;
    newState.matrix = state.matrix;
    newState.viewPort = state.viewPort;
    newState.dpi = state.dpi;

    newState.matrix.translate(origin.x, origin.y);
    if(m_orient.isSpecified() && m_orient.property()->orientType() == MarkerOrientTypeAngle)
    {
        newState.matrix.rotate(m_orient.property()->value() * PI / 180.0);
    }
    else if(m_orient.isSpecified() && m_orient.property()->orientType() == MarkerOrientTypeAuto)
    {
        newState.matrix.rotate(angle * PI / 180.0);
    }

    if(!m_markerUnits.isSpecified() || m_markerUnits.property()->enumValue() == MarkerUnitTypeStrokeWidth)
    {
        double strokeWidth = state.style.strokeWidth(state);
        newState.matrix.scale(strokeWidth, strokeWidth);
    }

    double refX = m_refX.value(state);
    double refY = m_refY.value(state);
    if(viewBox().isSpecified() && viewBox().property()->isValid())
    {
        double w = m_markerWidth.value(state);
        double h = m_markerHeight.value(state);

        AffineTransform viewTransform = calculateViewBoxTransform(Rect(0, 0, w, h), viewBox().property()->value());
        viewTransform.map(refX, refY, refX, refY);
        newState.matrix.translate(-refX, -refY);
        newState.matrix.multiply(viewTransform);
        newState.viewPort = viewBox().property()->value();
    }
    else
    {
        newState.matrix.translate(-refX, -refY);
    }

    RenderBreaker::registerElement(this);
    newContext.render(this, tail);
    RenderBreaker::unregisterElement(this);
}

void SVGMarkerElement::render(RenderContext& context) const
{
    if(context.state().element->elementId() != DOMElementIdMarker)
    {
        context.skipElement();
        return;
    }

    SVGStyledElement::render(context);
}

SVGElementImpl* SVGMarkerElement::clone(SVGDocument* document) const
{
    SVGMarkerElement* e = new SVGMarkerElement(document);
    baseClone(*e);
    return e;
}

} // namespace lunasvg
