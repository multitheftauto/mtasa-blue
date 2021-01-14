#include "svgnumber.h"

namespace lunasvg {

SVGNumber::SVGNumber()
    : m_value(0)
{
}

void SVGNumber::setValueAsString(const std::string& value)
{
    m_value = 0;
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    Utils::parseNumber(ptr, m_value);
}

std::string SVGNumber::valueAsString() const
{
    return Utils::toString(m_value);
}

SVGPropertyBase* SVGNumber::clone() const
{
    SVGNumber* property = new SVGNumber();
    property->m_value = m_value;

    return property;
}

SVGNumberPercentage::SVGNumberPercentage()
{
}

void SVGNumberPercentage::setValueAsString(const std::string& value)
{
    m_value = 0;
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    if(!Utils::parseNumber(ptr, m_value))
        return;

    if(Utils::skipDesc(ptr, "%", 1))
        m_value /= 100.0;
    m_value = (m_value < 0.0) ? 0.0 : (m_value > 1.0) ? 1.0 : m_value;
}

SVGPropertyBase* SVGNumberPercentage::clone() const
{
    SVGNumberPercentage* property = new SVGNumberPercentage();
    property->m_value = m_value;

    return property;
}

SVGNumberList::SVGNumberList()
{
}

std::vector<double> SVGNumberList::values() const
{
    std::vector<double> v(length());
    for(unsigned int i = 0;i < length();i++)
        v[i] = at(i)->value();
    return v;
}

void SVGNumberList::setValueAsString(const std::string& value)
{
    clear();
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    double number;
    while(*ptr)
    {
        if(!Utils::parseNumber(ptr, number))
            return;
        SVGNumber* item = new SVGNumber;
        item->setValue(number);
        appendItem(item);
        Utils::skipWsComma(ptr);
    }
}

SVGPropertyBase* SVGNumberList::clone() const
{
    SVGNumberList* property = new SVGNumberList();
    baseClone(property);

    return property;
}

} // namespace lunasvg
