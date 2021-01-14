#ifndef CSSPROPERTY_H
#define CSSPROPERTY_H

#include "svgproperty.h"

#include <array>

namespace lunasvg {

#define MAX_STYLE CSSPropertyLastId

class CSSPropertyBase
{
public:
    static CSSPropertyBase* create(CSSPropertyID propertyId);
    virtual ~CSSPropertyBase();
    virtual SVGPropertyBase* ensureProperty() = 0;
    virtual SVGPropertyBase* property() const = 0;
    virtual void setInherited() = 0;
    virtual bool isInherited() const = 0;
    virtual void setPropertyAsString(const std::string& value) = 0;
    virtual std::string propertyAsString() const = 0;
    virtual CSSPropertyBase* clone() const = 0;
    CSSPropertyID propertyId() const { return m_propertyId; }

protected:
    CSSPropertyBase(CSSPropertyID propertyId);

protected:
    const CSSPropertyID m_propertyId;
};

template<typename T>
class CSSProperty : public CSSPropertyBase
{
public:
    CSSProperty(CSSPropertyID propertyId)
        : CSSPropertyBase(propertyId),
          m_property(nullptr)
    {}

    SVGPropertyBase* ensureProperty();
    SVGPropertyBase* property() const { return m_property; }
    bool isInherited() const { return !m_property; }
    void setInherited();
    void setPropertyAsString(const std::string& value);
    std::string propertyAsString() const;
    CSSPropertyBase* clone() const;
    ~CSSProperty();

private:
    T* m_property;
};

template<typename T>
SVGPropertyBase* CSSProperty<T>::ensureProperty()
{
    if(!m_property)
        m_property = new T();
    return m_property;
}

template<typename T>
void CSSProperty<T>::setInherited()
{
    delete m_property;
    m_property = nullptr;
}

template<typename T>
void CSSProperty<T>::setPropertyAsString(const std::string& value)
{
    if(value.compare("inherit") == 0)
    {
        setInherited();
        return;
    }

    ensureProperty()->setValueAsString(value);
}

template<typename T>
std::string CSSProperty<T>::propertyAsString() const
{
    return m_property ? m_property->valueAsString() : std::string("inherit");
}

template<typename T>
CSSPropertyBase* CSSProperty<T>::clone() const
{
    CSSProperty<T>* property = new CSSProperty<T>(m_propertyId);
    property->m_property = m_property ? to<T>(m_property->clone()) : nullptr;

    return property;
}

template<typename T>
CSSProperty<T>::~CSSProperty()
{
    delete m_property;
}

class CSSPropertyList : public SVGPropertyBase
{
public:
    ~CSSPropertyList();
    CSSPropertyList();

    void addItem(CSSPropertyBase* item);
    void removeItem(CSSPropertyID nameId);
    CSSPropertyBase* getItem(CSSPropertyID nameId) const;
    void clear();

    void setValueAsString(const std::string& value);
    std::string valueAsString() const;
    SVGPropertyBase* clone() const;

private:
    std::array<CSSPropertyBase*, MAX_STYLE> m_values;
};

class DOMSVGStyle : public DOMSVGProperty<CSSPropertyList>
{
public:
    DOMSVGStyle(DOMPropertyID propertyId);

    void setItemAsString(CSSPropertyID nameId, const std::string& value);
    std::string itemAsString(CSSPropertyID nameId) const;
    bool hasItem(CSSPropertyID nameId) const;
    void removeItem(CSSPropertyID nameId);

    bool isDisplayNone() const;
    bool isHidden() const;
};

} // namespace lunasvg

#endif // CSSPROPERTY_H
