/***********************************************************************
    created:    28/5/2011
    author:     Martin Preisler

    purpose:    Implements platform independent clipboard handling
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/Clipboard.h"

// Start of CEGUI namespace section
namespace CEGUI
{

NativeClipboardProvider::~NativeClipboardProvider()
{}
    
//----------------------------------------------------------------------------//
Clipboard::Clipboard():
    d_mimeType("text/plain"), // reasonable default I think
    d_buffer(0),
    d_bufferSize(0),
    
    d_nativeProvider(0)
{}

//----------------------------------------------------------------------------//    
Clipboard::~Clipboard()
{
    if (d_buffer != 0)
    {
        CEGUI_DELETE_ARRAY_PT(d_buffer, BufferElement, d_bufferSize, Clipboard);
    }
}

//----------------------------------------------------------------------------//
void Clipboard::setNativeProvider(NativeClipboardProvider* provider)
{
    d_nativeProvider = provider;
}

//----------------------------------------------------------------------------//
NativeClipboardProvider* Clipboard::getNativeProvider() const
{
    return d_nativeProvider;
}

//----------------------------------------------------------------------------//
void Clipboard::setData(const String& mimeType, const void* buffer, size_t size)
{
    d_mimeType = mimeType;
    
    if (size != d_bufferSize)
    {
        if (d_buffer != 0)
        {
            CEGUI_DELETE_ARRAY_PT(d_buffer, BufferElement, d_bufferSize, Clipboard);
            d_buffer = 0;
        }
        
        d_bufferSize = size;
        d_buffer = CEGUI_NEW_ARRAY_PT(BufferElement, d_bufferSize, Clipboard);
    }
    
    memcpy(d_buffer, buffer, d_bufferSize);
    
    // we have set the data to the internal clipboard, now sync it with the
    // system-wide native clipboard if possible
    if (d_nativeProvider)
    {
        d_nativeProvider->sendToClipboard(d_mimeType, d_buffer, d_bufferSize);
    }
}

//----------------------------------------------------------------------------//
void Clipboard::getData(String& mimeType, const void*& buffer, size_t& size)
{
    // first make sure we are in sync with system-wide native clipboard
    // (if possible)
    if (d_nativeProvider)
    {
        size_t retrievedSize;
        void* retrievedBuffer;
        
        d_nativeProvider->retrieveFromClipboard(d_mimeType, retrievedBuffer, retrievedSize);
        
        if (retrievedSize != d_bufferSize)
        {
            if (d_buffer != 0)
            {
                CEGUI_DELETE_ARRAY_PT(d_buffer, BufferElement, d_bufferSize, Clipboard);
                d_buffer = 0;
            }
            
            d_bufferSize = retrievedSize;
            d_buffer = CEGUI_NEW_ARRAY_PT(BufferElement, d_bufferSize, Clipboard);
        }
        
        memcpy(d_buffer, retrievedBuffer, retrievedSize);
    }
    
    mimeType = d_mimeType;
    buffer = d_buffer;
    size = d_bufferSize;
}

//----------------------------------------------------------------------------//
void Clipboard::setText(const String& text)
{
    // could be just ASCII if std::string is used as CEGUI::String
    const char* utf8_bytes = text.c_str();
    
    // we don't want the actual string length, that might not be the buffer size
    // in case of utf8!
    // this gets us the number of bytes until \0 is encountered
    const size_t size = strlen(utf8_bytes);
    
    setData("text/plain", static_cast<const void*>(utf8_bytes), size);
}

//----------------------------------------------------------------------------//
String Clipboard::getText()
{
    String mimeType;
    const void* buffer;
    size_t size;

    // we have to use this, can't use the member variables directly because of
    // the native clipboard provider!
    getData(mimeType, buffer, size);

    if (mimeType == "text/plain" && size != 0)
    {
        // d_buffer an utf8 or ASCII C string (ASCII if std::string is used)

        // !!! However it is not null terminated !!! So we have to tell String
        // how many code units (not code points!) there are.
#if CEGUI_STRING_CLASS == CEGUI_STRING_CLASS_UNICODE
        return String(reinterpret_cast<const utf8*>(d_buffer), d_bufferSize);
#else
        return String(static_cast<const char*>(d_buffer), d_bufferSize);
#endif
    }
    else
    {
        // the held mime type differs, it's not plain text so we can't
        // return it as just string
        return String();
    }
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
