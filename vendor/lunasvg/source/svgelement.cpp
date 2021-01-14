#include "svgelementimpl.h"
#include "svgelementiter.h"
#include "svgdocumentimpl.h"

namespace lunasvg {

struct Bitmap::Impl
{
    Impl(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride);
    Impl(std::uint32_t width, std::uint32_t height);

    std::unique_ptr<std::uint8_t[]> ownData;
    std::uint8_t* data;
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t stride;
};

Bitmap::Impl::Impl(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride)
    : data(data), width(width), height(height), stride(stride)
{
}

Bitmap::Impl::Impl(std::uint32_t width, std::uint32_t height)
    : ownData(new std::uint8_t[width*height*4]), data(nullptr), width(width), height(height), stride(width * 4)
{
}

Bitmap::Bitmap()
{
}

Bitmap::Bitmap(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride)
    : m_impl(new Impl(data, width, height, stride))
{
}

Bitmap::Bitmap(std::uint32_t width, std::uint32_t height)
    : m_impl(new Impl(width, height))
{
}

void Bitmap::reset(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride)
{
    m_impl.reset(new Impl(data, width, height, stride));
}

void Bitmap::reset(std::uint32_t width, std::uint32_t height)
{
    m_impl.reset(new Impl(width, height));
}

std::uint8_t* Bitmap::data() const
{
    return m_impl ? m_impl->data ? m_impl->data : m_impl->ownData.get() : nullptr;
}

std::uint32_t Bitmap::width() const
{
    return m_impl ? m_impl->width : 0;
}

std::uint32_t Bitmap::height() const
{
    return m_impl ? m_impl->height : 0;
}

std::uint32_t Bitmap::stride() const
{
    return m_impl ? m_impl->stride : 0;
}

SVGElement::SVGElement(SVGDocument* document)
    : m_document(document)
{
}

SVGElement::~SVGElement()
{
}

SVGElement* SVGElement::insertContent(const std::string& content, InsertPosition position)
{
    return document()->impl()->insertContent(content, to<SVGElementImpl>(this), position);
}

SVGElement* SVGElement::appendContent(const std::string& content)
{
    return insertContent(content, BeforeEnd);
}

void SVGElement::clearContent()
{
    document()->impl()->clearContent(to<SVGElementImpl>(this));
}

void SVGElement::removeElement()
{
    document()->impl()->removeElement(to<SVGElementImpl>(this));
}

SVGElement* SVGElement::insertElement(const SVGElement* element, InsertPosition position)
{
    return document()->impl()->copyElement(to<SVGElementImpl>(element), to<SVGElementImpl>(this), position);
}

SVGElement* SVGElement::appendElement(const SVGElement* element)
{
    return insertElement(element, BeforeEnd);
}

SVGElement* SVGElement::getElementById(const std::string& id, int index) const
{
    SVGElementIter it(this, id);
    while(it.next())
    {
        if(index-- == 0)
            return it.currentElement();
    }

    return nullptr;
}

SVGElement* SVGElement::getElementByTag(const std::string& tagName, int index) const
{
    SVGElementIter it(this, KEmptyString, tagName);
    while(it.next())
    {
        if(index-- == 0)
            return it.currentElement();
    }

    return nullptr;
}

SVGElement* SVGElement::parentElement() const
{
    return to<SVGElementImpl>(this)->parent;
}

std::string SVGElement::toString() const
{
    return document()->impl()->toString(to<SVGElementImpl>(this));
}

} // namespace lunasvg
