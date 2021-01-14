#ifndef SVGSTRING_H
#define SVGSTRING_H

#include "svgproperty.h"

namespace lunasvg {

class SVGString : public SVGPropertyBase
{
public:
    SVGString();

    void setValue(std::string value) { m_value = value; }
    const std::string& value() const { return  m_value; }

    virtual void setValueAsString(const std::string& value);
    virtual std::string valueAsString() const;
    virtual SVGPropertyBase* clone() const;

protected:
    std::string m_value;
};

class SVGURIString : public SVGString
{
public:
    SVGURIString();

    void setValueAsString(const std::string& value);
    std::string valueAsString() const;
    SVGPropertyBase* clone() const;
};

typedef DOMSVGProperty<SVGString> DOMSVGString;

class SVGStringList : public SVGListProperty<SVGString>
{
public:
    SVGStringList();

    void setValueAsString(const std::string& value);
    SVGPropertyBase* clone() const;
};

} // namespace lunasvg

#endif // SVGSTRING_H
