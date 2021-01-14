#ifndef SVGGEOMETRYELEMENT_H
#define SVGGEOMETRYELEMENT_H

#include "svggraphicselement.h"
#include "svglength.h"
#include "svgpath.h"
#include "svgpoint.h"

namespace lunasvg {

class SVGGeometryElement : public SVGGraphicsElement
{
public:
    SVGGeometryElement(DOMElementID elementId, SVGDocument* document);
    bool isSVGGeometryElement() const { return true; }
    virtual Path makePath(const RenderState& state) const = 0;
    virtual Rect makeBoundingBox(const RenderState& state) const = 0;
    virtual void render(RenderContext& context) const;
};

class SVGCircleElement : public SVGGeometryElement
{
public:
    SVGCircleElement(SVGDocument* document);
    const DOMSVGLength& cx() const { return m_cx; }
    const DOMSVGLength& cy() const { return m_cy; }
    const DOMSVGLength& r() const { return m_r; }
    Path makePath(const RenderState& state) const;
    Rect makeBoundingBox(const RenderState& state) const;
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_cx;
    DOMSVGLength m_cy;
    DOMSVGLength m_r;
};

class SVGEllipseElement : public SVGGeometryElement
{
public:
    SVGEllipseElement(SVGDocument* document);
    const DOMSVGLength& cx() const { return m_cx; }
    const DOMSVGLength& cy() const { return m_cy; }
    const DOMSVGLength& rx() const { return m_rx; }
    const DOMSVGLength& ry() const { return m_ry; }
    Path makePath(const RenderState& state) const;
    Rect makeBoundingBox(const RenderState& state) const;
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_cx;
    DOMSVGLength m_cy;
    DOMSVGLength m_rx;
    DOMSVGLength m_ry;
};

class SVGLineElement : public SVGGeometryElement
{
public:
    SVGLineElement(SVGDocument* document);
    const DOMSVGLength& x1() const { return m_x1; }
    const DOMSVGLength& y1() const { return m_y1; }
    const DOMSVGLength& x2() const { return m_x2; }
    const DOMSVGLength& y2() const { return m_y2; }
    Path makePath(const RenderState& state) const;
    Rect makeBoundingBox(const RenderState& state) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_x1;
    DOMSVGLength m_y1;
    DOMSVGLength m_x2;
    DOMSVGLength m_y2;
};

class SVGPathElement : public SVGGeometryElement
{
public:
    SVGPathElement(SVGDocument* document);
    const DOMSVGPath& d() const { return m_d; }
    Path makePath(const RenderState&) const;
    Rect makeBoundingBox(const RenderState&) const;
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGPath m_d;
};

class SVGPolyElement : public SVGGeometryElement
{
public:
    SVGPolyElement(DOMElementID elementId, SVGDocument* document);
    bool isSVGPolyElement() const { return true; }
    const DOMSVGPointList& points() const { return m_points; }
    virtual Rect makeBoundingBox(const RenderState&) const;
    virtual void render(RenderContext& context) const;

private:
    DOMSVGPointList m_points;
};

class SVGPolygonElement : public SVGPolyElement
{
public:
    SVGPolygonElement(SVGDocument* document);
    Path makePath(const RenderState&) const;
    SVGElementImpl* clone(SVGDocument* document) const;
};

class SVGPolylineElement : public SVGPolyElement
{
public:
    SVGPolylineElement(SVGDocument* document);
    Path makePath(const RenderState&) const;
    SVGElementImpl* clone(SVGDocument* document) const;
};

class SVGRectElement : public SVGGeometryElement
{
public:
    SVGRectElement(SVGDocument* document);
    const DOMSVGLength& x() const { return m_x; }
    const DOMSVGLength& y() const { return m_y; }
    const DOMSVGLength& rx() const { return m_rx; }
    const DOMSVGLength& ry() const { return m_ry; }
    const DOMSVGLength& width() const { return m_width; }
    const DOMSVGLength& height() const { return m_height; }
    Path makePath(const RenderState& state) const;
    Rect makeBoundingBox(const RenderState& state) const;
    void render(RenderContext& context) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    DOMSVGLength m_x;
    DOMSVGLength m_y;
    DOMSVGLength m_rx;
    DOMSVGLength m_ry;
    DOMSVGLength m_width;
    DOMSVGLength m_height;
};

} // namespace lunasvg

#endif // SVGGEOMETRYELEMENT_H
