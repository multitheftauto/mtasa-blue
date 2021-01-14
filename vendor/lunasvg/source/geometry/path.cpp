#include "path.h"
#include "pathiterator.h"
#include "affinetransform.h"

#include <limits>
#include <algorithm>
#include <cmath>
#include <memory>

namespace lunasvg {

#define BEZIER_ARC_FACTOR 0.5522847498
#define PI 3.14159265358979323846

Path::Path()
{
}

void Path::moveTo(double x, double y, bool rel)
{
    if(rel)
    {
        Point p = currentPoint();
        x += p.x;
        y += p.y;
    }

    m_segments.push_back(SegTypeMoveTo);
    m_coordinates.emplace_back(x, y);

    m_startPoint = Point(x, y);
}

void Path::lineTo(double x, double y, bool rel)
{
    if(rel)
    {
        Point p = currentPoint();
        x += p.x;
        y += p.y;
    }

    m_segments.push_back(SegTypeLineTo);
    m_coordinates.emplace_back(x, y);
}

void Path::quadTo(double x1, double y1, double x2, double y2, bool rel)
{
    if(rel)
    {
        Point p = currentPoint();
        x1 += p.x;
        y1 += p.y;
        x2 += p.x;
        y2 += p.y;
    }

    m_segments.push_back(SegTypeQuadTo);
    m_coordinates.emplace_back(x1, y1);
    m_coordinates.emplace_back(x2, y2);
}

void Path::cubicTo(double x1, double y1, double x2, double y2, double x3, double y3, bool rel)
{
    if(rel)
    {
        Point p = currentPoint();
        x1 += p.x;
        y1 += p.y;
        x2 += p.x;
        y2 += p.y;
        x3 += p.x;
        y3 += p.y;
    }

    m_segments.push_back(SegTypeCubicTo);
    m_coordinates.emplace_back(x1, y1);
    m_coordinates.emplace_back(x2, y2);
    m_coordinates.emplace_back(x3, y3);
}

void Path::arcTo(double rx, double ry, double xAxisRotation, bool largeArcFlag, bool sweepFlag, double x, double y, bool rel)
{
    Point cp = currentPoint();
    if(rel)
    {
        x += cp.x;
        y += cp.y;
    }

    if(rx == 0.0 || ry == 0.0)
    {
        lineTo(x, y);
        return;
    }

    double sin_th = std::sin(xAxisRotation * PI / 180.0);
    double cos_th = std::cos(xAxisRotation * PI / 180.0);

    double dx2 = (cp.x - x) / 2.0;
    double dy2 = (cp.y - y) / 2.0;

    if(dx2 == 0.0 && dy2 == 0.0)
        return;

    double x1 = (cos_th * dx2 + sin_th * dy2);
    double y1 = (-sin_th * dx2 + cos_th * dy2);

    double rx_sq = rx * rx;
    double ry_sq = ry * ry;
    double x1_sq = x1 * x1;
    double y1_sq = y1 * y1;

    double check = x1_sq / rx_sq + y1_sq / ry_sq;
    if(check > 0.99999)
    {
        double scale = std::sqrt(check) * 1.00001;
        rx = scale * rx;
        ry = scale * ry;
        rx_sq = rx * rx;
        ry_sq = ry * ry;
    }

    double sign = (largeArcFlag == sweepFlag) ? -1 : 1;
    double sq = ((rx_sq * ry_sq) - (rx_sq * y1_sq) - (ry_sq * x1_sq)) / ((rx_sq * y1_sq) + (ry_sq * x1_sq));
    sq = (sq < 0) ? 0 : sq;
    double coef = (sign * std::sqrt(sq));
    double cx1 = coef * ((rx * y1) / ry);
    double cy1 = coef * -((ry * x1) / rx);

    double sx2 = (cp.x + x) / 2.0;
    double sy2 = (cp.y + y) / 2.0;
    double cx = sx2 + (cos_th * cx1 - sin_th * cy1);
    double cy = sy2 + (sin_th * cx1 + cos_th * cy1);

    double ux = (x1 - cx1) / rx;
    double uy = (y1 - cy1) / ry;
    double vx = (-x1 - cx1) / rx;
    double vy = (-y1 - cy1) / ry;
    double p, n;

    n = std::sqrt((ux * ux) + (uy * uy));
    p = ux;
    sign = (uy < 0) ? -1.0 : 1.0;
    double angleStart = sign * std::acos(p / n);

    n = std::sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
    p = ux * vx + uy * vy;
    sign = (ux * vy - uy * vx < 0) ? -1.0 : 1.0;
    double angleExtent = sign * ((p / n < -1.0) ? PI : (p / n > 1.0) ? 0 : std::acos(p / n));
    if(!sweepFlag && angleExtent > 0.0)
    {
        angleExtent -= PI * 2.0;
    }
    else if(sweepFlag && angleExtent < 0.0)
    {
        angleExtent += PI * 2.0;
    }

    int numSegments = int(std::ceil(std::abs(angleExtent) * 2.0 / PI));
    double angleIncrement = angleExtent / numSegments;
    double controlLength = 4.0 / 3.0 * std::sin(angleIncrement / 2.0) / (1.0 + std::cos(angleIncrement / 2.0));
    std::unique_ptr<double[]> coords(new double[numSegments * 6]);
    std::size_t pos = 0;
    for(int i = 0;i < numSegments;i++)
    {
        double angle = angleStart + i * angleIncrement;
        double dx = std::cos(angle);
        double dy = std::sin(angle);

        coords[pos++] = dx - controlLength * dy;
        coords[pos++] = dy + controlLength * dx;

        angle += angleIncrement;
        dx = std::cos(angle);
        dy = std::sin(angle);

        coords[pos++] = dx + controlLength * dy;
        coords[pos++] = dy - controlLength * dx;
        coords[pos++] = dx;
        coords[pos++] = dy;
    }

    AffineTransform m;
    m *= AffineTransform::fromScale(rx, ry);
    m *= AffineTransform::fromRotate(xAxisRotation * PI / 180.0);
    m *= AffineTransform::fromTranslate(cx, cy);
    m.map(coords.get(), coords.get(), int(pos));

    coords[pos - 2] = x;
    coords[pos - 1] = y;

    for(std::size_t i = 0;i < pos;i+=6)
        cubicTo(coords[i], coords[i+1], coords[i+2], coords[i+3], coords[i+4], coords[i+5]);
}

void Path::smoothQuadTo(double x2, double y2, bool rel)
{
    if(rel)
    {
        Point p = currentPoint();
        x2 += p.x;
        y2 += p.y;
    }

    Point p1;
    std::size_t count = m_segments.size();
    if(count > 0 && m_segments[count - 1] == SegTypeQuadTo)
        p1 = controlPoint();
    else
        p1 = currentPoint();

    quadTo(p1.x, p1.y, x2, y2, false);
}

void Path::smoothCubicTo(double x2, double y2, double x3, double y3, bool rel)
{
    if(rel)
    {
        Point p = currentPoint();
        x2 += p.x;
        y2 += p.y;
        x3 += p.x;
        y3 += p.y;
    }

    Point p1;
    std::size_t count = m_segments.size();
    if(count > 0 && m_segments[count - 1] == SegTypeCubicTo)
        p1 = controlPoint();
    else
        p1 = currentPoint();

    cubicTo(p1.x, p1.y, x2, y2, x3, y3, false);
}

void Path::horizontalTo(double x, bool rel)
{
    Point p = currentPoint();

    if(rel)
        x += p.x;

    m_segments.push_back(SegTypeLineTo);
    m_coordinates.emplace_back(x, p.y);
}

void Path::verticalTo(double y, bool rel)
{
    Point p = currentPoint();

    if(rel)
        y += p.y;

    m_segments.push_back(SegTypeLineTo);
    m_coordinates.emplace_back(p.x, y);
}

void Path::closePath()
{
    std::size_t count = m_segments.size();
    if(count > 0 && m_segments[count - 1] != SegTypeClose)
        m_segments.push_back(SegTypeClose);
}

Point Path::currentPoint() const
{
    if(m_coordinates.empty())
        return Point();

    if(m_segments[m_segments.size() - 1] == SegTypeClose)
        return m_startPoint;

    return m_coordinates[m_coordinates.size() - 1];
}

Point Path::controlPoint() const
{
    if(m_coordinates.size() < 2)
        return Point();

    std::size_t count = m_coordinates.size();
    const Point& p1 = m_coordinates[count - 2];
    const Point& p2 = m_coordinates[count - 1];
    return Point(p2.x + p2.x - p1.x, p2.y + p2.y - p1.y);
}

void Path::addPath(const Path& path, const AffineTransform& matrix)
{
    PathIterator it(path);
    double c[6];
    while(!it.isDone())
    {
        switch(it.currentSegment(c))
        {
        case SegTypeMoveTo:
            matrix.map(c, c, 2);
            moveTo(c[0], c[1]);
            break;
        case SegTypeLineTo:
            matrix.map(c, c, 2);
            lineTo(c[0], c[1]);
            break;
        case SegTypeQuadTo:
            matrix.map(c, c, 4);
            quadTo(c[0], c[1], c[2], c[3]);
            break;
        case SegTypeCubicTo:
            matrix.map(c, c, 6);
            cubicTo(c[0], c[1], c[2], c[3], c[4], c[5]);
            break;
        case SegTypeClose:
            closePath();
            break;
        }
        it.next();
    }
}

void Path::addPoints(const std::vector<Point>& points)
{
    if(points.empty())
        return;

    moveTo(points[0].x, points[0].y);
    for(std::size_t i = 1;i < points.size();i++)
        lineTo(points[i].x, points[i].y);
}

void Path::addEllipse(double cx, double cy, double rx, double ry)
{
    double left = cx - rx;
    double top = cy - ry;
    double right = cx + rx;
    double bottom = cy + ry;

    double cpx = rx * BEZIER_ARC_FACTOR;
    double cpy = ry * BEZIER_ARC_FACTOR;

    moveTo(cx, top);
    cubicTo(cx+cpx, top, right, cy-cpy, right, cy);
    cubicTo(right, cy+cpy, cx+cpx, bottom, cx, bottom);
    cubicTo(cx-cpx, bottom, left, cy+cpy, left, cy);
    cubicTo(left, cy-cpy, cx-cpx, top, cx, top);
    closePath();
}

void Path::addRect(double x, double y, double w, double h, double rx, double ry)
{
    rx = std::min(rx, w * 0.5);
    ry = std::min(ry, h * 0.5);

    double right = x + w;
    double bottom = y + h;

    if(rx==0.0 && ry==0.0)
    {
        moveTo(x, y);
        lineTo(right, y);
        lineTo(right, bottom);
        lineTo(x, bottom);
        lineTo(x, y);
    }
    else
    {
        double cpx = rx * BEZIER_ARC_FACTOR;
        double cpy = ry * BEZIER_ARC_FACTOR;
        moveTo(x, y+ry);
        cubicTo(x, y+ry-cpy, x+rx-cpx, y, x+rx, y);
        lineTo(right-rx, y);
        cubicTo(right-rx+cpx, y, right, y+ry-cpy, right, y+ry);
        lineTo(right, bottom-ry);
        cubicTo(right, bottom-ry+cpy, right-rx+cpx, bottom, right-rx, bottom);
        lineTo(x+rx, bottom);
        cubicTo(x+rx-cpx, bottom, x, bottom-ry+cpy, x, bottom-ry);
        lineTo(x, y+ry);
    }

    closePath();
}

Rect Path::boundingBox() const
{
    double xMin = std::numeric_limits<double>::max();
    double yMin = std::numeric_limits<double>::max();
    double xMax = std::numeric_limits<double>::min();
    double yMax = std::numeric_limits<double>::min();

    for(std::size_t i = 0;i < m_coordinates.size();i++)
    {
        const Point& p = m_coordinates[i];
        if(p.x < xMin) xMin = p.x;
        if(p.x > xMax) xMax = p.x;
        if(p.y < yMin) yMin = p.y;
        if(p.y > yMax) yMax = p.y;
    }

    return Rect(xMin, yMin, xMax - xMin, yMax - yMin);
}

void Path::transform(const AffineTransform& matrix)
{
    for(std::size_t i = 0;i < m_coordinates.size();i++)
    {
        Point& p = m_coordinates[i];
        matrix.map(p.x, p.y, p.x, p.y);
    }
}

void Path::reset()
{
    m_startPoint = Point();
    m_segments.clear();
    m_coordinates.clear();
}

} //namespace lunasvg
