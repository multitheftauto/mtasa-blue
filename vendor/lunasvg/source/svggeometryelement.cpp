#include "svggeometryelement.h"
#include "svgmarkerelement.h"
#include "pathiterator.h"
#include "paint.h"
#include "strokedata.h"

namespace lunasvg {

#define PI 3.14159265358979323846

SVGGeometryElement::SVGGeometryElement(DOMElementID elementId, SVGDocument* document)
    : SVGGraphicsElement(elementId, document)
{
}

void SVGGeometryElement::render(RenderContext& context) const
{
    if(style().isDisplayNone())
    {
        context.skipElement();
        return;
    }

    SVGGraphicsElement::render(context);
    RenderState& state = context.state();
    if(state.style.isHidden())
        return;

    state.bbox = makeBoundingBox(state);
    if(state.style.hasStroke())
    {
        double strokeWidth = state.style.strokeWidth(state);
        state.bbox.x -= strokeWidth * 0.5;
        state.bbox.y -= strokeWidth * 0.5;
        state.bbox.width += strokeWidth;
        state.bbox.height += strokeWidth;
    }

    if(context.mode() == RenderModeBounding)
        return;

    Path path = makePath(state);
    if(context.mode() == RenderModeClipping)
    {
        state.canvas.draw(path, state.matrix, state.style.clipRule(), KRgbBlack, Paint(), StrokeData());
        return;
    }

    StrokeData strokeData = state.style.strokeData(state);
    Paint fillPaint = state.style.fillPaint(state);
    Paint strokePaint = state.style.strokePaint(state);
    WindRule fillRule = state.style.fillRule();

    fillPaint.opacity = state.style.fillOpacity() * state.style.opacity();
    strokePaint.opacity = state.style.strokeOpacity() * state.style.opacity();

    state.canvas.draw(path, state.matrix, fillRule, fillPaint, strokePaint, strokeData);

    const SVGMarkerElement* markerStart = state.style.markerStart(document());
    const SVGMarkerElement* markerMid = state.style.markerMid(document());
    const SVGMarkerElement* markerEnd = state.style.markerEnd(document());

    if(markerStart==nullptr && markerMid==nullptr && markerEnd==nullptr)
        return;

    Point origin;
    Point startPoint;
    Point inslopePoints[2];
    Point outslopePoints[2];

    int index = 0;
    PathIterator it(path);
    double c[6];
    while(!it.isDone())
    {
        switch(it.currentSegment(c))
        {
        case SegTypeMoveTo:
            startPoint = Point(c[0], c[1]);
            inslopePoints[0] = origin;
            inslopePoints[1] = Point(c[0], c[1]);
            origin = Point(c[0], c[1]);
            break;
        case SegTypeLineTo:
            inslopePoints[0] = origin;
            inslopePoints[1] = Point(c[0], c[1]);
            origin = Point(c[0], c[1]);
            break;
        case SegTypeQuadTo:
            origin = Point(c[2], c[3]);
            break;
        case SegTypeCubicTo:
            inslopePoints[0] = Point(c[2], c[3]);
            inslopePoints[1] = Point(c[4], c[5]);
            origin = Point(c[4], c[5]);
            break;
        case SegTypeClose:
            inslopePoints[0] = origin;
            inslopePoints[1] = Point(c[0], c[1]);
            origin = startPoint;
            startPoint = Point();
            break;
        }

        index += 1;
        it.next();

        if(!it.isDone() && (markerStart || markerMid))
        {
            it.currentSegment(c);
            outslopePoints[0] = origin;
            outslopePoints[1] = Point(c[0], c[1]);

            if(index == 1 && markerStart)
            {
                Point slope(outslopePoints[1].x - outslopePoints[0].x, outslopePoints[1].y - outslopePoints[0].y);
                double angle = 180.0 * std::atan2(slope.y, slope.x) / PI;

                markerStart->renderMarker(state, origin, angle);
            }

            if(index > 1 && markerMid)
            {
                Point inslope(inslopePoints[1].x - inslopePoints[0].x, inslopePoints[1].y - inslopePoints[0].y);
                Point outslope(outslopePoints[1].x - outslopePoints[0].x, outslopePoints[1].y - outslopePoints[0].y);
                double inangle = 180.0 * std::atan2(inslope.y, inslope.x) / PI;
                double outangle = 180.0 * std::atan2(outslope.y, outslope.x) / PI;

                markerMid->renderMarker(state, origin, (inangle + outangle) * 0.5);
            }
        }

        if(it.isDone() && markerEnd)
        {
            Point slope(inslopePoints[1].x - inslopePoints[0].x, inslopePoints[1].y - inslopePoints[0].y);
            double angle = 180.0 * std::atan2(slope.y, slope.x) / PI;

            markerEnd->renderMarker(state, origin, angle);
        }
    }
}

SVGCircleElement::SVGCircleElement(SVGDocument* document)
    : SVGGeometryElement(DOMElementIdCircle, document),
      m_cx(DOMPropertyIdCx, LengthModeWidth, AllowNegativeLengths),
      m_cy(DOMPropertyIdCy, LengthModeHeight, AllowNegativeLengths),
      m_r(DOMPropertyIdR, LengthModeBoth, ForbidNegativeLengths)
{
    addToPropertyMap(m_cx);
    addToPropertyMap(m_cy);
    addToPropertyMap(m_r);
}

Path SVGCircleElement::makePath(const RenderState& state) const
{
    double _cx = m_cx.value(state);
    double _cy = m_cy.value(state);
    double _r = m_r.value(state);

    Path path;
    path.addEllipse(_cx, _cy, _r, _r);
    return path;
}

Rect SVGCircleElement::makeBoundingBox(const RenderState& state) const
{
    double _cx = m_cx.value(state);
    double _cy = m_cy.value(state);
    double _r = m_r.value(state);
    return Rect(_cx-_r, _cy-_r, _r+_r, _r+_r);
}

void SVGCircleElement::render(RenderContext& context) const
{
    if(m_r.isZero())
    {
        context.skipElement();
        return;
    }

    SVGGeometryElement::render(context);
}

SVGElementImpl* SVGCircleElement::clone(SVGDocument* document) const
{
    SVGCircleElement* e = new SVGCircleElement(document);
    baseClone(*e);
    return e;
}

SVGEllipseElement::SVGEllipseElement(SVGDocument* document)
    : SVGGeometryElement(DOMElementIdEllipse, document),
      m_cx(DOMPropertyIdCx, LengthModeWidth, AllowNegativeLengths),
      m_cy(DOMPropertyIdCy, LengthModeHeight, AllowNegativeLengths),
      m_rx(DOMPropertyIdRx, LengthModeWidth, ForbidNegativeLengths),
      m_ry(DOMPropertyIdRy, LengthModeHeight, ForbidNegativeLengths)
{
    addToPropertyMap(m_cx);
    addToPropertyMap(m_cy);
    addToPropertyMap(m_rx);
    addToPropertyMap(m_ry);
}

Path SVGEllipseElement::makePath(const RenderState& state) const
{
    double _cx = m_cx.value(state);
    double _cy = m_cy.value(state);
    double _rx = m_rx.value(state);
    double _ry = m_ry.value(state);

    Path path;
    path.addEllipse(_cx, _cy, _rx, _ry);
    return path;
}

Rect SVGEllipseElement::makeBoundingBox(const RenderState& state) const
{
    double _cx = m_cx.value(state);
    double _cy = m_cy.value(state);
    double _rx = m_rx.value(state);
    double _ry = m_ry.value(state);
    return Rect(_cx-_rx, _cy-_ry, _rx+_rx, _ry+_ry);
}

void SVGEllipseElement::render(RenderContext& context) const
{
    if(m_rx.isZero() || m_ry.isZero())
    {
        context.skipElement();
        return;
    }

    SVGGeometryElement::render(context);
}

SVGElementImpl* SVGEllipseElement::clone(SVGDocument* document) const
{
    SVGEllipseElement* e = new SVGEllipseElement(document);
    baseClone(*e);
    return e;
}

SVGLineElement::SVGLineElement(SVGDocument* document)
    : SVGGeometryElement(DOMElementIdLine, document),
      m_x1(DOMPropertyIdX1, LengthModeWidth, AllowNegativeLengths),
      m_y1(DOMPropertyIdY1, LengthModeHeight, AllowNegativeLengths),
      m_x2(DOMPropertyIdX2, LengthModeWidth, AllowNegativeLengths),
      m_y2(DOMPropertyIdY2, LengthModeHeight, AllowNegativeLengths)
{
    addToPropertyMap(m_x1);
    addToPropertyMap(m_y1);
    addToPropertyMap(m_x2);
    addToPropertyMap(m_y2);
}

Path SVGLineElement::makePath(const RenderState& state) const
{
    double _x1 = m_x1.value(state);
    double _y1 = m_y1.value(state);
    double _x2 = m_x2.value(state);
    double _y2 = m_y2.value(state);

    Path path;
    path.moveTo(_x1, _y1);
    path.lineTo(_x2, _y2);
    return path;
}

Rect SVGLineElement::makeBoundingBox(const RenderState& state) const
{
    double _x1 = m_x1.value(state);
    double _y1 = m_y1.value(state);
    double _x2 = m_x2.value(state);
    double _y2 = m_y2.value(state);

    Rect bbox;
    bbox.x = std::min(_x1, _x2);
    bbox.y = std::min(_y1, _y2);
    bbox.width = std::abs(_x2 - _x1);
    bbox.height = std::abs(_y2 - _y1);
    return bbox;
}

SVGElementImpl* SVGLineElement::clone(SVGDocument* document) const
{
    SVGLineElement* e = new SVGLineElement(document);
    baseClone(*e);
    return e;
}

SVGPathElement::SVGPathElement(SVGDocument* document)
    : SVGGeometryElement(DOMElementIdPath, document),
      m_d(DOMPropertyIdD)
{
    addToPropertyMap(m_d);
}

Path SVGPathElement::makePath(const RenderState&) const
{
    return m_d.property()->value();
}

Rect SVGPathElement::makeBoundingBox(const RenderState&) const
{
    const Path& path = m_d.property()->value();
    return path.boundingBox();
}

void SVGPathElement::render(RenderContext& context) const
{
    if(!m_d.isSpecified())
    {
        context.skipElement();
        return;
    }

    SVGGeometryElement::render(context);
}

SVGElementImpl* SVGPathElement::clone(SVGDocument* document) const
{
    SVGPathElement* e = new SVGPathElement(document);
    baseClone(*e);
    return e;
}

SVGPolyElement::SVGPolyElement(DOMElementID elementId, SVGDocument* document)
    : SVGGeometryElement(elementId, document),
      m_points(DOMPropertyIdPoints)
{
    addToPropertyMap(m_points);
}

Rect SVGPolyElement::makeBoundingBox(const RenderState&) const
{
    double xMin = std::numeric_limits<double>::max();
    double yMin = std::numeric_limits<double>::max();
    double xMax = std::numeric_limits<double>::min();
    double yMax = std::numeric_limits<double>::min();

    const SVGPointList* points = m_points.property();
    for(unsigned int i = 0;i < points->length();i++)
    {
        const SVGPoint* p = points->at(i);
        if(p->x() < xMin) xMin = p->x();
        if(p->x() > xMax) xMax = p->x();
        if(p->y() < yMin) yMin = p->y();
        if(p->y() > yMax) yMax = p->y();
    }

    return Rect(xMin, yMin, xMax - xMin, yMax - yMin);
}

void SVGPolyElement::render(RenderContext& context) const
{
    if(!m_points.isSpecified())
    {
        context.skipElement();
        return;
    }

    SVGGeometryElement::render(context);
}

SVGPolygonElement::SVGPolygonElement(SVGDocument* document)
    : SVGPolyElement(DOMElementIdPolygon, document)
{
}

Path SVGPolygonElement::makePath(const RenderState&) const
{
    Path path;
    path.addPoints(points().property()->values());
    path.closePath();
    return path;
}

SVGElementImpl* SVGPolygonElement::clone(SVGDocument* document) const
{
    SVGPolygonElement* e = new SVGPolygonElement(document);
    baseClone(*e);
    return e;
}

SVGPolylineElement::SVGPolylineElement(SVGDocument* document)
    : SVGPolyElement(DOMElementIdPolyline, document)
{
}

Path SVGPolylineElement::makePath(const RenderState&) const
{
    Path path;
    path.addPoints(points().property()->values());
    return path;
}

SVGElementImpl* SVGPolylineElement::clone(SVGDocument* document) const
{
    SVGPolylineElement* e = new SVGPolylineElement(document);
    baseClone(*e);
    return e;
}

SVGRectElement::SVGRectElement(SVGDocument* document)
    : SVGGeometryElement(DOMElementIdRect, document),
      m_x(DOMPropertyIdX, LengthModeWidth, AllowNegativeLengths),
      m_y(DOMPropertyIdY, LengthModeHeight, AllowNegativeLengths),
      m_rx(DOMPropertyIdRx, LengthModeWidth, ForbidNegativeLengths),
      m_ry(DOMPropertyIdRy, LengthModeHeight, ForbidNegativeLengths),
      m_width(DOMPropertyIdWidth, LengthModeWidth, ForbidNegativeLengths),
      m_height(DOMPropertyIdHeight, LengthModeHeight, ForbidNegativeLengths)
{
    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_rx);
    addToPropertyMap(m_ry);
    addToPropertyMap(m_width);
    addToPropertyMap(m_height);
}

