#ifndef SVGSTYLEDELEMENT_H
#define SVGSTYLEDELEMENT_H

#include "svgelementimpl.h"
#include "cssproperty.h"

namespace lunasvg {

class Rgb;

class SVGStyledElement : public SVGElementHead
{
public:
    SVGStyledElement(DOMElementID elementId, SVGDocument* document);
    virtual void setAttribute(const std::string& name, const std::string& value);
    virtual std::string getAttribute(const std::string& name) const;
    virtual bool hasAttribute(const std::string& name) const;
    virtual void removeAttribute(const std::string& name);
    virtual void render(RenderContext& context) const;
    virtual void renderTail(RenderContext& context) const;
    bool isSVGStyledElement() const { return true; }
    const DOMSVGStyle& style() const { return m_style; }
    Rgb currentColor() const;
    SVGPropertyBase* findInheritedProperty(CSSPropertyID nameId) const;

private:
    DOMSVGStyle m_style;
};

} // namespace lunasvg

#endif // SVGSTYLEDELEMENT_H
