#include "svgstyledelement.h"
#include "svgclippathelement.h"
#include "svgmaskelement.h"
#include "svgdocumentimpl.h"
#include "svgcolor.h"
#include "rendercontext.h"

namespace lunasvg {

SVGStyledElement::SVGStyledElement(DOMElementID elementId, SVGDocument* document)
    : SVGElementHead(elementId, document),
      m_style(DOMPropertyIdStyle)
{
    addToPropertyMap(m_style);
}

void SVGStyledElement::setAttribute(const std::string& name, const std::string& value)
{
    CSSPropertyID nameId = Utils::cssPropertyId(name);
    if(nameId!=CSSPropertyIdUnknown)
    {
        m_style.setItemAsString(nameId, value);
        return;
    }

    SVGElementHead::setAttribute(name, value);
}

std::string SVGStyledElement::getAttribute(const std::string& name) const
{
    CSSPropertyID nameId = Utils::cssPropertyId(name);
    if(nameId!=CSSPropertyIdUnknown)
        return m_style.itemAsString(nameId);

    return SVGElementHead::getAttribute(name);
}

bool SVGStyledElement::hasAttribute(const std::string& name) const
{
    CSSPropertyID nameId = Utils::cssPropertyId(name);
    if(nameId!=CSSPropertyIdUnknown)
        return m_style.hasItem(nameId);

    return SVGElementHead::hasAttribute(name);
}

void SVGStyledElement::removeAttribute(const std::string& name)
{
    CSSPropertyID nameId = Utils::cssPropertyId(name);
    if(nameId!=CSSPropertyIdUnknown)
    {
        m_style.removeItem(nameId);
        return;
    }

    SVGElementHead::removeAttribute(name);
}

void SVGStyledElement::render(RenderContext& context) const
{
    context.push();
    RenderState& state = context.state();
    state.style.add(style());
    if(context.mode() == RenderModeBounding)
    {
        state.style.clear(CSSPropertyIdClip_Path);
        state.style.clear(CSSPropertyIdOpacity);
        state.style.clear(CSSPropertyIdMask);
    }
    else if(context.mode() == RenderModeClipping)
    {
        state.style.clear(CSSPropertyIdOpacity);
        state.style.clear(CSSPropertyIdMask);
    }

    if(state.style.requiresCompositing(this))
        state.canvas.reset(state.canvas.width(), state.canvas.height());

    if(const SVGPropertyBase* property = state.style.get(CSSPropertyIdColor))
    {
        const SVGColor* color = to<SVGColor>(property);
        if(color->colorType() != ColorTypeCurrentColor)
            state.color = color->value();
    }
}

void SVGStyledElement::renderTail(RenderContext& context) const
{
    RenderState& state = context.state();
    RenderState& newState = context.parent();
    if(state.canvas.impl() != newState.canvas.impl())
    {
        if(const SVGClipPathElement* clipPath = state.style.clipPath(document()))
            clipPath->applyClip(state);
        if(const SVGMaskElement* mask = state.style.mask(document()))
            mask->applyMask(state);

        double opacity = (isSVGGeometryElement() || elementId()==DOMElementIdText) ? 1.0 : state.style.opacity();
        newState.canvas.blend(state.canvas, BlendModeSrc_Over, opacity, 0.0, 0.0);
    }

    context.pop();
}

Rgb SVGStyledElement::currentColor() const
{
    if(style().isSpecified())
    {
        const CSSPropertyBase* item = style().property()->getItem(CSSPropertyIdColor);
        if(item && !item->isInherited())
        {
            const SVGColor* color = to<SVGColor>(item->property());
            if(color->colorType() != ColorTypeCurrentColor)
                return color->value();
        }
    }

    return !isSVGRootElement() ? to<SVGStyledElement>(parent)->currentColor() : KRgbBlack;
}

SVGPropertyBase* SVGStyledElement::findInheritedProperty(CSSPropertyID nameId) const
{
    if(style().isSpecified())
    {
        const CSSPropertyBase* item = style().property()->getItem(nameId);
        if(item && !item->isInherited())
            return item->property();
    }

    return !isSVGRootElement() ? to<SVGStyledElement>(parent)->findInheritedProperty(nameId) : nullptr;
}

} // namespace lunasvg
