#include "svggraphicselement.h"

namespace lunasvg {

SVGGraphicsElement::SVGGraphicsElement(DOMElementID elementId, SVGDocument* document)
    : SVGStyledElement(elementId, document),
      m_transform(DOMPropertyIdTransform)
{
    addToPropertyMap(m_transform);
}

void SVGGraphicsElement::render(RenderContext& context) const
{
    SVGStyledElement::render(context);
    RenderState& state = context.state();
    if(m_transform.isSpecified())
        state.matrix.multiply(m_transform.property()->value());
}

} // namespace lunasvg
