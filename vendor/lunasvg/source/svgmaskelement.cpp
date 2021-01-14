#include "svgmaskelement.h"
#include "rendercontext.h"

namespace lunasvg {

SVGMaskElement::SVGMaskElement(SVGDocument* document)
    : SVGStyledElement(DOMElementIdMask, document),
      m_x(DOMPropertyIdX, LengthModeWidth, AllowNegativeLengths),
      m_y(DOMPropertyIdY, LengthModeHeight, AllowNegativeLengths),
      m_width(DOMPropertyIdWidth, LengthModeWidth, ForbidNegativeLengths),
      m_height(DOMPropertyIdHeight, LengthModeHeight, ForbidNegativeLengths),
      m_maskUnits(DOMPropertyIdMaskUnits),
      m_maskContentUnits(DOMPropertyIdMaskContentUnits)
{
    m_x.setDefaultValue(minusTenPercent());
    m_y.setDefaultValue(minusTenPercent());
    m_height.setDefaultValue(oneTwentyPercent());
    m_width.setDefaultValue(oneTwentyPercent());

    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_width);
    addToPropertyMap(m_height);
    addToPropertyMap(m_maskUnits);
    addToPropertyMap(m_maskContentUnits);
}

void SVGMaskElement::applyMask(RenderState& state) const
{
    if(m_width.isZero() || m_height.isZero() || RenderBreaker::hasElement(this))
        return;

    RenderContext newContext(this, RenderModeDisplay);
    RenderState& newState = newContext.state();
    newState.element = this;
    newState.canvas.reset(state.canvas.width(), state.canvas.height());
    newState.matrix = state.matrix;
    newState.viewPort = state.viewPort;
    newState.dpi = state.dpi;

    if(m_maskContentUnits.isSpecified() && m_maskContentUnits.property()->enumValue() == UnitTypeObjectBoundingBox)
    {
        newState.matrix.translate(state.bbox.x, state.bbox.y);
        newState.matrix.scale(state.bbox.width, state.bbox.height);
    }

    RenderBreaker::registerElement(this);
    newContext.render(this, tail);
    RenderBreaker::unregisterElement(this);

    newState.canvas.updateLuminance();

    state.canvas.blend(newState.canvas, BlendModeDst_In, 1.0, 0.0, 0.0);
}

void SVGMaskElement::render(RenderContext& context) const
{
    if(context.state().element->elementId() != DOMElementIdMask)
    {
        context.skipElement();
        return;
    }

    SVGStyledElement::render(context);
}

SVGElementImpl* SVGMaskElement::clone(SVGDocument* document) const
{
    SVGMaskElement* e = new SVGMaskElement(document);
    baseClone(*e);
    return e;
}

} // namespace lunasvg
