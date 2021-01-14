#ifndef SVGRECT_H
#define SVGRECT_H

#include "rect.h"
#include "svgproperty.h"

namespace lunasvg {

class SVGRect : public SVGPropertyBase
{
public:
    SVGRect();

    double x() const { return m_value.x; }
    double y() const { return m_value.y; }
    double width() const { return m_value.width; }
    double height() const { return m_value.height; }

    void setX(double x) { m_value.x = x; }
    void setY(double y) { m_value.y = y; }
    void setWidth(double w) { m_value.width = w; }
    void setHeight(double h) { m_value.height = h; }

    void setValue(const Rect& value) { m_value = value; }
    void setValid(bool valid) { m_valid = valid; }
    const Rect& value() const { return  m_value; }
    bool isValid() const { return  m_valid; }

    void setValueAsString(const std::string&);
    std::string valueAsString() const;
    SVGPropertyBase* clone() const;

private:
    Rect m_value;
    bool m_valid;
};

typedef DOMSVGProperty<SVGRect> DOMSVGRect;

class DOMSVGViewBoxRect : public DOMSVGRect
{
public:
    DOMSVGViewBoxRect(DOMPropertyID propertyId);
    void setPropertyAsString(const std::string& value);
};

} // namespace lunasvg

#endif // SVGRECT_H
