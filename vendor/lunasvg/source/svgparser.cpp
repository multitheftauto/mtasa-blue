#include "svgparser.h"
#include "svgdocumentimpl.h"
#include "svgelementimpl.h"

#define KTagUnknown 0
#define KTagOpen 1
#define KTagClose 2
#define KTagEmpty 3
#define KTagComment 4
#define KTagCData 5
#define KTagPCData 6
#define KTagInstruction 7
#define KTagDocType 8

namespace lunasvg {

SVGElementImpl* SVGParser::parse(const std::string& source, SVGDocument* document, SVGElementHead* parent)
{
    SVGElementImpl* start = new SVGElementText(document, KEmptyString);
    start->parent = parent;
    SVGElementImpl* current = start;

    std::stack<SVGElementHead*> blocks;
    std::stack<std::string> unsupported;
    const char* ptr = source.c_str();
    int tagType;
    std::string tagName, content;
    AttributeList attributes;
    while(enumTag(ptr, tagType, tagName, content, attributes))
    {
        if(tagType==KTagOpen || tagType==KTagEmpty)
        {
            if(!unsupported.empty())
            {
                if(tagType==KTagOpen)
                    unsupported.push(tagName);
                continue;
            }

            DOMElementID elementId = Utils::domElementId(tagName);
            SVGElementHead* element = nullptr;
            parent = blocks.empty() ? current->parent : blocks.top();
            if(parent==nullptr)
            {
                if(elementId!=DOMElementIdSvg)
                    break;
                element = to<SVGElementHead>(document->rootElement());
            }
            else if(Utils::isElementPermitted(parent->elementId(), elementId))
            {
                element = Utils::createElement(elementId, document);
            }

            if(element==nullptr)
            {
                if(tagType==KTagOpen)
                    unsupported.push(tagName);
                continue;
            }

            if(parent!=nullptr)
            {
                element->parent = parent;
                element->prev = current;
                current->next = element;
                current = element;
                if(tagType==KTagEmpty)
                {
                    element->tail = new SVGElementTail(document);
                    element->tail->parent = element;
                    element->tail->prev = current;
                    element->next = element->tail;
                    current = element->tail;
                }
            }

            if(tagType==KTagOpen)
                blocks.push(element);

            for(unsigned int i = 0;i < attributes.size();i++)
                element->setAttribute(attributes[i].first, attributes[i].second);
        }
        else if(tagType==KTagClose)
        {
            if(!unsupported.empty())
            {
                if(unsupported.top() != tagName)
                    break;
                unsupported.pop();
                continue;
            }

            DOMElementID elementId = Utils::domElementId(tagName);
            if(blocks.empty() || elementId!=blocks.top()->elementId())
                break;

            SVGElementHead* element = blocks.top();
            blocks.pop();

            if(element->isSVGRootElement())
                break;

            element->tail = new SVGElementTail(document);
            element->tail->parent = element;
            element->tail->prev = current;
            current->next = element->tail;
            current = element->tail;
        }
        else if(tagType==KTagPCData && !blocks.empty())
        {
            SVGElementText* element = new SVGElementText(document, content);
            element->parent = blocks.top();
            element->prev = current;
            current->next = element;
            current = element;
        }
    }

    current->next = start;
    start->prev = current;

    if(!blocks.empty() || !unsupported.empty())
    {
        document->impl()->freeElement(start, start->prev);
        return nullptr;
    }

    SVGElementImpl* head = start->next;
    SVGElementImpl* tail = start->prev;
    delete start;
    if(head==start)
        return nullptr;

    head->prev = tail;
    tail->next = head;

    return head;
}

#define IS_STARTNAMECHAR(c) (IS_ALPHA(c) ||  c == '_' || c == ':')
#define IS_NAMECHAR(c) (IS_STARTNAMECHAR(c) || IS_NUM(c) || c == '-' || c == '.')

bool SVGParser::enumTag(const char*& ptr, int& tagType, std::string& tagName, std::string& content, AttributeList& attributes)
{
    tagType = KTagUnknown;
    tagName.clear();
    content.clear();
    attributes.clear();
    Utils::skipWs(ptr);
    const char* start = ptr;
    while(*ptr && *ptr!='<')
        ++ptr;
    if(ptr!=start)
    {
        tagType = KTagPCData;
        content.assign(start, Utils::rtrim(start, ptr));
        return true;
    }

    if(*ptr!='<')
        return false;
    ++ptr;

    if(*ptr=='/')
    {
        tagType = KTagClose;
        ++ptr;
    }
    else if(*ptr=='?')
    {
        tagType = KTagInstruction;
        ++ptr;
    }
    else if(*ptr=='!')
    {
        ++ptr;
        if(Utils::skipDesc(ptr, "--", 2))
        {
            const char* sub = strstr(ptr, "-->");
            if(!sub)
                return false;

            tagType = KTagComment;
            content.assign(ptr, sub);
            ptr += content.length() + 3;

            return true;
        }

        if(Utils::skipDesc(ptr, "[CDATA[", 7))
        {
            const char* sub = strstr(ptr, "]]>");
            if(!sub)
                return false;

            tagType = KTagCData;
            content.assign(ptr, sub);
            ptr += content.length() + 3;

            return true;
        }

        if(Utils::skipDesc(ptr, "DOCTYPE", 7))
        {
            start = ptr;
            while(*ptr && *ptr!='>')
            {
                if(*ptr=='[')
                {
                    ++ptr;
                    int depth = 1;
                    while(*ptr && depth > 0)
                    {
                        if(*ptr=='[') ++depth;
                        else if(*ptr==']') --depth;
                        ++ptr;
                    }
                }
                else
                {
                    ++ptr;
                }
            }

            if(*ptr!='>')
                return false;

            tagType = KTagDocType;
            content.assign(start, ptr);
            ptr += 1;

            return true;
        }

        return false;
    }

    if(!*ptr || !IS_STARTNAMECHAR(*ptr))
        return false;

    start = ptr;
    ++ptr;
    while(*ptr && IS_NAMECHAR(*ptr))
        ++ptr;
    tagName.assign(start, ptr);

    Utils::skipWs(ptr);
    if(tagType==KTagClose)
    {
        if(*ptr!='>')
            return false;
        ++ptr;
        return true;
    }

    while(*ptr && IS_STARTNAMECHAR(*ptr))
    {
        start = ptr;
        ++ptr;
        while(*ptr && IS_NAMECHAR(*ptr))
            ++ptr;
        Attribute attribute;
        attribute.first.assign(start, ptr);

        Utils::skipWs(ptr);
        if(*ptr!='=')
            return false;
        ++ptr;

        Utils::skipWs(ptr);
        if(*ptr!='"' && *ptr!='\'')
            return false;

        const char quote = *ptr;
        ++ptr;
        Utils::skipWs(ptr);
        start = ptr;
        while(*ptr && *ptr!=quote)
            ++ptr;
        if(*ptr!=quote)
            return false;
        attribute.second.assign(start, Utils::rtrim(start, ptr));
        attributes.push_back(attribute);
        ++ptr;
        Utils::skipWs(ptr);
    }

    if(*ptr=='>')
    {
        if(tagType!=KTagUnknown)
            return false;
        tagType = KTagOpen;
        ++ptr;
        return true;
    }

    if(*ptr=='/')
    {
        if(tagType!=KTagUnknown)
            return false;
        ++ptr;
        if(*ptr!='>')
            return false;
        tagType = KTagEmpty;
        ++ptr;
        return true;
    }

    if(*ptr=='?')
    {
        if(tagType!=KTagInstruction)
            return false;
        ++ptr;
        if(*ptr!='>')
            return false;
        ++ptr;
        return true;
    }

    return false;
}

} // namespace lunasvg
