#include "svgrect.h"

namespace lunasvg {

SVGRect::SVGRect()
    : m_valid(true)
{
}

void SVGRect::setValueAsString(const std::string& value)
{
    if(value.empty())
    {
        m_valid = true;
        m_value = Rect(0, 0, 0, 0);
        return;
    }

    double x = 0;
    double y = 0;
    double w = 0;
    double h = 0;

    const char* ptr = value.c_str();
    m_valid = Utils::skipWs(ptr)
            && Utils::parseNumber(ptr, x)
            && Utils::skipWsComma(ptr)
            && Utils::parseNumber(ptr, y)
            && Utils::skipWsComma(ptr)
            && Utils::parseNumber(ptr, w)
            && Utils::skipWsComma(ptr)
            && Utils::parseNumber(ptr, h)
            && !Utils::skipWs(ptr);

    m_value = m_valid ? Rect(x, y, w, h) :  Rect(0, 0, 0, 0);
}

std::string SVGRect::valueAsString() const
{
    std::string out;
    out += Utils::toString(x());
    out += ' ';
    out += Utils::toString(y());
    out += ' ';
    out += Utils::toString(width());
    out += ' ';
    out += Utils::toString(height());

    return out;
}

SVGPropertyBase* SVGRect::clone() const
{
    SVGRect* property = new SVGRect();
    property->m_value = m_value;
    property->m_valid = m_valid;

    return property;
}

DOMSVGViewBoxRect::DOMSVGViewBoxRect(DOMPropertyID propertyId)
    : DOMSVGRect(propertyId)
{
}

void DOMSVGViewBoxRect::setPropertyAsString(const std::string& value)
{
    DOMSVGRect::setPropertyAsString(value);

    if(property()->width() < 0 || property()->height() < 0)
    {
        property()->setValue(Rect());
        property()->setValid(false);
    }
}

} // namespace lunasvg
