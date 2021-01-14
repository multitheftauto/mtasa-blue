#ifndef PATH_H
#define PATH_H

#include "point.h"
#include "rect.h"

#include <vector>

namespace lunasvg {

enum SegType
{
    SegTypeMoveTo,
    SegTypeLineTo,
    SegTypeQuadTo,
    SegTypeCubicTo,
    SegTypeClose
};

class AffineTransform;
class Rect;

class Path
{
public:
    Path();

    void moveTo(double x1, double y1, bool rel = false);
    void lineTo(double x1, double y1, bool rel = false);
    void quadTo(double x1, double y1, double x2, double y2, bool rel = false);
    void cubicTo(double x1, double y1, double x2, double y2, double x3, double y3, bool rel = false);
    void arcTo(double rx, double ry, double xAxisRotation, bool largeArcFlag, bool sweepFlag, double x, double y, bool rel = false);
    void smoothQuadTo(double x2, double y2, bool rel = false);
    void smoothCubicTo(double x2, double y2, double x3, double y3, bool rel = false);
    void horizontalTo(double x, bool rel = false);
    void verticalTo(double y, bool rel = false);
    void closePath();

    void moveTo(const Point& p1, bool rel = false);
    void lineTo(const Point& p1, bool rel = false);
    void quadTo(const Point& p1, const Point& p2, bool rel = false);
    void cubicTo(const Point& p1, const Point& p2, const Point& p3, bool rel = false);
    void smoothQuadTo(const Point& p2, bool rel = false);
    void smoothCubicTo(const Point& p2, const Point& p3, bool rel = false);

    void addPath(const Path& path, const AffineTransform& matrix);
    void addPoints(const std::vector<Point>& points);
    void addEllipse(double cx, double cy, double rx, double ry);
    void addRect(double x, double y, double w, double h, double rx, double ry);

    bool isEmpty() const;
    void reset();
    void transform(const AffineTransform& matrix);
    Rect boundingBox() const;

    Point currentPoint() const;
    Point controlPoint() const;

    const Point& startPoint() const { return m_startPoint; }
    const std::vector<SegType>& segments() const { return m_segments; }
    const std::vector<Point>& coordinates() const { return m_coordinates; }

private:
    Point m_startPoint;
    std::vector<SegType> m_segments;
    std::vector<Point> m_coordinates;
};

inline bool Path::isEmpty() const
{
    return m_coordinates.empty();
}

inline void Path::moveTo(const Point& p1, bool rel)
{
    moveTo(p1.x, p1.y, rel);
}

inline void Path::lineTo(const Point& p1, bool rel)
{
    lineTo(p1.x, p1.y, rel);
}

inline void Path::quadTo(const Point& p1, const Point& p2, bool rel)
{
    quadTo(p1.x, p1.y, p2.x, p2.y, rel);
}

inline void Path::cubicTo(const Point& p1, const Point& p2, const Point& p3, bool rel)
{
    cubicTo(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, rel);
}

inline void Path::smoothQuadTo(const Point& p2, bool rel)
{
    smoothQuadTo(p2.x, p2.y, rel);
}

inline void Path::smoothCubicTo(const Point& p2, const Point& p3, bool rel)
{
    smoothCubicTo(p2.x, p2.y, p3.x, p3.y, rel);
}

} // namespace lunasvg

#endif //PATH_H
