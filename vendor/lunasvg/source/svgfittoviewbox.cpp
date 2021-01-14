#include "svgfittoviewbox.h"
#include "svgelementimpl.h"
#include "affinetransform.h"

namespace lunasvg {

SVGFitToViewBox::SVGFitToViewBox(SVGElementHead* element)
    : m_viewBox(DOMPropertyIdViewBox),
      m_preserveAspectRatio(DOMPropertyIdPreserveAspectRatio)
{
    element->addToPropertyMap(m_viewBox);
    element->addToPropertyMap(m_preserveAspectRatio);
}

AffineTransform SVGFitToViewBox::calculateViewBoxTransform(const Rect& viewPort, const Rect& viewBox) const
{
    const SVGPreserveAspectRatio* positioning = m_preserveAspectRatio.isSpecified() ? m_preserveAspectRatio.property() : SVGPreserveAspectRatio::defaultValue();
    return positioning->getMatrix(viewPort, viewBox);
}

} // namespace lunasvg
