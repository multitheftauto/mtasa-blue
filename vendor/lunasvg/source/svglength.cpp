#include "svglength.h"
#include "rendercontext.h"

namespace lunasvg {

#define SQRT2 1.41421356237309504880

const SVGLength* hundredPercent()
{
    static const SVGLength value(100.0, LengthUnitPercent);
    return &value;
}

const SVGLength* fiftyPercent()
{
    static const SVGLength value(50.0, LengthUnitPercent);
    return &value;
}

const SVGLength* minusTenPercent()
{
    static SVGLength value(-10.0, LengthUnitPercent);
    return &value;
}

const SVGLength* oneTwentyPercent()
{
    static SVGLength value(120.0, LengthUnitPercent);
    return &value;
}

const SVGLength* threePixels()
{
    static SVGLength value(3.0, LengthUnitPx);
    return &value;
}

SVGLength::SVGLength()
    : m_value(0),
      m_unit(LengthUnitNumber)
{
}

SVGLength::SVGLength(double value, LengthUnit unit)
    : m_value(value),
      m_unit(unit)
{
}

bool SVGLength::isRelative() const
{
    return m_unit == LengthUnitPercent
            || m_unit == LengthUnitEm
            || m_unit == LengthUnitEx;
}

double SVGLength::value(double dpi) const
{
    switch(m_unit)
    {
    case LengthUnitNumber:
    case LengthUnitPx:
        return m_value;
    case LengthUnitIn:
        return m_value * dpi;
    case LengthUnitCm:
        return m_value * dpi / 2.54;
    case LengthUnitMm:
        return m_value * dpi / 25.4;
    case LengthUnitPt:
        return m_value * dpi / 72.0;
    case LengthUnitPc:
        return m_value * dpi / 6.0;
    default:
        break;
    }

    return 0.0;
}

double SVGLength::value(const RenderState& state, double max) const
{
    if(m_unit == LengthUnitPercent)
        return m_value * max / 100.0;

    return value(state.dpi);
}

double SVGLength::value(const RenderState& state, LengthMode mode) const
{
    if(m_unit == LengthUnitPercent)
    {
        double w = state.viewPort.width;
        double h = state.viewPort.height;
        double max = (mode == LengthModeWidth) ? w : (mode == LengthModeHeight) ? h : std::sqrt(w*w+h*h) / SQRT2;
        return m_value * max / 100.0;
    }

    return value(state.dpi);
}

double SVGLength::valueX(const RenderState& state) const
{
    return value(state, LengthModeWidth);
}

double SVGLength::valueY(const RenderState& state) const
{
    return value(state, LengthModeHeight);
}

bool SVGLength::parseLength(const char*& ptr, double& value, LengthUnit& unit)
{
    if(!Utils::parseNumber(ptr, value))
        return false;

    switch(ptr[0])
    {
    case '%':
        unit = LengthUnitPercent;
        ++ptr;
        break;
    case 'p':
        if(ptr[1] == 'x')
            unit = LengthUnitPx;
        else if(ptr[1] == 'c')
            unit = LengthUnitPc;
        else if(ptr[1] == 't')
            unit = LengthUnitPt;
        else
            return false;
        ptr += 2;
        break;
    case 'i':
        if(ptr[1] == 'n')
            unit = LengthUnitIn;
        else
            return false;
        ptr += 2;
        break;
    case 'c':
        if(ptr[1] == 'm')
            unit = LengthUnitCm;
        else
            return false;
        ptr += 2;
        break;
    case 'm':
        if(ptr[1] == 'm')
            unit = LengthUnitMm;
        else
            return false;
        ptr += 2;
        break;
    case 'e':
        if(ptr[1] == 'm')
            unit = LengthUnitEm;
        else if(ptr[1] == 'x')
            unit = LengthUnitEx;
        else
            return false;
        ptr += 2;
        break;
    default:
        unit = LengthUnitNumber;
        break;
    }

    return true;
}

void SVGLength::setValueAsString(const std::string& value)
{
    m_value = 0;
    m_unit = LengthUnitNumber;
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    parseLength(ptr, m_value, m_unit);
}

std::string SVGLength::valueAsString() const
{
    std::string out;

    out += Utils::toString(m_value);
    switch(m_unit)
    {
    case LengthUnitPx:
        out += "px";
        break;
    case LengthUnitPc:
        out += "pc";
        break;
    case LengthUnitPt:
        out += "pt";
        break;
    case LengthUnitIn:
        out += "in";
        break;
    case LengthUnitCm:
        out += "cm";
        break;
    case LengthUnitMm:
        out += "mm";
        break;
    case LengthUnitEm:
        out += "em";
        break;
    case LengthUnitEx:
        out += "ex";
        break;
    case LengthUnitPercent:
        out += "%";
        break;
    default:
        break;
    }

    return out;
}

SVGPropertyBase* SVGLength::clone() const
{
    SVGLength* property = new SVGLength();
    property->m_value = m_value;
    property->m_unit = m_unit;

    return property;
}

SVGLengthList::SVGLengthList()
{
}

std::vector<double> SVGLengthList::values(const RenderState& state, LengthMode mode) const
{
    std::vector<double> v(length());
    for(unsigned int i = 0;i < length();i++)
        v[i] = at(i)->value(state, mode);
    return v;
}

void SVGLengthList::setValueAsString(const std::string& value)
{
    clear();
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    double number;
    LengthUnit unit;
    while(*ptr)
    {
        if(!SVGLength::parseLength(ptr, number, unit))
            return;
        SVGLength* item = new SVGLength(number, unit);
        appendItem(item);
        Utils::skipWsComma(ptr);
    }
}

SVGPropertyBase* SVGLengthList::clone() const
{
    SVGLengthList* property = new SVGLengthList();
    baseClone(property);

    return property;
}

DOMSVGLength::DOMSVGLength(DOMPropertyID propertyId, LengthMode mode, LengthNegativeValuesMode negativeValuesMode)
    : DOMSVGProperty<SVGLength>(propertyId),
      m_mode(mode),
      m_negativeValuesMode(negativeValuesMode),
      m_defaultValue(nullptr)
{
}

void DOMSVGLength::setPropertyAsString(const std::string& value)
{
    DOMSVGProperty::setPropertyAsString(value);

    if(m_negativeValuesMode==ForbidNegativeLengths && property()->value()<0)
    {
        property()->setValue(0);
        property()->setUnit(LengthUnitNumber);
    }
}

void DOMSVGLength::setDefaultValue(const SVGLength* value)
{
    m_defaultValue = value;
}

double DOMSVGLength::value(const RenderState& state) const
{
    return isSpecified() ? property()->value(state, m_mode) : m_defaultValue ? m_defaultValue->value(state, m_mode) : 0.0;
}

bool DOMSVGLength::isZero() const
{
    return isSpecified() ? property()->value() == 0.0 : m_defaultValue ? m_defaultValue->value() == 0.0 : true;
}

} // namespace lunasvg
