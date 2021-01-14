#ifndef SVGCLIPPATHELEMENT_H
#define SVGCLIPPATHELEMENT_H

#include "svggraphicselement.h"

namespace lunasvg {

class SVGClipPathElement : public SVGGraphicsElement
{
public:
    SVGClipPathElement(SVGDocument* document);
    const DOMSVGEnumeration<UnitType>& clipPathUnits() const { return m_clipPathUnits; }
    void applyClip(RenderState& state) const;
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGEnumeration<UnitType> m_clipPathUnits;
};

} // namespace lunasvg

#endif // SVGCLIPPATHELEMENT_H
