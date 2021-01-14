#include "svgenumeration.h"

namespace lunasvg {

template<> const EnumEntryList& getEnumEntryList<Display>()
{
    static EnumEntryList entries;
    if(entries.empty())
    {
        entries.emplace_back(DisplayInline, "inline");
        entries.emplace_back(DisplayBlock, "block");
        entries.emplace_back(DisplayNone, "none");
    }

    return entries;
}

template<> const EnumEntryList& getEnumEntryList<Visibility>()
{
    static EnumEntryList entries;
    if(entries.empty())
    {
        entries.emplace_back(VisibilityVisible, "visible");
        entries.emplace_back(VisibilityHidden, "hidden");
        entries.emplace_back(VisibilityCollaspe, "collaspe");
    }

    return entries;
}

template<> const EnumEntryList& getEnumEntryList<LineCap>()
{
    static EnumEntryList entries;
    if(entries.empty())
    {
        entries.emplace_back(LineCapButt, "butt");
        entries.emplace_back(LineCapRound, "round");
        entries.emplace_back(LineCapSquare, "square");
    }

    return entries;
}

template<> const EnumEntryList& getEnumEntryList<LineJoin>()
{
    static EnumEntryList entries;
    if(entries.empty())
    {
        entries.emplace_back(LineJoinMiter, "miter");
        entries.emplace_back(LineJoinBevel, "bevel");
        entries.emplace_back(LineJoinRound, "round");
    }

    return entries;
}

template<> const EnumEntryList& getEnumEntryList<WindRule>()
{
    static EnumEntryList entries;
    if(entries.empty())
    {
        entries.emplace_back(WindRuleNonZero, "nonzero");
        entries.emplace_back(WindRuleEvenOdd, "evenodd");
    }

    return entries;
}

template<> const EnumEntryList& getEnumEntryList<SpreadMethod>()
{
    static EnumEntryList entries;
    if(entries.empty())
    {
        entries.emplace_back(SpreadMethodPad, "pad");
        entries.emplace_back(SpreadMethodReflect, "reflect");
        entries.emplace_back(SpreadMethodRepeat, "repeat");
    }

    return entries;
}

template<> const EnumEntryList& getEnumEntryList<UnitType>()
{
    static EnumEntryList entries;
    if(entries.empty())
    {
        entries.emplace_back(UnitTypeObjectBoundingBox, "objectBoundingBox");
        entries.emplace_back(UnitTypeUserSpaceOnUse, "userSpaceOnUse");
    }

    return entries;
}

template<> const EnumEntryList& getEnumEntryList<MarkerUnitType>()
{
    static EnumEntryList entries;
    if(entries.empty())
    {
        entries.emplace_back(MarkerUnitTypeStrokeWidth, "strokeWidth");
        entries.emplace_back(MarkerUnitTypeUserSpaceOnUse, "objectBoundingBox");
    }

    return entries;
}

template<> const EnumEntryList& getEnumEntryList<TextAnchor>()
{
    static EnumEntryList entries;
    if(entries.empty())
    {
        entries.emplace_back(TextAnchorStart, "start");
        entries.emplace_back(TextAnchorMiddle, "middle");
        entries.emplace_back(TextAnchorEnd, "end");
    }

    return entries;
}

void SVGEnumerationBase::setValueAsString(const std::string& value)
{
    m_value = 0;
    if(value.empty())
        return;

    for(unsigned int i = 0;i < m_entries.size();i++)
    {
        if(m_entries[i].second == value)
        {
            m_value = m_entries[i].first;
            return;
        }
    }
}

std::string SVGEnumerationBase::valueAsString() const
{
    for(unsigned int i = 0;i < m_entries.size();i++)
        if(m_entries[i].first == m_value)
            return m_entries[i].second;

    return KEmptyString;
}

} // namespace lunasvg
