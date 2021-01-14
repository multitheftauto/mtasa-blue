#ifndef SVGSYMBOLELEMENT_H
#define SVGSYMBOLELEMENT_H

#include "svgstyledelement.h"
#include "svgfittoviewbox.h"

namespace lunasvg {

class SVGSymbolElement : public SVGStyledElement,
        public SVGFitToViewBox
{
public:
    SVGSymbolElement(SVGDocument* document);
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;
};

} // namespace lunasvg

#endif // SVGSYMBOLELEMENT_H
