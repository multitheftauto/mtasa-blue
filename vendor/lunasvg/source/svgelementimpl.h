#ifndef SVGELEMENTIMPL_H
#define SVGELEMENTIMPL_H

#include "svgelement.h"
#include "svgstring.h"

namespace lunasvg {

class SVGElementHead;
class SVGElementTail;
class RenderContext;

template<typename T>
inline T* to(SVGElement* element)
{
    return static_cast<T*>(element);
}

template<typename T>
inline const T* to(const SVGElement* element)
{
    return static_cast<const T*>(element);
}

class SVGElementImpl : public SVGElement
{
public:
    virtual ~SVGElementImpl();
    virtual void setAttribute(const std::string&, const std::string&) {}
    virtual std::string getAttribute(const std::string&) const { return KEmptyString; }
    virtual bool hasAttribute(const std::string&) const { return false; }
    virtual void removeAttribute(const std::string&) {}
    virtual const std::string& tagName() const { return KEmptyString; }
    virtual const std::string& id() const { return KEmptyString; }
    virtual DOMElementID elementId() const { return DOMElementIdUnknown; }
    virtual void render(RenderContext& context) const = 0;
    virtual void externalise(std::string& out, unsigned int& indent) const = 0;
    virtual SVGElementImpl* clone(SVGDocument* document) const = 0;

    SVGElementHead* parent;
    SVGElementImpl* next;
    SVGElementImpl* prev;

    virtual bool isSVGElementHead() const { return false; }
    virtual bool isSVGElementTail() const { return false; }
    virtual bool isSVGElementText() const { return false; }
    virtual bool isSVGStyledElement() const { return false; }
    virtual bool isSVGRootElement() const { return false; }
    virtual bool isSVGGraphicsElement() const { return false; }
    virtual bool isSVGGeometryElement() const { return false; }
    virtual bool isSVGPaintElement() const { return false; }
    virtual bool isSVGGradientElement() const { return false; }
    virtual bool isSVGPolyElement() const { return false; }

protected:
    SVGElementImpl(SVGDocument* document);
};

class SVGElementHead : public SVGElementImpl
{
public:
    virtual ~SVGElementHead();
    virtual void setAttribute(const std::string& name, const std::string& value);
    virtual std::string getAttribute(const std::string& name) const;
    virtual bool hasAttribute(const std::string& name) const;
    virtual void removeAttribute(const std::string& name);
    virtual void render(RenderContext&) const;
    virtual void renderTail(RenderContext&) const;
    virtual void externalise(std::string& out, std::uint32_t& indent) const;
    virtual void externaliseTail(std::string& out, std::uint32_t& indent) const;
    void setProperty(DOMPropertyID nameId, const std::string& value);
    std::string getProperty(DOMPropertyID nameId) const;
    bool hasProperty(DOMPropertyID nameId) const;
    void removeProperty(DOMPropertyID nameId);
    const std::string& tagName() const;
    const std::string& id() const { return m_id; }
    DOMElementID elementId() const { return m_elementId; }
    bool isSVGElementHead() const { return true; }

    SVGElementTail* tail;

    void addToPropertyMap(DOMSVGPropertyBase& property);
    void updateId(const std::string& newValue);

protected:
    SVGElementHead(DOMElementID elementId, SVGDocument* document);
    void baseClone(SVGElementHead& e) const;

private:
    DOMElementID m_elementId;
    std::string m_id;
    std::vector<DOMSVGPropertyBase*> m_properties;
};

class SVGElementTail : public SVGElementImpl
{
public:
    SVGElementTail(SVGDocument* document);
    bool isSVGElementTail() const { return true; }
    void render(RenderContext& context) const;
    void externalise(std::string& out, std::uint32_t& indent) const;
    SVGElementImpl* clone(SVGDocument* document) const;
};

class SVGElementText : public SVGElementImpl
{
public:
    SVGElementText(SVGDocument* document, const std::string& content);
    bool isSVGElementText() const { return true; }
    const std::string& content() const { return m_content; }
    void render(RenderContext&) const;
    void externalise(std::string& out, std::uint32_t&) const;
    SVGElementImpl* clone(SVGDocument* document) const;

private:
    std::string m_content;
};

} // namespace lunasvg

#endif // SVGELEMENTIMPL_H
