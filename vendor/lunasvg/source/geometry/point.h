#ifndef POINT_H
#define POINT_H

namespace lunasvg {

class Point
{
public:
    Point() : x(0), y(0) {}
    Point(double _x, double _y) : x(_x), y(_y) {}

public:
    double x;
    double y;
};

} // namespace lunasvg

#endif // POINT_H
