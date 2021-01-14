#ifndef SVGANGLE_H
#define SVGANGLE_H

#include "svgproperty.h"

namespace lunasvg {

enum MarkerOrientType
{
    MarkerOrientTypeUnknown,
    MarkerOrientTypeAuto,
    MarkerOrientTypeAngle
};

class SVGAngle : public SVGPropertyBase
{
public:
    SVGAngle();

    void setValue(double value) { m_value = value; }
    void setOrientType(MarkerOrientType type) { m_orientType = type; }
    double value() const { return m_value; }
    MarkerOrientType orientType() const { return m_orientType; }

    void setValueAsString(const std::string& value);
    std::string valueAsString() const;
    SVGPropertyBase* clone() const;

private:
    double m_value;
    MarkerOrientType m_orientType;
};

typedef DOMSVGProperty<SVGAngle> DOMSVGAngle;

} // namespace lunasvg

#endif // SVGANGLE_H
