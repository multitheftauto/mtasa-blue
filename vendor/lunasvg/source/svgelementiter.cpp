#include "svgelementiter.h"
#include "svgelementimpl.h"

namespace lunasvg {

SVGElementIter::SVGElementIter(const SVGElement* element, const std::string& id, const std::string& tagName)
{
    m_element = to<SVGElementImpl>(element);
    m_id.assign(id);
    m_elementId = tagName.empty() ? DOMElementLastId : Utils::domElementId(tagName);
    m_currentElement = m_element->isSVGElementHead() ? const_cast<SVGElementImpl*>(m_element) : nullptr;
}

bool SVGElementIter::next()
{
    if(!m_currentElement)
        return false;

    m_currentElement = m_currentElement->next;
    while(m_currentElement != to<SVGElementHead>(m_element)->tail)
    {
       if(!m_currentElement->isSVGElementTail())
       {
           if((m_elementId==DOMElementLastId || m_elementId==m_currentElement->elementId()) && (m_id.length()==0 || m_id==m_currentElement->id()))
               return true;
       }

       m_currentElement = m_currentElement->next;
    }

    m_currentElement = nullptr;
    return false;
}

SVGElement* SVGElementIter::currentElement() const
{
    return m_currentElement;
}

} // namespace lunasvg
