#ifndef ELEMENT_H
#define ELEMENT_H

#include <memory>
#include <list>

#include "property.h"

namespace lunasvg {

enum class ElementID
{
    Unknown = 0,
    Star,
    Circle,
    ClipPath,
    Defs,
    Ellipse,
    G,
    Line,
    LinearGradient,
    Marker,
    Mask,
    Path,
    Pattern,
    Polygon,
    Polyline,
    RadialGradient,
    Rect,
    SolidColor,
    Stop,
    Style,
    Svg,
    Symbol,
    Use
};

enum class PropertyID
{
    Unknown = 0,
    Class,
    Clip_Path,
    Clip_Rule,
    ClipPathUnits,
    Color,
    Cx,
    Cy,
    D,
    Display,
    Fill,
    Fill_Opacity,
    Fill_Rule,
    Fx,
    Fy,
    GradientTransform,
    GradientUnits,
    Height,
    Href,
    Id,
    Marker_End,
    Marker_Mid,
    Marker_Start,
    MarkerHeight,
    MarkerUnits,
    MarkerWidth,
    Mask,
    MaskContentUnits,
    MaskUnits,
    Offset,
    Opacity,
    Orient,
    Overflow,
    PatternContentUnits,
    PatternTransform,
    PatternUnits,
    Points,
    PreserveAspectRatio,
    R,
    RefX,
    RefY,
    Rx,
    Ry,
    Solid_Color,
    Solid_Opacity,
    SpreadMethod,
    Stop_Color,
    Stop_Opacity,
    Stroke,
    Stroke_Dasharray,
    Stroke_Dashoffset,
    Stroke_Linecap,
    Stroke_Linejoin,
    Stroke_Miterlimit,
    Stroke_Opacity,
    Stroke_Width,
    Style,
    Transform,
    ViewBox,
    Visibility,
    Width,
    X,
    X1,
    X2,
    Y,
    Y1,
    Y2
};

struct Property
{
    PropertyID id;
    std::string value;
    int specificity;
};

class PropertyList
{
public:
    PropertyList() = default;

    void set(PropertyID id, const std::string& value, int specificity);
    Property* get(PropertyID id) const;
    void add(const Property& property);
    void add(const PropertyList& properties);
    void clear() { m_properties.clear(); }

private:
    std::vector<Property> m_properties;
};

class LayoutContext;
class LayoutContainer;
class Element;

class Node
{
public:
    Node() = default;
    virtual ~Node() = default;

    virtual bool isText() const { return false; }
    virtual bool isPaint() const { return false; }
    virtual bool isGeometry() const { return false; }
    virtual void layout(LayoutContext*, LayoutContainer*) const;
    virtual std::unique_ptr<Node> clone() const = 0;

public:
    Element* parent = nullptr;
};

class TextNode : public Node
{
public:
    TextNode() = default;

    bool isText() const { return true; }
    std::unique_ptr<Node> clone() const;

public:
    std::string text;
};

using NodeList = std::list<std::unique_ptr<Node>>;

class Element : public Node
{
public:
    Element(ElementID id);

    void set(PropertyID id, const std::string& value, int specificity);
    const std::string& get(PropertyID id) const;
    const std::string& find(PropertyID id) const;
    bool has(PropertyID id) const;

    Element* previousElement() const;
    Element* nextElement() const;
    Node* addChild(std::unique_ptr<Node> child);
    void layoutChildren(LayoutContext* context, LayoutContainer* current) const;
    Rect currentViewport() const;

    template<typename T>
    void transverse(T callback)
    {
        if(callback(this))
            return;

        for(auto& child : children)
        {
            if(child->isText())
            {
                if(callback(child.get()))
                    return;
                continue;
            }

            auto element = static_cast<Element*>(child.get());
            element->transverse(callback);
        }
    }

    template<typename T>
    std::unique_ptr<T> cloneElement() const
    {
        auto element = std::make_unique<T>();
        element->properties = properties;
        for(auto& child : children)
            element->addChild(child->clone());
        return element;
    }

public:
    ElementID id;
    NodeList children;
    PropertyList properties;
};

} // namespace lunasvg

#endif // ELEMENT_H
