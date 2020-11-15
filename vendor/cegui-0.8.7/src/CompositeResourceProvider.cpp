/***********************************************************************
  created:  14.07.2011
  Author:   Hans Mackowiak (Hanmac) hanmac@gmx.de
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
#include "CEGUI/CompositeResourceProvider.h"
#include "CEGUI/Exceptions.h"

// Start of CEGUI namespace section
namespace CEGUI
{
CompositeResourceProvider::CompositeResourceProvider() : CEGUI::ResourceProvider(){}

CompositeResourceProvider::~CompositeResourceProvider(void)
{
  ProviderIterator it=getIterator();
  for(it.toStart(); !it.isAtEnd(); ++it)
  {
    CEGUI_DELETE_AO it.getCurrentValue();
    d_providerlist.erase (it.getCurrentKey());
  }
}

void CompositeResourceProvider::add(ResourceProvider *prov,const String& name)
{
  d_providerlist.insert(std::pair<String,ResourceProvider*>(name,prov));
}

void CompositeResourceProvider::remove(ResourceProvider *prov)
{   
  ProviderIterator it=getIterator();
  for(it.toStart(); !it.isAtEnd(); ++it)
  {
    if(it.getCurrentValue() == prov)
      d_providerlist.erase (it.getCurrentKey());
  }
}

void CompositeResourceProvider::remove(const String& name)
{
  d_providerlist.erase(name);
}
ResourceProvider* CompositeResourceProvider::get(const String& name)
{
  return d_providerlist.find(name)->second;
}
void CompositeResourceProvider::loadRawDataContainer(const String& filename,
                         RawDataContainer& output,
                         const String& resourceGroup)
{
  if (filename.empty())
    CEGUI_THROW(InvalidRequestException(
      "Filename supplied for data loading must be valid"));
  ProviderIterator it=getIterator();
  for(it.toStart(); !it.isAtEnd(); ++it)
  {
    std::vector<String> out_vec;
    it.getCurrentValue()->getResourceGroupFileNames(out_vec,filename,resourceGroup);
    for (std::vector<String>::const_iterator vit = out_vec.begin(); vit != out_vec.end(); ++vit)
    {
      if(*vit == filename)
      {
        it.getCurrentValue()->loadRawDataContainer(filename,output,resourceGroup);
        return;
      }
    }   
  }
  CEGUI_THROW(InvalidRequestException(filename + " does not exist"));
}

size_t CompositeResourceProvider::getResourceGroupFileNames(std::vector<String>& out_vec,
                const String& file_pattern,const String& resource_group)
{
  size_t entries = 0;
  ProviderIterator it=getIterator();
  for(it.toStart(); !it.isAtEnd(); ++it)
  {
    entries += it.getCurrentValue()->getResourceGroupFileNames(out_vec,file_pattern,resource_group);
  }
  return entries;
}
CompositeResourceProvider::ProviderIterator CompositeResourceProvider::getIterator(void) const
{
  return ProviderIterator(d_providerlist.begin(), d_providerlist.end());
}
}
