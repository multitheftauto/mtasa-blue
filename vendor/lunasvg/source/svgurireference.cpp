#include "svgurireference.h"
#include "svgelementimpl.h"

namespace lunasvg {

SVGURIReference::SVGURIReference(SVGElementHead* element)
    : m_href(DOMPropertyIdHref)
{
    element->addToPropertyMap(m_href);
}

const std::string& SVGURIReference::hrefValue() const
{
    return m_href.isSpecified() ? m_href.property()->value() : KEmptyString;
}

} // namespace lunasvg
