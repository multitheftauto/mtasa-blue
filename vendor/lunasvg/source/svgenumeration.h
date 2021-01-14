#ifndef SVGENUMERATION_H
#define SVGENUMERATION_H

#include "svgproperty.h"

#include <vector>

namespace lunasvg {

enum Display
{
    DisplayInline,
    DisplayBlock,
    DisplayNone,
};

enum Visibility
{
    VisibilityVisible,
    VisibilityHidden,
    VisibilityCollaspe
};

enum LineCap
{
    LineCapButt,
    LineCapRound,
    LineCapSquare,
};

enum LineJoin
{
    LineJoinMiter,
    LineJoinRound,
    LineJoinBevel
};

enum WindRule
{
    WindRuleNonZero,
    WindRuleEvenOdd
};

enum UnitType
{
    UnitTypeUserSpaceOnUse,
    UnitTypeObjectBoundingBox
};

enum SpreadMethod
{
    SpreadMethodPad,
    SpreadMethodReflect,
    SpreadMethodRepeat
};

enum MarkerUnitType
{
    MarkerUnitTypeStrokeWidth,
    MarkerUnitTypeUserSpaceOnUse
};

enum TextAnchor
{
    TextAnchorStart,
    TextAnchorMiddle,
    TextAnchorEnd
};

typedef std::pair<unsigned short, std::string> EnumEntry;
typedef std::vector<EnumEntry> EnumEntryList;

template<typename Enum> const EnumEntryList& getEnumEntryList();
template<> const EnumEntryList& getEnumEntryList<Display>();
template<> const EnumEntryList& getEnumEntryList<Visibility>();
template<> const EnumEntryList& getEnumEntryList<LineCap>();
template<> const EnumEntryList& getEnumEntryList<LineJoin>();
template<> const EnumEntryList& getEnumEntryList<WindRule>();
template<> const EnumEntryList& getEnumEntryList<SpreadMethod>();
template<> const EnumEntryList& getEnumEntryList<UnitType>();
template<> const EnumEntryList& getEnumEntryList<MarkerUnitType>();
template<> const EnumEntryList& getEnumEntryList<TextAnchor>();

class SVGEnumerationBase : public SVGPropertyBase
{
public:
    void setValue(unsigned short value) { m_value = value; }
    unsigned short value() const { return m_value; }

    void setValueAsString(const std::string& value);
    std::string valueAsString() const;

protected:
    SVGEnumerationBase(const EnumEntryList& entries)
        : m_value(0),
          m_entries(entries)
    {}

protected:
    unsigned short m_value;
    const EnumEntryList& m_entries;
};

template<typename T>
class SVGEnumeration : public SVGEnumerationBase
{
public:
    SVGEnumeration()
        : SVGEnumerationBase(getEnumEntryList<T>())
    {}

    T enumValue() const
    {
        return static_cast<T>(m_value);
    }

    void setEnumValue(T value)
    {
        m_value = value;
    }

    SVGPropertyBase* clone() const
    {
        SVGEnumeration<T>* property = new SVGEnumeration<T>();
        property->m_value = m_value;

        return property;
    }
};

template<typename T>
class DOMSVGEnumeration : public DOMSVGProperty<SVGEnumeration<T>>
{
public:
    DOMSVGEnumeration(DOMPropertyID propertyId)
        : DOMSVGProperty<SVGEnumeration<T>>(propertyId)
    {}
};

} // namespace lunasvg

#endif // SVGENUMERATION_H
