#include "svgclippathelement.h"

namespace lunasvg {

SVGClipPathElement::SVGClipPathElement(SVGDocument* document)
    : SVGGraphicsElement(DOMElementIdClipPath, document),
      m_clipPathUnits(DOMPropertyIdClipPathUnits)
{
    addToPropertyMap(m_clipPathUnits);
}

void SVGClipPathElement::applyClip(RenderState& state) const
{
    if(RenderBreaker::hasElement(this))
        return;

    RenderContext newContext(this, RenderModeClipping);
    RenderState& newState = newContext.state();
    newState.element = this;
    newState.canvas.reset(state.canvas.width(), state.canvas.height());
    newState.matrix = state.matrix;
    newState.viewPort = state.viewPort;
    newState.dpi = state.dpi;

    if(m_clipPathUnits.isSpecified() && m_clipPathUnits.property()->enumValue() == UnitTypeObjectBoundingBox)
    {
        newState.matrix.translate(state.bbox.x, state.bbox.y);
        newState.matrix.scale(state.bbox.width, state.bbox.height);
    }

    RenderBreaker::registerElement(this);
    newContext.render(this, tail);
    RenderBreaker::unregisterElement(this);

    state.canvas.blend(newState.canvas, BlendModeDst_In, 1.0, 0.0, 0.0);
}

void SVGClipPathElement::render(RenderContext& context) const
{
    if(context.state().element->elementId() != DOMElementIdClipPath)
    {
        context.skipElement();
        return;
    }

    SVGGraphicsElement::render(context);
}

SVGElementImpl* SVGClipPathElement::clone(SVGDocument* document) const
{
    SVGClipPathElement* e = new SVGClipPathElement(document);
    baseClone(*e);
    return e;
}

} // namespace lunasvg
