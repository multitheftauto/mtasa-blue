#include "svgdocument.h"
#include "svgdocumentimpl.h"
#include "svgsvgelement.h"

namespace lunasvg {

SVGDocument::SVGDocument()
    : m_impl(new SVGDocumentImpl(this))
{
}

SVGElement* SVGDocument::getElementById(const std::string& id, int index) const
{
    return m_impl->rootElement()->getElementById(id, index);
}

SVGElement* SVGDocument::getElementByTag(const std::string& tag, int index) const
{
    return m_impl->rootElement()->getElementByTag(tag, index);
}

SVGElement* SVGDocument::insertElement(const SVGElement* element, InsertPosition position)
{
    return m_impl->rootElement()->insertElement(element, position);
}

SVGElement* SVGDocument::appendElement(const SVGElement* element)
{
    return m_impl->rootElement()->appendElement(element);
}

SVGElement* SVGDocument::insertContent(const std::string& content, InsertPosition position)
{
    return m_impl->rootElement()->insertContent(content, position);
}

SVGElement* SVGDocument::appendContent(const std::string& content)
{
    return m_impl->rootElement()->appendContent(content);
}

void SVGDocument::clearContent()
{
    m_impl->rootElement()->clearContent();
}

bool SVGDocument::loadFromFile(const std::string& filename)
{
    return m_impl->loadFromFile(filename);
}

bool SVGDocument::loadFromData(const std::string& content)
{
    return m_impl->loadFromData(content);
}

bool SVGDocument::loadFontFromFile(const std::string& filename)
{
    return m_impl->loadFontFromFile(filename);
}

double SVGDocument::documentWidth(double dpi) const
{
    return m_impl->documentWidth(dpi);
}

double SVGDocument::documentHeight(double dpi) const
{
    return m_impl->documentHeight(dpi);
}

Box SVGDocument::getBBox(double dpi) const
{
    return m_impl->getBBox(dpi);
}

Bitmap SVGDocument::renderToBitmap(std::uint32_t width, std::uint32_t height, double dpi, std::uint32_t bgColor) const
{
    return m_impl->renderToBitmap(width, height, dpi, bgColor);
}

void SVGDocument::render(Bitmap bitmap, double dpi, std::uint32_t bgColor) const
{
    m_impl->render(bitmap, dpi, bgColor);
}

SVGElement* SVGDocument::rootElement() const
{
    return m_impl->rootElement();
}

std::string SVGDocument::toString() const
{
    return m_impl->rootElement()->toString();
}

bool SVGDocument::hasAnimation() const
{
    return m_impl->hasAnimation();
}

double SVGDocument::animationDuration() const
{
    return m_impl->animationDuration();
}

bool SVGDocument::setCurrentTime(double time, bool seekToTime)
{
    return m_impl->setCurrentTime(time, seekToTime);
}

double SVGDocument::currentTime() const
{
    return m_impl->currentTime();
}

SVGDocument::~SVGDocument()
{
    delete m_impl;
}

} // namespace lunasvg
