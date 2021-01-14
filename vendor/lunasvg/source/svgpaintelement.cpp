#include "svgpaintelement.h"
#include "svgdocumentimpl.h"
#include "svgstopelement.h"
#include "svgcolor.h"
#include "rendercontext.h"

namespace lunasvg {

SVGPaintElement::SVGPaintElement(DOMElementID elementId, SVGDocument* document)
    : SVGStyledElement(elementId, document)
{
}

void SVGPaintElement::render(RenderContext& context) const
{
    context.skipElement();
}

SVGGradientElement::SVGGradientElement(DOMElementID elementId, SVGDocument* document)
    : SVGPaintElement(elementId, document),
      SVGURIReference (this),
      m_gradientTransform(DOMPropertyIdGradientTransform),
      m_spreadMethod(DOMPropertyIdSpreadMethod),
      m_gradientUnits(DOMPropertyIdGradientUnits)
{
    addToPropertyMap(m_gradientTransform);
    addToPropertyMap(m_spreadMethod);
    addToPropertyMap(m_gradientUnits);
}

void SVGGradientElement::setGradientAttributes(GradientAttributes& attributes) const
{
    if(!attributes.gradientTransform && m_gradientTransform.isSpecified())
        attributes.gradientTransform = m_gradientTransform.property();
    if(!attributes.spreadMethod && m_spreadMethod.isSpecified())
        attributes.spreadMethod = m_spreadMethod.property();
    if(!attributes.gradientUnits && m_gradientUnits.isSpecified())
        attributes.gradientUnits = m_gradientUnits.property();
    if(attributes.gradientStops.empty())
    {
        const SVGElementImpl* e = next;
        while(e != tail)
        {
            if(e->elementId()==DOMElementIdStop)
                attributes.gradientStops.push_back(to<SVGStopElement>(e));
            e = e->next;
        }
    }
}

GradientStops SVGGradientElement::buildGradientStops(const std::vector<const SVGStopElement*>& gradientStops) const
{
    GradientStops stops(gradientStops.size());
    double prevOffset = 0.0;
    for(unsigned int i = 0;i < gradientStops.size();i++)
    {
        const SVGStopElement* stop = gradientStops[i];
        double offset = std::min(std::max(prevOffset, stop->offsetValue()), 1.0);
        prevOffset = offset;
        stops[i] = GradientStop(offset, stop->stopColorWithOpacity());
    }

    return stops;
}

SVGLinearGradientElement::SVGLinearGradientElement(SVGDocument* document)
    : SVGGradientElement(DOMElementIdLinearGradient, document),
      m_x1(DOMPropertyIdX1, LengthModeWidth, AllowNegativeLengths),
      m_y1(DOMPropertyIdY1, LengthModeHeight, AllowNegativeLengths),
      m_x2(DOMPropertyIdX2, LengthModeWidth, AllowNegativeLengths),
      m_y2(DOMPropertyIdY2, LengthModeHeight, AllowNegativeLengths)
{
    m_x2.setDefaultValue(hundredPercent());

    addToPropertyMap(m_x1);
    addToPropertyMap(m_y1);
    addToPropertyMap(m_x2);
    addToPropertyMap(m_y2);
}

void SVGLinearGradientElement::collectGradientAttributes(LinearGradientAttributes& attributes) const
{
    std::set<const SVGGradientElement*> processedGradients;
    const SVGGradientElement* current = this;

    while(true)
    {
        current->setGradientAttributes(attributes);
        if(current->elementId() == DOMElementIdLinearGradient)
        {
            const SVGLinearGradientElement* linear = to<SVGLinearGradientElement>(current);
            if(!attributes.x1 && linear->m_x1.isSpecified())
                attributes.x1 = linear->m_x1.property();
            if(!attributes.y1 && linear->m_y1.isSpecified())
                attributes.y1 = linear->m_y1.property();
            if(!attributes.x2 && linear->m_x2.isSpecified())
                attributes.x2 = linear->m_x2.property();
            if(!attributes.y2 && linear->m_y2.isSpecified())
                attributes.y2 = linear->m_y2.property();
        }

        SVGElementImpl* ref = document()->impl()->resolveIRI(current->hrefValue());
        if(!ref || !ref->isSVGGradientElement())
            break;

        processedGradients.insert(current);
        current = to<SVGGradientElement>(ref);
        if(processedGradients.find(current)!=processedGradients.end())
            break;
    }
}

Paint SVGLinearGradientElement::getPaint(const RenderState& state) const
{
    LinearGradientAttributes attributes;
    collectGradientAttributes(attributes);
    if(attributes.gradientStops.empty())
        return Paint();

    double x1, y1, x2, y2;
    UnitType units = attributes.gradientUnits ? attributes.gradientUnits->enumValue() : UnitTypeObjectBoundingBox;
    if(units == UnitTypeObjectBoundingBox)
    {
        x1 = attributes.x1 ? attributes.x1->value(state, 1) : 0.0;
        y1 = attributes.y1 ? attributes.y1->value(state, 1) : 0.0;
        x2 = attributes.x2 ? attributes.x2->value(state, 1) : 1.0;
        y2 = attributes.y2 ? attributes.y2->value(state, 1) : 0.0;
    }
    else
    {
        x1 = attributes.x1 ? attributes.x1->valueX(state) : 0.0;
        y1 = attributes.y1 ? attributes.y1->valueY(state) : 0.0;
        x2 = attributes.x2 ? attributes.x2->valueX(state) : hundredPercent()->valueX(state);
        y2 = attributes.y2 ? attributes.y2->valueY(state) : 0.0;
    }

    GradientStops stops = buildGradientStops(attributes.gradientStops);
    if((x1 == x2 && y1 == y2) || stops.size() == 1)
        return stops.back().second;

    SpreadMethod spread = attributes.spreadMethod ? attributes.spreadMethod->enumValue() : SpreadMethodPad;
    AffineTransform matrix;
    if(units == UnitTypeObjectBoundingBox)
    {
        matrix.translate(state.bbox.x, state.bbox.y);
        matrix.scale(state.bbox.width, state.bbox.height);
    }

    if(attributes.gradientTransform)
        matrix.multiply(attributes.gradientTransform->value());

    LinearGradient values(x1, y1, x2, y2);
    Gradient gradient(values);
    gradient.stops = stops;
    gradient.spread = spread;
    gradient.matrix = matrix;

    return gradient;
}

SVGElementImpl* SVGLinearGradientElement::clone(SVGDocument* document) const
{
    SVGLinearGradientElement* e = new SVGLinearGradientElement(document);
    baseClone(*e);
    return e;
}

SVGRadialGradientElement::SVGRadialGradientElement(SVGDocument* document)
    : SVGGradientElement(DOMElementIdRadialGradient, document),
      m_cx(DOMPropertyIdCx, LengthModeWidth, AllowNegativeLengths),
      m_cy(DOMPropertyIdCy, LengthModeHeight, AllowNegativeLengths),
      m_r(DOMPropertyIdR, LengthModeBoth, ForbidNegativeLengths),
      m_fx(DOMPropertyIdFx, LengthModeWidth, AllowNegativeLengths),
      m_fy(DOMPropertyIdFy, LengthModeHeight, AllowNegativeLengths)
{
    m_cx.setDefaultValue(fiftyPercent());
    m_cy.setDefaultValue(fiftyPercent());
    m_r.setDefaultValue(fiftyPercent());

    addToPropertyMap(m_cx);
    addToPropertyMap(m_cy);
    addToPropertyMap(m_r);
    addToPropertyMap(m_fx);
    addToPropertyMap(m_fy);
}

void SVGRadialGradientElement::collectGradientAttributes(RadialGradientAttributes& attributes) const
{
    std::set<const SVGGradientElement*> processedGradients;
    const SVGGradientElement* current = this;

    while(true)
    {
        current->setGradientAttributes(attributes);
        if(current->elementId() == DOMElementIdRadialGradient)
        {
            const SVGRadialGradientElement* radial = to<SVGRadialGradientElement>(current);
            if(!attributes.cx && radial->m_cx.isSpecified())
                attributes.cx = radial->m_cx.property();
            if(!attributes.cy && radial->m_cy.isSpecified())
                attributes.cy = radial->m_cy.property();
            if(!attributes.r && radial->m_r.isSpecified())
                attributes.r = radial->m_r.property();
            if(!attributes.fx && radial->m_fx.isSpecified())
                attributes.fx = radial->m_fx.property();
            if(!attributes.fy && radial->m_fy.isSpecified())
                attributes.fy = radial->m_fy.property();
        }

        SVGElementImpl* ref = document()->impl()->resolveIRI(current->hrefValue());
        if(!ref || !ref->isSVGGradientElement())
            break;

        processedGradients.insert(current);
        current = to<SVGGradientElement>(ref);
        if(processedGradients.find(current)!=processedGradients.end())
            break;
    }
}

Paint SVGRadialGradientElement::getPaint(const RenderState& state) const
{
    RadialGradientAttributes attributes;
    collectGradientAttributes(attributes);
    if(attributes.gradientStops.empty())
        return Paint();

    double cx, cy, r, fx, fy;
    UnitType units = attributes.gradientUnits ? attributes.gradientUnits->enumValue() : UnitTypeObjectBoundingBox;
    if(units == UnitTypeObjectBoundingBox)
    {
        cx = attributes.cx ? attributes.cx->value(state, 1) : 0.5;
        cy = attributes.cy ? attributes.cy->value(state, 1) : 0.5;
        r = attributes.r ? attributes.r->value(state, 1) : 0.5;
        fx = attributes.fx ? attributes.fx->value(state, 1) : cx;
        fy = attributes.fy ? attributes.fy->value(state, 1) : cy;
    }
    else
    {
        cx = attributes.cx ? attributes.cx->valueX(state) : fiftyPercent()->valueX(state);
        cy = attributes.cy ? attributes.cy->valueY(state) : fiftyPercent()->valueY(state);
        r = attributes.r ? attributes.r->value(state) : fiftyPercent()->value(state);
        fx = attributes.fx ? attributes.fx->valueX(state) : cx;
        fy = attributes.fy ? attributes.fy->valueY(state) : cy;
    }

    GradientStops stops = buildGradientStops(attributes.gradientStops);
    if(r == 0.0 || stops.size() == 1)
        return stops.back().second;

    SpreadMethod spread = attributes.spreadMethod ? attributes.spreadMethod->enumValue() : SpreadMethodPad;
    AffineTransform matrix;
    if(units == UnitTypeObjectBoundingBox)
    {
        matrix.translate(state.bbox.x, state.bbox.y);
        matrix.scale(state.bbox.width, state.bbox.height);
    }

    if(attributes.gradientTransform)
        matrix.multiply(attributes.gradientTransform->value());

    RadialGradient values(cx, cy, r, fx, fy);
    Gradient gradient(values);
    gradient.stops = stops;
    gradient.spread = spread;
    gradient.matrix = matrix;

    return gradient;
}

SVGElementImpl* SVGRadialGradientElement::clone(SVGDocument* document) const
{
    SVGRadialGradientElement* e = new SVGRadialGradientElement(document);
    baseClone(*e);
    return e;
}

SVGPatternElement::SVGPatternElement(SVGDocument* document)
    : SVGPaintElement(DOMElementIdPattern, document),
      SVGURIReference(this),
      SVGFitToViewBox(this),
      m_x(DOMPropertyIdX, LengthModeWidth, AllowNegativeLengths),
      m_y(DOMPropertyIdY, LengthModeHeight, AllowNegativeLengths),
      m_width(DOMPropertyIdWidth, LengthModeWidth, ForbidNegativeLengths),
      m_height(DOMPropertyIdHeight, LengthModeHeight, ForbidNegativeLengths),
      m_patternTransform(DOMPropertyIdPatternTransform),
      m_patternUnits(DOMPropertyIdPatternUnits),
      m_patternContentUnits(DOMPropertyIdPatternContentUnits)
{
    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_width);
    addToPropertyMap(m_height);
    addToPropertyMap(m_patternTransform);
    addToPropertyMap(m_patternUnits);
    addToPropertyMap(m_patternContentUnits);
}

