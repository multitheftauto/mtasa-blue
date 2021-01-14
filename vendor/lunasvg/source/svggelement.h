#ifndef SVGGELEMENT_H
#define SVGGELEMENT_H

#include "svggraphicselement.h"

namespace lunasvg {

class SVGGElement : public SVGGraphicsElement
{
public:
    SVGGElement(SVGDocument* document);
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;
};

} // namespace lunasvg

#endif // SVGGELEMENT_H
