#ifndef SVGUSEELEMENT_H
#define SVGUSEELEMENT_H

#include "svggraphicselement.h"
#include "svgurireference.h"
#include "svglength.h"

namespace lunasvg {

class SVGUseElement : public SVGGraphicsElement,
        public SVGURIReference
{
public:
    SVGUseElement(SVGDocument* document);
    const DOMSVGLength& x() const { return m_x; }
    const DOMSVGLength& y() const { return m_y; }
    const DOMSVGLength& width() const { return m_width; }
    const DOMSVGLength& height() const { return m_height; }
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_x;
    DOMSVGLength m_y;
    DOMSVGLength m_width;
    DOMSVGLength m_height;
};

} // namespace lunasvg

#endif // SVGUSEELEMENT_H