Path SVGRectElement::makePath(const RenderState& state) const
{
    double _x = m_x.value(state);
    double _y = m_y.value(state);
    double _w = m_width.value(state);
    double _h = m_height.value(state);

    double _rx, _ry;
    if(m_rx.isSpecified() && m_ry.isSpecified())
    {
        _rx = m_rx.value(state);
        _ry = m_ry.value(state);
    }
    else if(m_rx.isSpecified())
    {
        _rx = _ry = m_rx.value(state);
    }
    else if(m_ry.isSpecified())
    {
        _rx = _ry = m_ry.value(state);
    }
    else
    {
        _rx = _ry = 0.0;
    }

    Path path;
    path.addRect(_x, _y, _w, _h, _rx, _ry);
    return path;
}

Rect SVGRectElement::makeBoundingBox(const RenderState& state) const
{
    double _x = m_x.value(state);
    double _y = m_y.value(state);
    double _w = m_width.value(state);
    double _h = m_height.value(state);
    return Rect(_x, _y, _w, _h);
}

void SVGRectElement::render(RenderContext& context) const
{
    if(m_width.isZero() || m_height.isZero())
    {
        context.skipElement();
        return;
    }

    SVGGeometryElement::render(context);
}

SVGElementImpl* SVGRectElement::clone(SVGDocument* document) const
{
    SVGRectElement* e = new SVGRectElement(document);
    baseClone(*e);
    return e;
}

} // namespace lunasvg
