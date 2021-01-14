#include "svgdocumentimpl.h"
#include "svgsvgelement.h"
#include "svgparser.h"
#include "font.h"

#include <fstream>

namespace lunasvg {

SVGDocumentImpl::SVGDocumentImpl(SVGDocument* document)
{
    m_rootElement = new SVGRootElement(document);
    m_rootElement->tail = new SVGElementTail(document);
    m_rootElement->parent = m_rootElement;
    m_rootElement->tail->parent = m_rootElement;
    m_rootElement->prev = m_rootElement->tail;
    m_rootElement->next = m_rootElement->tail;
    m_rootElement->tail->prev = m_rootElement;
    m_rootElement->tail->next = m_rootElement;
}

SVGDocumentImpl::~SVGDocumentImpl()
{
    m_idCache.clear();
    freeElement(m_rootElement, m_rootElement->tail);
}

bool SVGDocumentImpl::loadFromFile(const std::string& filename)
{
    std::ifstream fs;
    fs.open(filename.c_str());
    if(!fs.is_open())
        return false;

    std::string content;
    std::getline(fs, content, '\0');
    fs.close();

    return loadFromData(content);
}

bool SVGDocumentImpl::loadFromData(const std::string& content)
{
    m_rootElement->clearContent();
    SVGElementImpl* head = SVGParser::parse(content, m_rootElement->document(), nullptr);
    if(!head)
        return false;

    SVGElementImpl* tail = head->prev;
    insertElement(head, tail, m_rootElement, AfterBegin);
    dispatchElementInsertEvent(head, tail);
    return true;
}

bool SVGDocumentImpl::loadFontFromFile(const std::string& filename)
{
    m_font = Font::loadFromFile(filename);
    return !!m_font;
}

double SVGDocumentImpl::documentWidth(double dpi) const
{
    const DOMSVGLength& width = m_rootElement->width();
    const DOMSVGRect& viewBox = m_rootElement->viewBox();

    if(width.isSpecified() && !width.property()->isRelative())
        return width.property()->value(dpi);

    if(viewBox.isSpecified() && viewBox.property()->isValid())
        return viewBox.property()->width();

    return -1.0;
}

double SVGDocumentImpl::documentHeight(double dpi) const
{
    const DOMSVGLength& height = m_rootElement->height();
    const DOMSVGRect& viewBox = m_rootElement->viewBox();

    if(height.isSpecified() && !height.property()->isRelative())
        return height.property()->value(dpi);

    if(viewBox.isSpecified() && viewBox.property()->isValid())
        return viewBox.property()->height();

    return -1.0;
}

Box SVGDocumentImpl::getBBox(double dpi) const
{
    RenderContext context(m_rootElement, RenderModeBounding);
    RenderState& state = context.state();
    state.element = m_rootElement;
    state.viewPort = Rect(0, 0, std::max(documentWidth(dpi), 1.0), std::max(documentHeight(dpi), 1.0));
    state.dpi = dpi;
    context.render(m_rootElement, m_rootElement->tail);

    return Box(state.bbox.x, state.bbox.y, state.bbox.width, state.bbox.height);
}

Bitmap SVGDocumentImpl::renderToBitmap(std::uint32_t width, std::uint32_t height, double dpi, std::uint32_t bgColor) const
{
    double documentWidth = this->documentWidth(dpi);
    double documentHeight = this->documentHeight(dpi);
    if(documentWidth < 0.0 || documentHeight < 0.0)
    {
        Box bbox = this->getBBox(dpi);
        documentWidth = bbox.width;
        documentHeight = bbox.height;
    }

    if(documentWidth == 0.0 || documentHeight == 0.0)
        return Bitmap();

    if(width == 0 && height == 0)
    {
        width = std::uint32_t(std::ceil(documentWidth));
        height = std::uint32_t(std::ceil(documentHeight));
    }
    else if(width != 0 && height == 0)
    {
        height = std::uint32_t(std::ceil(width * documentHeight / documentWidth));
    }
    else if(height != 0 && width == 0)
    {
        width = std::uint32_t(std::ceil(height * documentWidth / documentHeight));
    }

    Bitmap bitmap(width, height);
    Rect viewBox(0, 0, documentWidth, documentHeight);
    if(m_rootElement->viewBox().isSpecified() && m_rootElement->viewBox().property()->isValid())
        viewBox = m_rootElement->viewBox().property()->value();

    m_rootElement->renderToBitmap(bitmap, viewBox, dpi, bgColor);
    return bitmap;
}

void SVGDocumentImpl::render(Bitmap& bitmap, double dpi, std::uint32_t bgColor) const
{
    RenderContext context(m_rootElement, RenderModeDisplay);
    RenderState& state = context.state();
    state.element = m_rootElement;
    state.canvas.reset(bitmap.data(), bitmap.width(), bitmap.height(), bitmap.stride());
    state.canvas.clear(bgColor);
    state.viewPort = Rect(0, 0, bitmap.width(), bitmap.height());
    state.dpi = dpi;
    context.render(m_rootElement, m_rootElement->tail);
    state.canvas.convertToRGBA();
}

void SVGDocumentImpl::updateIdCache(const std::string& oldValue, const std::string& newValue, SVGElementImpl* element)
{
    if(!oldValue.empty() && !m_idCache.empty())
    {
        typedef std::multimap<std::string, SVGElementImpl*>::iterator iterator;
        std::pair<iterator, iterator> range = m_idCache.equal_range(oldValue);
        for(iterator it = range.first;it!=range.second;++it)
        {
            if(it->second == element)
            {
                m_idCache.erase(it);
                break;
            }
        }
    }

    if(!newValue.empty())
        m_idCache.insert(std::make_pair(newValue, element));
}

SVGElementImpl* SVGDocumentImpl::resolveIRI(const std::string& href) const
{
    if(href.length()>1 && href[0]=='#')
    {
        std::multimap<std::string, SVGElementImpl*>::const_iterator it = m_idCache.find(href.substr(1));
        return it != m_idCache.end() ? it->second : nullptr;
    }

    return nullptr;
}

SVGElementImpl* SVGDocumentImpl::insertContent(const std::string& content, SVGElementImpl* target, InsertPosition position)
{
    SVGElementHead* parent;
    if(target->isSVGElementHead() && (target->isSVGRootElement() || position==AfterBegin || position==BeforeEnd))
        parent = to<SVGElementHead>(target);
    else
        parent = target->parent;

    SVGElementImpl* head = SVGParser::parse(content, target->document(), parent);
    if(!head)
        return nullptr;

    SVGElementImpl* tail = head->prev;
    insertElement(head, tail, target, position);
    dispatchElementInsertEvent(head, tail);
    return head;
}

SVGElementImpl* SVGDocumentImpl::copyElement(const SVGElementImpl* element, SVGElementImpl* target, InsertPosition position)
{
    SVGElementHead* parent;
    if(target->isSVGElementHead() && (target->isSVGRootElement() || position==AfterBegin || position==BeforeEnd))
        parent = to<SVGElementHead>(target);
    else
        parent = target->parent;

    if(element->isSVGElementHead() && !Utils::isElementPermitted(parent->elementId(), element->elementId()))
        return nullptr;

    const SVGElementImpl* head = element;
    const SVGElementImpl* tail;
    if(element->isSVGElementHead())
        tail = to<SVGElementHead>(element)->tail;
    else
        tail = element;

    std::stack<SVGElementHead*> blocks;
    SVGElementImpl* start = head->clone(parent->document());
    start->parent = parent;
    if(start->isSVGElementHead())
        blocks.push(to<SVGElementHead>(start));

    SVGElementImpl* current = start;
    while(head != tail)
    {
        head = head->next;
        SVGElementImpl* newElement = head->clone(parent->document());
        if(blocks.empty())
            newElement->parent = parent;
        else
            newElement->parent = blocks.top();
        newElement->prev = current;
        current->next = newElement;
        current = newElement;
        if(newElement->isSVGElementHead())
            blocks.push(to<SVGElementHead>(newElement));
        else if(newElement->isSVGElementTail())
        {
            blocks.top()->tail = to<SVGElementTail>(newElement);
            blocks.pop();
        }
    }

    insertElement(start, current, target, position);
    dispatchElementInsertEvent(start, current);
    return start;
}

SVGElementImpl* SVGDocumentImpl::moveElement(SVGElementImpl* element, SVGElementImpl* target, InsertPosition position)
{
    if(element->isSVGRootElement())
        return nullptr;

    SVGElementHead* parent;
    if(target->isSVGElementHead() && (target->isSVGRootElement() || position==AfterBegin || position==BeforeEnd))
        parent = to<SVGElementHead>(target);
    else
        parent = target->parent;

    if(element->isSVGElementHead() && !Utils::isElementPermitted(parent->elementId(), element->elementId()))
        return nullptr;

    SVGElementImpl* head = element;
    SVGElementImpl* tail;
    if(element->isSVGElementHead())
        tail = to<SVGElementHead>(element)->tail;
    else
        tail = element;

    dispatchElementRemoveEvent(head, tail);

    head->parent = parent;
    head->prev->next = tail->next;
    tail->next->prev = head->prev;

    insertElement(head, tail, target, position);
    dispatchElementInsertEvent(head, tail);
    return head;
}

void SVGDocumentImpl::clearContent(SVGElementImpl* element)
{
    if(!element->isSVGElementHead())
        return;

    SVGElementImpl* head = element;
    SVGElementImpl* tail = to<SVGElementHead>(element)->tail;
    if(head->next == tail)
        return;
    head = head->next;
    tail = tail->prev;
    dispatchElementRemoveEvent(head, tail);
    freeElement(head, tail);
}

void SVGDocumentImpl::removeElement(SVGElementImpl* element)
{
    if(element->isSVGRootElement())
    {
        element->clearContent();
        return;
    }

    SVGElementImpl* head = element;
    SVGElementImpl* tail;
    if(element->isSVGElementHead())
        tail = to<SVGElementHead>(element)->tail;
    else
        tail = element;

    dispatchElementRemoveEvent(head, tail);
    freeElement(head, tail);
}

void SVGDocumentImpl::insertElement(SVGElementImpl* head, SVGElementImpl* tail, SVGElementImpl* target, InsertPosition position)
{
    if(target->isSVGRootElement())
    {
        if(position == BeforeBegin)
            position = AfterBegin;
        else if(position == AfterEnd)
            position = BeforeEnd;
    }
    else if(target->isSVGElementText())
    {
        if(position == BeforeEnd)
            position = BeforeBegin;
        else if(position == AfterEnd)
            position = AfterBegin;
    }

    head->prev = tail;
    tail->next = head;

    switch(position)
    {
    case BeforeBegin:
    {
        SVGElementImpl* targetPrev = target->prev;

        targetPrev->next = head;
        head->prev = targetPrev;

        target->prev = tail;
        tail->next = target;
        break;
    }
    case AfterBegin:
    {
        SVGElementImpl* targetNext = target->next;

        target->next = head;
        head->prev = target;

        targetNext->prev = tail;
        tail->next = targetNext;
        break;
    }
    case BeforeEnd:
    {
        assert(target->isSVGElementHead());
        target = to<SVGElementHead>(target)->tail;
        SVGElementImpl* targetPrev = target->prev;

        targetPrev->next = head;
        head->prev = targetPrev;

        target->prev = tail;
        tail->next = target;
        break;
    }
    case AfterEnd:
    {
        assert(target->isSVGElementHead());
        target = to<SVGElementHead>(target)->tail;
        SVGElementImpl* targetNext = target->next;

        target->next = head;
        head->prev = target;

        targetNext->prev = tail;
        tail->next = targetNext;
        break;
    }
    }
}

void SVGDocumentImpl::freeElement(SVGElementImpl* head, SVGElementImpl* tail)
{
    head->prev->next = tail->next;
    tail->next->prev = head->prev;

    head->prev = tail;
    tail->next = head;

    SVGElementImpl* e = head, *n;
    do
    {
        n = e->next;
        delete e;
        e = n;
    } while(e != head);
}

std::string SVGDocumentImpl::toString(const SVGElementImpl* element) const
{
    const SVGElementImpl* head = element;
    const SVGElementImpl* tail;
    if(element->isSVGElementHead())
        tail = to<SVGElementHead>(element)->tail;
    else
        tail = element;

    std::string out;
    std::uint32_t indent = 0;
    head->externalise(out, indent);
    while(head != tail)
    {
        head = head->next;
        head->externalise(out, indent);
    }

    return out;
}

bool SVGDocumentImpl::hasAnimation() const
{
    return false;
}

double SVGDocumentImpl::animationDuration() const
{
    return 0.0;
}

bool SVGDocumentImpl::setCurrentTime(double, bool)
{
    return false;
}

double SVGDocumentImpl::currentTime() const
{
    return 0.0;
}

void SVGDocumentImpl::dispatchElementRemoveEvent(const SVGElementImpl*, const SVGElementImpl*)
{
}

void SVGDocumentImpl::dispatchElementInsertEvent(const SVGElementImpl*, const SVGElementImpl*)
{
}

} // namespace lunasvg
