#ifndef SVGNUMBER_H
#define SVGNUMBER_H

#include "svgproperty.h"

namespace lunasvg {

class SVGNumber : public SVGPropertyBase
{
public:
    SVGNumber();

    void setValue(double value) { m_value = value; }
    double value() const { return  m_value; }

    std::string valueAsString() const;
    virtual void setValueAsString(const std::string& value);
    virtual SVGPropertyBase* clone() const;

protected:
    double m_value;
};

class SVGNumberPercentage : public SVGNumber
{
public:
    SVGNumberPercentage();

    void setValueAsString(const std::string& value);
    SVGPropertyBase* clone() const;
};

class SVGNumberList : public SVGListProperty<SVGNumber>
{
public:
    SVGNumberList();

    std::vector<double> values() const;
    void setValueAsString(const std::string& value);
    SVGPropertyBase* clone() const;
};

typedef DOMSVGProperty<SVGNumber> DOMSVGNumber;
typedef DOMSVGProperty<SVGNumberPercentage> DOMSVGNumberPercentage;
typedef DOMSVGProperty<SVGNumberList> DOMSVGNumberList;

} // namespace lunasvg

#endif // SVGNUMBER_H
