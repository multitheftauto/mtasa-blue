#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <set>

#include "property.h"
#include "element.h"

namespace lunasvg {

class SVGElement;
class StyledElement;

enum LengthNegativeValuesMode
{
    AllowNegativeLengths,
    ForbidNegativeLengths
};

enum class TransformType
{
    Matrix,
    Rotate,
    Scale,
    SkewX,
    SkewY,
    Translate
};

class Parser
{
public:
    static Length parseLength(const std::string& string, LengthNegativeValuesMode mode, const Length& defaultValue);
    static LengthList parseLengthList(const std::string& string, LengthNegativeValuesMode mode);
    static double parseNumber(const std::string& string, double defaultValue);
    static double parseNumberPercentage(const std::string& string, double defaultValue);
    static PointList parsePointList(const std::string& string);
    static Transform parseTransform(const std::string& string);
    static Path parsePath(const std::string& string);
    static std::string parseUrl(const std::string& string);
    static std::string parseHref(const std::string& string);
    static Rect parseViewBox(const std::string& string);
    static PreserveAspectRatio parsePreserveAspectRatio(const std::string& string);
    static Angle parseAngle(const std::string& string);
    static MarkerUnits parseMarkerUnits(const std::string& string);
    static SpreadMethod parseSpreadMethod(const std::string& string);
    static Units parseUnits(const std::string& string, Units defaultValue);
    static Color parseColor(const std::string& string, const StyledElement* element, const Color& defaultValue);
    static Paint parsePaint(const std::string& string, const StyledElement* element, const Color& defaultValue);
    static WindRule parseWindRule(const std::string& string);
    static LineCap parseLineCap(const std::string& string);
    static LineJoin parseLineJoin(const std::string& string);
    static Display parseDisplay(const std::string& string);
    static Visibility parseVisibility(const std::string& string);
    static Overflow parseOverflow(const std::string& string, Overflow defaultValue);

private:
    static bool parseLength(const char*& ptr, const char* end, double& value, LengthUnits& units, LengthNegativeValuesMode mode);
    static bool parseNumberList(const char*& ptr, const char* end, double* values, int count);
    static bool parseArcFlag(const char*& ptr, const char* end, bool& flag);
    static bool parseColorComponent(const char*& ptr, const char* end, int& component);
    static bool parseUrlFragment(const char*& ptr, const char* end, std::string& ref);
    static bool parseTransform(const char*& ptr, const char* end, TransformType& type, double* values, int& count);
};

struct SimpleSelector;

using Selector = std::vector<SimpleSelector>;
using SelectorList = std::vector<Selector>;

struct AttributeSelector
{
    enum class MatchType
    {
        None,
        Equal,
        Includes,
        DashMatch,
        StartsWith,
        EndsWith,
        Contains
    };

    MatchType matchType{MatchType::None};
    PropertyID id{PropertyID::Unknown};
    std::string value;
};

struct PseudoClassSelector
{
    enum class Type
    {
        Unknown,
        Empty,
        Root,
        Is,
        Not,
        FirstChild,
        LastChild,
        OnlyChild,
        FirstOfType,
        LastOfType,
        OnlyOfType
    };

    Type type{Type::Unknown};
    int16_t a{0};
    int16_t b{0};
    SelectorList subSelectors;
};

struct SimpleSelector
{
    enum class Combinator
    {
        Descendant,
        Child,
        DirectAdjacent,
        InDirectAdjacent
    };

    Combinator combinator{Combinator::Descendant};
    ElementID id{ElementID::Star};
    std::vector<AttributeSelector> attributeSelectors;
    std::vector<PseudoClassSelector> pseudoClassSelectors;
};

struct Rule
{
    SelectorList selectors;
    PropertyList declarations;
};

class RuleData {
public:
    RuleData(const Selector& selector, const PropertyList& properties, uint32_t specificity, uint32_t position)
        : m_selector(selector), m_properties(properties), m_specificity(specificity), m_position(position)
    {}

    const Selector& selector() const { return m_selector; }
    const PropertyList& properties() const { return m_properties; }
    const uint32_t& specificity() const { return m_specificity; }
    const uint32_t& position() const { return m_position; }

    bool match(const Element* element) const;

private:
    bool matchSimpleSelector(const SimpleSelector& selector, const Element* element) const;
    bool matchAttributeSelector(const AttributeSelector& selector, const Element* element) const;
    bool matchPseudoClassSelector(const PseudoClassSelector& selector, const Element* element) const;

private:
    Selector m_selector;
    PropertyList m_properties;
    uint32_t m_specificity;
    uint32_t m_position;
};

inline bool operator<(const RuleData& a, const RuleData& b) { return std::tie(a.specificity(), a.position()) < std::tie(b.specificity(), b.position()); }
inline bool operator>(const RuleData& a, const RuleData& b) { return std::tie(a.specificity(), a.position()) > std::tie(b.specificity(), b.position()); }

class StyleSheet {
public:
    StyleSheet() = default;

    void parse(const std::string& content);
    void add(const Rule& rule);
    bool empty() const { return m_position == 0; }

    std::vector<const PropertyList*> match(const Element* element) const;

private:
    std::multiset<RuleData> m_rules;
    uint32_t m_position{0};
};

class CSSParser
{
public:
    CSSParser() = default;

    static bool parseSheet(StyleSheet* sheet, const std::string& value);

private:
    static bool parseAtRule(const char*& ptr, const char* end);
    static bool parseRule(const char*& ptr, const char* end, Rule& rule);
    static bool parseSelectors(const char*& ptr, const char* end, SelectorList& selectors);
    static bool parseDeclarations(const char*& ptr, const char* end, PropertyList& declarations);
    static bool parseSelector(const char*& ptr, const char* end, Selector& selector);
    static bool parseSimpleSelector(const char*& ptr, const char* end, SimpleSelector& simpleSelector);
};

class LayoutSymbol;

class TreeBuilder {
public:
    TreeBuilder();
    ~TreeBuilder();

    bool parse(const char* data, std::size_t size);

    SVGElement* rootElement() const { return m_rootElement.get(); }
    Element* getElementById(const std::string& id) const;
    std::unique_ptr<LayoutSymbol> build() const;

private:
    std::unique_ptr<SVGElement> m_rootElement;
    std::map<std::string, Element*> m_idCache;
};

} // namespace lunasvg

#endif // PARSER_H
