#ifndef SVGDEFSELEMENT_H
#define SVGDEFSELEMENT_H

#include "svggraphicselement.h"

namespace lunasvg {

class SVGDefsElement : public SVGGraphicsElement
{
public:
    SVGDefsElement(SVGDocument* document);
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;
};

} // namespace lunasvg

#endif // SVGDEFSELEMENT_H
