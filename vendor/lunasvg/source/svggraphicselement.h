#ifndef SVGGRAPHICSELEMENT_H
#define SVGGRAPHICSELEMENT_H

#include "svgstyledelement.h"
#include "svgtransform.h"
#include "rendercontext.h"

namespace lunasvg {

class SVGGraphicsElement : public SVGStyledElement
{
public:
    SVGGraphicsElement(DOMElementID elementId, SVGDocument* document);
    bool isSVGGraphicsElement() const { return true; }
    const DOMSVGTransform& transform() const { return m_transform; }
    virtual void render(RenderContext& context) const;

private:
    DOMSVGTransform m_transform;
};

} // namespace lunasvg

#endif // SVGGRAPHICSELEMENT_H
