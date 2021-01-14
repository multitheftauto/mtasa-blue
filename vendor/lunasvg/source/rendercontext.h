#ifndef RENDERCONTEXT_H
#define RENDERCONTEXT_H

#include "cssproperty.h"
#include "paint.h"
#include "rect.h"

#include <stack>
#include <set>

namespace lunasvg {

class RenderState;
class Paint;
class StrokeData;
class SVGDocument;
class SVGElementImpl;
class SVGMarkerElement;
class SVGMaskElement;
class SVGClipPathElement;

class RenderStyle
{
public:
    RenderStyle();

    void add(const CSSPropertyList* items);
    void add(const DOMSVGStyle& style);
    void add(const RenderStyle& style);
    void inheritFrom(const RenderStyle& style);
    void set(const CSSPropertyBase* item);
    void clear(CSSPropertyID nameId);
    void clearAll();
    bool isDisplayNone() const;
    bool isHidden() const;

    const SVGPropertyBase* get(CSSPropertyID nameId) const { return m_properties[nameId]; }
    bool isSet(CSSPropertyID nameId) const { return m_properties[nameId]; }
    bool isEmpty() const { return m_properties.empty(); }

    bool requiresCompositing(const SVGElementImpl* element) const;
    bool hasStroke() const;
    bool hasFill() const;

    StrokeData strokeData(const RenderState& state) const;
    Paint fillPaint(const RenderState& state) const;
    Paint strokePaint(const RenderState& state) const;
    double strokeWidth(const RenderState& state) const;
    double fontSize(const RenderState& state) const;
    double fillOpacity() const;
    double strokeOpacity() const;
    double opacity() const;
    WindRule fillRule() const;
    WindRule clipRule() const;
    TextAnchor textAnchor() const;
    const SVGMaskElement* mask(const SVGDocument* document) const;
    const SVGClipPathElement* clipPath(const SVGDocument* document) const;
    const SVGMarkerElement* markerStart(const SVGDocument* document) const;
    const SVGMarkerElement* markerMid(const SVGDocument* document) const;
    const SVGMarkerElement* markerEnd(const SVGDocument* document) const;

private:
    std::array<const SVGPropertyBase*, MAX_STYLE> m_properties;
};

class RenderState
{
public:
    const SVGElementImpl* element{nullptr};
    Canvas canvas;
    AffineTransform matrix;
    RenderStyle style;
    Rect viewPort;
    Rect bbox;
    Rgb color;
    double dpi{96.0};
};

class RenderBreaker
{
public:
    static void registerElement(const SVGElementImpl* element);
    static void unregisterElement(const SVGElementImpl* element);
    static bool hasElement(const SVGElementImpl* element);

private:
    static std::set<const SVGElementImpl*> renderBreaker;
};

enum RenderMode
{
    RenderModeDisplay,
    RenderModeClipping,
    RenderModeBounding
};

class RenderContext
{
public:
    ~RenderContext();
    RenderContext(const SVGElementImpl* element, RenderMode mode);
    RenderMode mode() const { return m_mode; }
    RenderState& parent() const { return *m_states.top(); }
    RenderState& state() const { return *m_state; }
    const SVGElementImpl* current() const { return m_current; }
    void push();
    void pop();
    void skipElement();
    void render(const SVGElementImpl* head, const SVGElementImpl* tail);

private:
    RenderMode m_mode;
    RenderState* m_state;
    std::stack<RenderState*> m_states;
    const SVGElementImpl* m_current;
};

} // namespace lunasvg

#endif // RENDERCONTEXT_H
