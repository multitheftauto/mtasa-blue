#ifndef SVGPOINT_H
#define SVGPOINT_H

#include "svgproperty.h"
#include "point.h"

namespace lunasvg {

class SVGPoint : public SVGPropertyBase
{
public:
    SVGPoint();

    double x() const { return m_value.x; }
    double y() const { return m_value.y; }
    void setX(double x) { m_value.x = x; }
    void setY(double y) { m_value.y = y; }

    void setValue(const Point& value) { m_value = value; }
    const Point& value() const { return  m_value; }

    void setValueAsString(const std::string& value);
    std::string valueAsString() const;
    SVGPropertyBase* clone() const;

private:
    Point m_value;
};

class SVGPointList : public SVGListProperty<SVGPoint>
{
public:
    SVGPointList();

    std::vector<Point> values() const;
    void setValueAsString(const std::string& value);
    SVGPropertyBase* clone() const;
};

typedef DOMSVGProperty<SVGPointList> DOMSVGPointList;

} // namespace lunasvg

#endif // SVGPOINT_H
