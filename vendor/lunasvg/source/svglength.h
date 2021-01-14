#ifndef SVGLENGTH_H
#define SVGLENGTH_H

#include "svgproperty.h"

namespace lunasvg {

enum LengthUnit
{
    LengthUnitUnknown,
    LengthUnitNumber,
    LengthUnitPx,
    LengthUnitPt,
    LengthUnitPc,
    LengthUnitIn,
    LengthUnitCm,
    LengthUnitMm,
    LengthUnitEx,
    LengthUnitEm,
    LengthUnitPercent
};

enum LengthMode
{
    LengthModeWidth,
    LengthModeHeight,
    LengthModeBoth
};

enum LengthNegativeValuesMode
{
    AllowNegativeLengths,
    ForbidNegativeLengths
};

class RenderState;
class SVGLength;

const SVGLength* hundredPercent();
const SVGLength* fiftyPercent();
const SVGLength* minusTenPercent();
const SVGLength* oneTwentyPercent();
const SVGLength* threePixels();

class SVGLength : public SVGPropertyBase
{
public:
    SVGLength();
    SVGLength(double value, LengthUnit unit);

    void setUnit(LengthUnit unit) { m_unit = unit; }
    LengthUnit unit() const { return m_unit; }
    bool isRelative() const;
    void setValue(double value) { m_value = value; }
    double value() const { return m_value; }
    double value(double dpi) const;
    double value(const RenderState& state, double max) const;
    double value(const RenderState& state, LengthMode mode = LengthModeBoth) const;
    double valueX(const RenderState& state) const;
    double valueY(const RenderState& state) const;

    static bool parseLength(const char*& ptr, double& value, LengthUnit& unit);
    void setValueAsString(const std::string& value);
    std::string valueAsString() const;
    SVGPropertyBase* clone() const;

private:
    double m_value;
    LengthUnit m_unit;
};

class SVGLengthList : public SVGListProperty<SVGLength>
{
public:
    SVGLengthList();

    std::vector<double> values(const RenderState& state, LengthMode mode = LengthModeBoth) const;
    void setValueAsString(const std::string& value);
    SVGPropertyBase* clone() const;
};

typedef DOMSVGProperty<SVGLengthList> DOMSVGLengthList;

class DOMSVGLength : public DOMSVGProperty<SVGLength>
{
public:
    DOMSVGLength(DOMPropertyID propertyId, LengthMode mode, LengthNegativeValuesMode negativeValuesMode);
    void setPropertyAsString(const std::string& value);
    void setDefaultValue(const SVGLength* value);

    const SVGLength* defaultValue() const { return m_defaultValue; }
    LengthMode mode() const { return m_mode; }
    LengthNegativeValuesMode negativeValuesMode() const { return m_negativeValuesMode; }
    double value(const RenderState& state) const;
    bool isZero() const;

private:
    LengthMode m_mode;
    LengthNegativeValuesMode m_negativeValuesMode;
    const SVGLength* m_defaultValue;
};

} // namespace lunasvg

#endif // SVGLENGTH_H
