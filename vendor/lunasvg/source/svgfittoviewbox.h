#ifndef SVGFITTOVIEWBOX_H
#define SVGFITTOVIEWBOX_H

#include "svgpreserveaspectratio.h"
#include "svgrect.h"

namespace lunasvg {

class SVGElementHead;

class SVGFitToViewBox
{
public:
    const DOMSVGViewBoxRect& viewBox() const { return m_viewBox; }
    const DOMSVGPreserveAspectRatio& preserveAspectRatio() const { return m_preserveAspectRatio; }
    AffineTransform calculateViewBoxTransform(const Rect& viewPort, const Rect& viewBox) const;

protected:
    SVGFitToViewBox(SVGElementHead* element);

private:
    DOMSVGViewBoxRect m_viewBox;
    DOMSVGPreserveAspectRatio m_preserveAspectRatio;
};

} // namespace lunasvg

#endif // SVGFITTOVIEWBOX_H
