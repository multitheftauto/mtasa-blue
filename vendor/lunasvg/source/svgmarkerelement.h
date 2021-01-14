#ifndef SVGMARKERELEMENT_H
#define SVGMARKERELEMENT_H

#include "svgstyledelement.h"
#include "svgfittoviewbox.h"
#include "svglength.h"
#include "svgangle.h"
#include "svgenumeration.h"

namespace lunasvg {

class Point;

class SVGMarkerElement : public SVGStyledElement,
        SVGFitToViewBox
{
public:
    SVGMarkerElement(SVGDocument* document);
    const DOMSVGLength& refX() const { return m_refX; }
    const DOMSVGLength& refY() const { return m_refY; }
    const DOMSVGLength& markerWidth() const { return m_markerWidth; }
    const DOMSVGLength& markerHeight() const { return m_markerHeight; }
    const DOMSVGAngle& orient() const { return m_orient; }
    const DOMSVGEnumeration<MarkerUnitType>& markerUnits() { return m_markerUnits; }
    void renderMarker(RenderState& state, const Point& origin, double angle) const;
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_refX;
    DOMSVGLength m_refY;
    DOMSVGLength m_markerWidth;
    DOMSVGLength m_markerHeight;
    DOMSVGAngle m_orient;
    DOMSVGEnumeration<MarkerUnitType> m_markerUnits;
};

} // namespace lunasvg

#endif // SVGMARKERELEMENT_H
