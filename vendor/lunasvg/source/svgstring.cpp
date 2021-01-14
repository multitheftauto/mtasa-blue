#include "svgstring.h"

namespace lunasvg {

SVGString::SVGString()
{
}

void SVGString::setValueAsString(const std::string& value)
{
    m_value.assign(value);
}

std::string SVGString::valueAsString() const
{
    return  m_value;
}

SVGPropertyBase* SVGString::clone() const
{
    SVGString* property = new SVGString();
    property->m_value = m_value;

    return property;
}

SVGURIString::SVGURIString()
{
}

void SVGURIString::setValueAsString(const std::string& value)
{
    m_value.clear();
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    if(!Utils::skipDesc(ptr, "url(", 4))
        return;

    const char* closeBracket = strchr(ptr, ')');
    if(closeBracket)
        m_value.assign(ptr, closeBracket);
}

std::string SVGURIString::valueAsString() const
{
    return "url(" + m_value + ")";
}

SVGPropertyBase* SVGURIString::clone() const
{
    SVGURIString* property = new SVGURIString();
    property->m_value = m_value;

    return property;
}

SVGStringList::SVGStringList()
{
}

void SVGStringList::setValueAsString(const std::string& value)
{
    clear();
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    const char delimiter = ' ';
    Utils::skipWs(ptr);
    while(*ptr)
    {
        const char* start = ptr;
        while(*ptr && *ptr!=delimiter)
            ++ptr;
        SVGString* item = new SVGString;
        item->setValue(std::string(start, ptr));
        appendItem(item);
        Utils::skipWsDelimiter(ptr, delimiter);
    }
}

SVGPropertyBase* SVGStringList::clone() const
{
    SVGStringList* property = new SVGStringList();
    baseClone(property);

    return property;
}

} // namespace lunasvg
