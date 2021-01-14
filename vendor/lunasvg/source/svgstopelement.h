#ifndef SVGSTOPELEMENT_H
#define SVGSTOPELEMENT_H

#include "svgstyledelement.h"
#include "svgnumber.h"

namespace lunasvg {

class Rgb;

class SVGStopElement : public SVGStyledElement
{
public:
    SVGStopElement(SVGDocument* document);
    const DOMSVGNumberPercentage& offset() const { return m_offset; }
    Rgb stopColorWithOpacity() const;
    double offsetValue() const;
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGNumberPercentage m_offset;
};

} // namespace lunasvg

#endif // SVGSTOPELEMENT_H
