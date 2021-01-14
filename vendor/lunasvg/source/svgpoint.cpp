#include "svgpoint.h"

namespace lunasvg {

SVGPoint::SVGPoint()
{
}

void SVGPoint::setValueAsString(const std::string& value)
{
    m_value = Point();
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    Utils::parseNumber(ptr, m_value.x);
    Utils::skipWsComma(ptr);
    Utils::parseNumber(ptr, m_value.y);
}

std::string SVGPoint::valueAsString() const
{
    std::string out;
    out += Utils::toString(x());
    out += ' ';
    out += Utils::toString(y());

    return out;
}

SVGPropertyBase* SVGPoint::clone() const
{
    SVGPoint* property = new SVGPoint();
    property->m_value = m_value;

    return property;
}

SVGPointList::SVGPointList()
{
}

std::vector<Point> SVGPointList::values() const
{
    std::vector<Point> v(length());
    for(unsigned int i = 0;i < length();i++)
        v[i] = at(i)->value();
    return v;
}

void SVGPointList::setValueAsString(const std::string& value)
{
    clear();
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    double x;
    double y;
    while(*ptr)
    {
        if(!Utils::parseNumber(ptr, x)
                || !Utils::skipWsComma(ptr)
                || !Utils::parseNumber(ptr, y))
            return;
        SVGPoint* item = new SVGPoint;
        item->setValue(Point(x, y));
        appendItem(item);
        Utils::skipWsComma(ptr);
    }
}

SVGPropertyBase* SVGPointList::clone() const
{
    SVGPointList* property = new SVGPointList();
    baseClone(property);

    return property;
}

} // namespace lunasvg
