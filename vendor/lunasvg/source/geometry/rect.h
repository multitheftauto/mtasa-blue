#ifndef RECT_H
#define RECT_H

namespace lunasvg {

class Rect
{
public:
    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(double _x, double _y, double _w, double _h) : x(_x), y(_y), width(_w), height(_h) {}

public:
    double x;
    double y;
    double width;
    double height;
};

} // namespace lunasvg

#endif // RECT_H
