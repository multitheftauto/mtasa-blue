#ifndef SVGMASKELEMENT_H
#define SVGMASKELEMENT_H

#include "svgstyledelement.h"
#include "svglength.h"
#include "svgenumeration.h"

namespace lunasvg {

class SVGMaskElement : public SVGStyledElement
{
public:
    SVGMaskElement(SVGDocument* document);
    const DOMSVGLength& x() const { return m_x; }
    const DOMSVGLength& y() const { return m_y; }
    const DOMSVGLength& width() const { return m_width; }
    const DOMSVGLength& height() const { return m_height; }
    const DOMSVGEnumeration<UnitType>& maskUnits() const { return m_maskUnits; }
    const DOMSVGEnumeration<UnitType>& maskContentUnits() const { return m_maskContentUnits; }
    void applyMask(RenderState& state) const;
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_x;
    DOMSVGLength m_y;
    DOMSVGLength m_width;
    DOMSVGLength m_height;
    DOMSVGEnumeration<UnitType> m_maskUnits;
    DOMSVGEnumeration<UnitType> m_maskContentUnits;
};

} // namespace lunasvg

#endif // SVGMASKELEMENT_H