void SVGPatternElement::collectPatternAttributes(PatternAttributes& attributes) const
{
    std::set<const SVGPatternElement*> processedPatterns;
    const SVGPatternElement* current = this;

    while(true)
    {
        if(!attributes.x && current->x().isSpecified())
            attributes.x = current->x().property();
        if(!attributes.y && current->y().isSpecified())
            attributes.y = current->y().property();
        if(!attributes.width && current->width().isSpecified())
            attributes.width = current->width().property();
        if(!attributes.height && current->height().isSpecified())
            attributes.height = current->height().property();
        if(!attributes.patternTransform && current->patternTransform().isSpecified())
            attributes.patternTransform = current->patternTransform().property();
        if(!attributes.patternUnits && current->patternUnits().isSpecified())
            attributes.patternUnits = current->patternUnits().property();
        if(!attributes.patternContentUnits && current->patternContentUnits().isSpecified())
            attributes.patternContentUnits = current->patternContentUnits().property();
        if(!attributes.viewBox && current->viewBox().isSpecified())
            attributes.viewBox = current->viewBox().property();
        if(!attributes.preserveAspectRatio && current->preserveAspectRatio().isSpecified())
            attributes.preserveAspectRatio = current->preserveAspectRatio().property();
        if(!attributes.patternContentElement && current->next != current->tail)
            attributes.patternContentElement = current;

        SVGElementImpl* ref = document()->impl()->resolveIRI(current->hrefValue());
        if(!ref || ref->elementId() != DOMElementIdPattern)
            break;

        processedPatterns.insert(current);
        current = to<SVGPatternElement>(ref);
        if(processedPatterns.find(current)!=processedPatterns.end())
            break;
    }
}

