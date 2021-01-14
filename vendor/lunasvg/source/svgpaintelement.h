#ifndef SVGPAINTELEMENT_H
#define SVGPAINTELEMENT_H

#include "svgstyledelement.h"
#include "svgurireference.h"
#include "svgfittoviewbox.h"
#include "svgtransform.h"
#include "svglength.h"
#include "paint.h"

namespace lunasvg {

class SVGPatternElement;
class SVGStopElement;

struct GradientAttributes;
struct LinearGradientAttributes;
struct RadialGradientAttributes;
struct PatternAttributes;

class SVGPaintElement : public SVGStyledElement
{
public:
    SVGPaintElement(DOMElementID elementId, SVGDocument* document);
    bool isSVGPaintElement() const { return true; }
    virtual Paint getPaint(const RenderState& state) const = 0;
    virtual void render(RenderContext& context) const;
};

class SVGGradientElement : public SVGPaintElement,
        public SVGURIReference
{
public:
    SVGGradientElement(DOMElementID elementId, SVGDocument* document);
    bool isSVGGradientElement() const { return true; }
    const DOMSVGTransform& gradientTransform() const { return m_gradientTransform; }
    const DOMSVGEnumeration<SpreadMethod>& spreadMethod() const { return m_spreadMethod; }
    const DOMSVGEnumeration<UnitType>& gradientUnits() const { return m_gradientUnits; }
    void setGradientAttributes(GradientAttributes& attributes) const;
    GradientStops buildGradientStops(const std::vector<const SVGStopElement*>& gradientStops) const;

private:
    DOMSVGTransform m_gradientTransform;
    DOMSVGEnumeration<SpreadMethod> m_spreadMethod;
    DOMSVGEnumeration<UnitType> m_gradientUnits;
};

class SVGLinearGradientElement : public SVGGradientElement
{
public:
    SVGLinearGradientElement(SVGDocument* document);
    const DOMSVGLength& x1() const { return m_x1; }
    const DOMSVGLength& y1() const { return m_y1; }
    const DOMSVGLength& x2() const { return m_x2; }
    const DOMSVGLength& y2() const { return m_y2; }
    void collectGradientAttributes(LinearGradientAttributes& attributes) const;
    Paint getPaint(const RenderState& state) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_x1;
    DOMSVGLength m_y1;
    DOMSVGLength m_x2;
    DOMSVGLength m_y2;
};

class SVGRadialGradientElement : public SVGGradientElement
{
public:
    SVGRadialGradientElement(SVGDocument* document);
    const DOMSVGLength& cx() const { return m_cx; }
    const DOMSVGLength& cy() const { return m_cy; }
    const DOMSVGLength& r() const { return m_r; }
    const DOMSVGLength& fx() const { return m_fx; }
    const DOMSVGLength& fy() const { return m_fy; }
    void collectGradientAttributes(RadialGradientAttributes& attributes) const;
    Paint getPaint(const RenderState& state) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_cx;
    DOMSVGLength m_cy;
    DOMSVGLength m_r;
    DOMSVGLength m_fx;
    DOMSVGLength m_fy;
};

class SVGPatternElement : public SVGPaintElement,
        public SVGURIReference,
        public SVGFitToViewBox
{
public:
    SVGPatternElement(SVGDocument* document);
    const DOMSVGLength& x() const { return m_x; }
    const DOMSVGLength& y() const { return m_y; }
    const DOMSVGLength& width() const { return m_width; }
    const DOMSVGLength& height() const { return m_height; }
    const DOMSVGTransform& patternTransform() const { return m_patternTransform; }
    const DOMSVGEnumeration<UnitType>& patternUnits() const { return m_patternUnits; }
    const DOMSVGEnumeration<UnitType>& patternContentUnits() const { return m_patternContentUnits; }
    void collectPatternAttributes(PatternAttributes& attributes) const;
    Paint getPaint(const RenderState& state) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_x;
    DOMSVGLength m_y;
    DOMSVGLength m_width;
    DOMSVGLength m_height;
    DOMSVGTransform m_patternTransform;
    DOMSVGEnumeration<UnitType> m_patternUnits;
    DOMSVGEnumeration<UnitType> m_patternContentUnits;
};

class SVGSolidColorElement : public SVGPaintElement
{
public:
    SVGSolidColorElement(SVGDocument* document);
    Paint getPaint(const RenderState&) const;
    SVGElementImpl* clone(SVGDocument* document) const;
};

struct GradientAttributes
{
    const SVGTransform* gradientTransform{nullptr};
    const SVGEnumeration<SpreadMethod>* spreadMethod{nullptr};
    const SVGEnumeration<UnitType>* gradientUnits{nullptr};
    std::vector<const SVGStopElement*> gradientStops;
};

struct LinearGradientAttributes : public GradientAttributes
{
    const SVGLength* x1{nullptr};
    const SVGLength* y1{nullptr};
    const SVGLength* x2{nullptr};
    const SVGLength* y2{nullptr};
};

struct RadialGradientAttributes : public GradientAttributes
{
    const SVGLength* cx{nullptr};
    const SVGLength* cy{nullptr};
    const SVGLength* r{nullptr};
    const SVGLength* fx{nullptr};
    const SVGLength* fy{nullptr};
};

struct PatternAttributes
{
    const SVGLength* x{nullptr};
    const SVGLength* y{nullptr};
    const SVGLength* width{nullptr};
    const SVGLength* height{nullptr};
    const SVGTransform* patternTransform{nullptr};
    const SVGEnumeration<UnitType>* patternUnits{nullptr};
    const SVGEnumeration<UnitType>* patternContentUnits{nullptr};
    const SVGRect* viewBox{nullptr};
    const SVGPreserveAspectRatio* preserveAspectRatio{nullptr};
    const SVGPatternElement* patternContentElement{nullptr};
};

} // namespace lunasvg

#endif // SVGPAINTELEMENT_H
