#include "svgangle.h"

namespace lunasvg {

#define PI 3.14159265358979323846

SVGAngle::SVGAngle()
    : m_value(0),
      m_orientType(MarkerOrientTypeAngle)
{
}

void SVGAngle::setValueAsString(const std::string& value)
{
    m_value = 0.0;
    m_orientType = MarkerOrientTypeAngle;
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    if(Utils::skipDesc(ptr, "auto", 4))
    {
        m_orientType = MarkerOrientTypeAuto;
        return;
    }

    if(!Utils::parseNumber(ptr, m_value))
        return;

    if(Utils::skipDesc(ptr, "rad", 3))
        m_value *= 180.0 / PI;
    else if(Utils::skipDesc(ptr, "grad", 4))
        m_value *= 360.0 / 400.0;
}

std::string SVGAngle::valueAsString() const
{
    return m_orientType == MarkerOrientTypeAuto ? std::string("auto") : Utils::toString(m_value);
}

SVGPropertyBase* SVGAngle::clone() const
{
    SVGAngle* property = new SVGAngle();
    property->m_value = m_value;
    property->m_orientType = m_orientType;

    return property;
}

} // namespace lunasvg