Paint SVGPatternElement::getPaint(const RenderState& state) const
{
    PatternAttributes attributes;
    collectPatternAttributes(attributes);

    double x, y, w, h;
    UnitType units = attributes.patternUnits ? attributes.patternUnits->enumValue() : UnitTypeObjectBoundingBox;
    if(units == UnitTypeObjectBoundingBox)
    {
        x = attributes.x ? state.bbox.x + attributes.x->value(state, 1) * state.bbox.width : 0;
        y = attributes.y ? state.bbox.y + attributes.y->value(state, 1) * state.bbox.height : 0;
        w = attributes.width ? attributes.width->value(state, 1) * state.bbox.width : 0;
        h = attributes.height ? attributes.height->value(state, 1) * state.bbox.height : 0;
    }
    else
    {
        x = attributes.x ? attributes.x->valueX(state) : 0;
        y = attributes.y ? attributes.y->valueY(state) : 0;
        w = attributes.width ? attributes.width->valueX(state) : 0;
        h = attributes.height ? attributes.height->valueY(state) : 0;
    }

    AffineTransform transform(state.matrix);
    if(attributes.patternTransform)
        transform.multiply(attributes.patternTransform->value());

    const double* m = transform.getMatrix();
    double scalex = std::sqrt(m[0] * m[0] + m[2] * m[2]);
    double scaley = std::sqrt(m[1] * m[1] + m[3] * m[3]);

    double width = w * scalex;
    double height = h * scaley;

    if(width == 0.0 || height == 0.0 || attributes.patternContentElement == nullptr || RenderBreaker::hasElement(this))
        return Paint();

    RenderContext newContext(this, RenderModeDisplay);
    RenderState& newState = newContext.state();
    newState.element = this;
    newState.canvas.reset(std::uint32_t(std::ceil(width)), std::uint32_t(std::ceil(height)));
    newState.style.add(style());
    newState.matrix.scale(scalex, scaley);
    newState.viewPort = state.viewPort;
    newState.color = KRgbBlack;
    newState.dpi = state.dpi;

    if(attributes.viewBox)
    {
        const SVGPreserveAspectRatio* positioning = attributes.preserveAspectRatio ? attributes.preserveAspectRatio : SVGPreserveAspectRatio::defaultValue();
        newState.matrix.multiply(positioning->getMatrix(Rect(0, 0, w, h), attributes.viewBox->value()));
        newState.viewPort = attributes.viewBox->value();
    }
    else if(attributes.patternContentUnits && attributes.patternContentUnits->enumValue() == UnitTypeObjectBoundingBox)
    {
        newState.matrix.scale(state.bbox.width, state.bbox.height);
        newState.viewPort = Rect(0, 0, 1, 1);
    }

    RenderBreaker::registerElement(this);
    newContext.render(attributes.patternContentElement->next, attributes.patternContentElement->tail->prev);
    RenderBreaker::unregisterElement(this);

    AffineTransform matrix(1.0/scalex, 0, 0, 1.0/scaley, x, y);
    if(attributes.patternTransform)
        matrix.postmultiply(attributes.patternTransform->value());

    Texture texture;
    texture.type = TextureTypeTiled;
    texture.canvas = newState.canvas;
    texture.matrix = matrix;

    return texture;
}

