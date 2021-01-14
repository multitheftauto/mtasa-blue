#ifndef SVGSVGELEMENT_H
#define SVGSVGELEMENT_H

#include "svggraphicselement.h"
#include "svgfittoviewbox.h"
#include "svglength.h"

namespace lunasvg {

class SVGSVGElement : public SVGGraphicsElement,
        public SVGFitToViewBox
{
public:
    SVGSVGElement(SVGDocument* document);
    const DOMSVGLength& x() const { return m_x; }
    const DOMSVGLength& y() const { return m_y; }
    const DOMSVGLength& width() const { return m_width; }
    const DOMSVGLength& height() const { return m_height; }
    virtual void render(RenderContext& context) const;
    virtual SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_x;
    DOMSVGLength m_y;
    DOMSVGLength m_width;
    DOMSVGLength m_height;
};

class SVGRootElement : public SVGSVGElement
{
public:
    SVGRootElement(SVGDocument* document);
    bool isSVGRootElement() const { return true; }
    void renderToBitmap(Bitmap& bitmap, const Rect& viewBox, double dpi, std::uint32_t bgColor) const;
};

} // namespace lunasvg

#endif // SVGSVGELEMENT_H
