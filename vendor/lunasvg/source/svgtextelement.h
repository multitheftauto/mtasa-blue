#ifndef SVGTEXTELEMENT_H
#define SVGTEXTELEMENT_H

#include "svggraphicselement.h"
#include "svglength.h"

namespace lunasvg {

class SVGTextElement : public SVGGraphicsElement
{
public:
    SVGTextElement(SVGDocument* document);
    const DOMSVGLengthList& x() const { return m_x; }
    const DOMSVGLengthList& y() const { return m_y; }
    std::vector<std::uint32_t> buildTextContent() const;
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLengthList m_x;
    DOMSVGLengthList m_y;
};

} // namespace lunasvg

#endif // SVGTEXTELEMENT_H