SVGElementImpl* SVGPatternElement::clone(SVGDocument* document) const
{
    SVGPatternElement* e = new SVGPatternElement(document);
    baseClone(*e);
    return e;
}

SVGSolidColorElement::SVGSolidColorElement(SVGDocument* document)
    : SVGPaintElement(DOMElementIdSolidColor, document)
{
}

Paint SVGSolidColorElement::getPaint(const RenderState&) const
{
    if(!style().isSpecified())
        return KRgbBlack;

    Rgb color;
    if(const CSSPropertyBase* item = style().property()->getItem(CSSPropertyIdSolid_Color))
    {
        const SVGPropertyBase* property = !item->isInherited() ? item->property() : findInheritedProperty(CSSPropertyIdSolid_Color);
        if(property)
        {
            const SVGColor* solidColor = to<SVGColor>(property);
            color = solidColor->colorType() == ColorTypeCurrentColor ? currentColor() : solidColor->value();
        }
    }

    if(const CSSPropertyBase* item = style().property()->getItem(CSSPropertyIdSolid_Opacity))
    {
        const SVGPropertyBase* property = !item->isInherited() ? item->property() : findInheritedProperty(CSSPropertyIdSolid_Opacity);
        if(property)
        {
            const SVGNumber* solidOpacity = to<SVGNumber>(property);
            color.a = std::uint8_t(solidOpacity->value() * 255.0);
        }
    }

    return color;
}

SVGElementImpl* SVGSolidColorElement::clone(SVGDocument* document) const
{
    SVGSolidColorElement* e = new SVGSolidColorElement(document);
    baseClone(*e);
    return e;
}

} // namespace lunasvg
