#include "cssproperty.h"
#include "svgcolor.h"
#include "svglength.h"
#include "svgnumber.h"
#include "svgenumeration.h"
#include "svgstring.h"

namespace lunasvg {

CSSPropertyBase::~CSSPropertyBase()
{
}

CSSPropertyBase::CSSPropertyBase(CSSPropertyID propertyId)
    : m_propertyId(propertyId)
{
}

CSSPropertyBase* CSSPropertyBase::create(CSSPropertyID propertyId)
{
    switch(propertyId)
    {
    case CSSPropertyIdFill:
    case CSSPropertyIdStroke:
        return new CSSProperty<SVGPaint>(propertyId);
    case CSSPropertyIdColor:
    case CSSPropertyIdSolid_Color:
    case CSSPropertyIdStop_Color:
        return new CSSProperty<SVGColor>(propertyId);
    case CSSPropertyIdOpacity:
    case CSSPropertyIdFill_Opacity:
    case CSSPropertyIdStroke_Opacity:
    case CSSPropertyIdSolid_Opacity:
    case CSSPropertyIdStop_Opacity:
        return new CSSProperty<SVGNumberPercentage>(propertyId);
    case CSSPropertyIdStroke_Miterlimit:
        return new CSSProperty<SVGNumber>(propertyId);
    case CSSPropertyIdFont_Size:
    case CSSPropertyIdStroke_Width:
    case CSSPropertyIdStroke_Dashoffset:
        return new CSSProperty<SVGLength>(propertyId);
    case CSSPropertyIdStroke_Dasharray:
        return new CSSProperty<SVGLengthList>(propertyId);
    case CSSPropertyIdFill_Rule:
    case CSSPropertyIdClip_Rule:
        return new CSSProperty<SVGEnumeration<WindRule>>(propertyId);
    case CSSPropertyIdStroke_Linecap:
        return new CSSProperty<SVGEnumeration<LineCap>>(propertyId);
    case CSSPropertyIdStroke_Linejoin:
        return new CSSProperty<SVGEnumeration<LineJoin>>(propertyId);
    case CSSPropertyIdDisplay:
        return new CSSProperty<SVGEnumeration<Display>>(propertyId);
    case CSSPropertyIdText_Anchor:
        return new CSSProperty<SVGEnumeration<TextAnchor>>(propertyId);
    case CSSPropertyIdVisibility:
        return new CSSProperty<SVGEnumeration<Visibility>>(propertyId);
    case CSSPropertyIdClip_Path:
    case CSSPropertyIdMask:
    case CSSPropertyIdMarker_Start:
    case CSSPropertyIdMarker_Mid:
    case CSSPropertyIdMarker_End:
        return new CSSProperty<SVGURIString>(propertyId);
    default:
        break;
    }

    return nullptr;
}

CSSPropertyList::~CSSPropertyList()
{
    for(unsigned int i = 0;i < MAX_STYLE;i++)
        delete m_values[i];
}

CSSPropertyList::CSSPropertyList()
{
    m_values.fill(nullptr);
}

CSSPropertyBase* CSSPropertyList::getItem(CSSPropertyID nameId) const
{
    return m_values[nameId];
}

void CSSPropertyList::clear()
{
    for(unsigned int i = 0;i < MAX_STYLE;i++)
    {
        delete m_values[i];
        m_values[i] = nullptr;
    }
}

void CSSPropertyList::addItem(CSSPropertyBase* item)
{
    delete m_values[item->propertyId()];
    m_values[item->propertyId()] = item;
}

void CSSPropertyList::removeItem(CSSPropertyID nameId)
{
    delete m_values[nameId];
    m_values[nameId] = nullptr;
}

#define IS_STARTNAMECHAR(c) (IS_ALPHA(c) || c == '_')
#define IS_NAMECHAR(c) (IS_STARTNAMECHAR(c) || IS_NUM(c) || c == '-')

void CSSPropertyList::setValueAsString(const std::string& value)
{
    clear();
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    while(*ptr && IS_STARTNAMECHAR(*ptr))
    {
        const char* start = ptr;
        ++ptr;
        while(*ptr && IS_NAMECHAR(*ptr))
            ++ptr;
        std::string name(start, ptr);
        Utils::skipWs(ptr);
        if(*ptr!=':')
            return;
        ++ptr;
        Utils::skipWs(ptr);
        start = ptr;
        while(*ptr && *ptr!=';')
            ++ptr;
        std::string value(start, Utils::rtrim(start, ptr));
        Utils::skipWsDelimiter(ptr, ';');
        if(CSSPropertyBase* item = CSSPropertyBase::create(Utils::cssPropertyId(name)))
        {
            item->setPropertyAsString(value);
            addItem(item);
        }
    }
}

std::string CSSPropertyList::valueAsString() const
{
    std::string out;
    for(unsigned int i = 0;i < MAX_STYLE;i++)
    {
        if(m_values[i] == nullptr)
            continue;

        out += Utils::cssPropertyName(m_values[i]->propertyId());
        out += ':';
        out += m_values[i]->propertyAsString();
        out += ';';
    }

    return out;
}

SVGPropertyBase* CSSPropertyList::clone() const
{
    CSSPropertyList* property = new CSSPropertyList;
    for(unsigned int i = 0;i < MAX_STYLE;i++)
    {
        if(m_values[i])
            property->m_values[i] = m_values[i]->clone();
    }

    return property;
}

DOMSVGStyle::DOMSVGStyle(DOMPropertyID propertyId)
    : DOMSVGProperty<CSSPropertyList>(propertyId)
{
}

void DOMSVGStyle::setItemAsString(CSSPropertyID nameId, const std::string& value)
{
    if(CSSPropertyBase* item = ensureProperty()->getItem(nameId))
    {
        item->setPropertyAsString(value);
        return;
    }

    if(CSSPropertyBase* item = CSSPropertyBase::create(nameId))
    {
        item->setPropertyAsString(value);
        property()->addItem(item);
    }
}

std::string DOMSVGStyle::itemAsString(CSSPropertyID nameId) const
{
    if(!isSpecified())
        return KEmptyString;

    if(const CSSPropertyBase* item = property()->getItem(nameId))
        return item->propertyAsString();

   return KEmptyString;
}

bool DOMSVGStyle::hasItem(CSSPropertyID nameId) const
{
    if(!isSpecified())
        return false;

    return property()->getItem(nameId);
}

void DOMSVGStyle::removeItem(CSSPropertyID nameId)
{
    if(!isSpecified())
        return;

    property()->removeItem(nameId);
}

bool DOMSVGStyle::isDisplayNone() const
{
    if(!isSpecified())
        return false;

    if(const CSSPropertyBase* item = property()->getItem(CSSPropertyIdDisplay))
    {
        if(!item->isInherited())
        {
            const SVGEnumeration<Display>* display = to<SVGEnumeration<Display>>(item->property());
            return display->enumValue() == DisplayNone;
        }
    }

    return false;
}

bool DOMSVGStyle::isHidden() const
{
    if(!isSpecified())
        return false;

    if(const CSSPropertyBase* item = property()->getItem(CSSPropertyIdVisibility))
    {
        if(!item->isInherited())
        {
            const SVGEnumeration<Visibility>* visibility = to<SVGEnumeration<Visibility>>(item->property());
            return visibility->enumValue() == VisibilityHidden;
        }
    }

    return false;
}

} // namespace lunasvg
