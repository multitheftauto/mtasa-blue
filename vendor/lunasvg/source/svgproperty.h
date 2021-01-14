#ifndef SVGPROPERTY_H
#define SVGPROPERTY_H

#include "svgcontentutils.h"
#include "svgparserutils.h"

#include <vector>
#include <cassert>

namespace lunasvg {

class SVGPropertyBase;

template<typename T>
inline T* to(SVGPropertyBase* property)
{
    return static_cast<T*>(property);
}

template<typename T>
inline const T* to(const SVGPropertyBase* property)
{
    return static_cast<const T*>(property);
}

const static std::string KEmptyString;

class SVGPropertyBase
{
public:
    virtual ~SVGPropertyBase();
    virtual void setValueAsString(const std::string& value) = 0;
    virtual std::string valueAsString() const = 0;
    virtual SVGPropertyBase* clone() const = 0;

protected:
    SVGPropertyBase();
};

class DOMSVGPropertyBase
{
public:
    virtual ~DOMSVGPropertyBase();
    virtual SVGPropertyBase* ensurePropertyBase() = 0;
    virtual SVGPropertyBase* propertyBase() const = 0;
    virtual void setPropertyAsString(const std::string& value) = 0;
    virtual std::string propertyAsString() const = 0;
    virtual void resetProperty() = 0;
    virtual bool isSpecified() const = 0;
    virtual void clone(const DOMSVGPropertyBase* property) = 0;
    DOMPropertyID propertyId() const { return m_propertyId; }

protected:
    DOMSVGPropertyBase(DOMPropertyID propertyId)
        : m_propertyId(propertyId)
    {}

private:
    DOMPropertyID m_propertyId;
};

template <typename T>
class SVGListProperty : public SVGPropertyBase
{
public:
    virtual ~SVGListProperty();
    virtual std::string valueAsString() const;
    const T* at(std::size_t index) const { return m_values.at(index); }
    T* at(std::size_t index) { return m_values.at(index); }
    std::size_t length() const { return  m_values.size(); }
    bool isEmpty() const { return m_values.empty(); }
    void appendItem(T* item) { m_values.push_back(item); }
    void insertItem(T* item, std::size_t index) { m_values.insert(m_values.begin() + index, item); }
    void replaceItem(T* item, std::size_t index)
    {
        delete m_values[index];
        m_values[index] = item;
    }

    void removeItem(std::size_t index)
    {
        delete m_values[index];
        m_values.erase(m_values.begin() + index);
    }

    void clear()
    {
        for(std::size_t i = 0;i < m_values.size();i++)
            delete m_values[i];
        m_values.clear();
    }

protected:
    SVGListProperty();
    void baseClone(SVGListProperty<T>* property) const
    {
        for(std::size_t i = 0;i < length();i++)
        {
            T* item = to<T>(at(i)->clone());
            property->appendItem(item);
        }
    }

private:
    std::vector<T*> m_values;
};

template<typename T>
SVGListProperty<T>::SVGListProperty()
{
}

template<typename T>
SVGListProperty<T>::~SVGListProperty()
{
    for(std::size_t i = 0;i < m_values.size();i++)
        delete m_values[i];
}

template<typename T>
std::string SVGListProperty<T>::valueAsString() const
{
    std::string out;
    for(std::size_t i = 0;i < length();i++)
    {
        if(i != 0)
            out += ' ';
        out += at(i)->valueAsString();
    }

    return out;
}

template<typename T>
class DOMSVGProperty : public DOMSVGPropertyBase
{
public:
    DOMSVGProperty(DOMPropertyID propertyId)
        : DOMSVGPropertyBase(propertyId),
          m_property(nullptr)
    {}

    SVGPropertyBase* ensurePropertyBase() { return ensureProperty(); }
    SVGPropertyBase* propertyBase() const { return property(); }
    T* ensureProperty();
    T* property() const { return m_property; }
    bool isSpecified() const { return m_property; }
    void resetProperty();
    void clone(const DOMSVGPropertyBase* property);
    virtual std::string propertyAsString() const;
    virtual void setPropertyAsString(const std::string& value);
    virtual ~DOMSVGProperty();

private:
    T* m_property;
};

template<typename T>
T* DOMSVGProperty<T>::ensureProperty()
{
    if(!m_property)
        m_property = new T();
    return m_property;
}

template<typename T>
void DOMSVGProperty<T>::resetProperty()
{
    delete m_property;
    m_property = nullptr;
}

template<typename T>
void DOMSVGProperty<T>::clone(const DOMSVGPropertyBase* property)
{
    assert(propertyId() == property->propertyId());
    delete m_property;
    m_property = property->isSpecified() ? to<T>(property->propertyBase()->clone()) : nullptr;
}

template<typename T>
std::string DOMSVGProperty<T>::propertyAsString() const
{
    return m_property ? m_property->valueAsString() : KEmptyString;
}

template<typename T>
void DOMSVGProperty<T>::setPropertyAsString(const std::string& value)
{
    ensureProperty()->setValueAsString(value);
}

template<typename T>
DOMSVGProperty<T>::~DOMSVGProperty()
{
    delete m_property;
}

} // namespace lunasvg

#endif // SVGPROPERTY_H
