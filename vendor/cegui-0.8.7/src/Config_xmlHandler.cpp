/***********************************************************************
    created:    Sat Jul 25 2009
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2009 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/Config_xmlHandler.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/System.h"
#include "CEGUI/XMLAttributes.h"
#include "CEGUI/DefaultResourceProvider.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/FontManager.h"
#include "CEGUI/SchemeManager.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/ScriptModule.h"
#include "CEGUI/XMLParser.h"
#include "CEGUI/falagard/WidgetLookManager.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
const String Config_xmlHandler::CEGUIConfigSchemaName("CEGUIConfig.xsd");
const String Config_xmlHandler::CEGUIConfigElement("CEGUIConfig");
const String Config_xmlHandler::LoggingElement("Logging");
const String Config_xmlHandler::AutoLoadElement("AutoLoad");
const String Config_xmlHandler::ResourceDirectoryElement("ResourceDirectory");
const String Config_xmlHandler::DefaultResourceGroupElement("DefaultResourceGroup");
const String Config_xmlHandler::ScriptingElement("Scripting");
const String Config_xmlHandler::XMLParserElement("DefaultXMLParser");
const String Config_xmlHandler::ImageCodecElement("DefaultImageCodec");
const String Config_xmlHandler::DefaultFontElement("DefaultFont");
const String Config_xmlHandler::DefaultMouseCursorElement("DefaultMouseCursor");
const String Config_xmlHandler::DefaultTooltipElement("DefaultTooltip");
const String Config_xmlHandler::FilenameAttribute("filename");
const String Config_xmlHandler::LevelAttribute("level");
const String Config_xmlHandler::TypeAttribute("type");
const String Config_xmlHandler::GroupAttribute("group");
const String Config_xmlHandler::PatternAttribute("pattern");
const String Config_xmlHandler::DirectoryAttribute("directory");
const String Config_xmlHandler::InitScriptAttribute("initScript");
const String Config_xmlHandler::TerminateScriptAttribute("terminateScript");
const String Config_xmlHandler::ImageAttribute("image");
const String Config_xmlHandler::NameAttribute("name");

//----------------------------------------------------------------------------//
Config_xmlHandler::Config_xmlHandler() :
    d_logLevel(Standard)
{
}

//----------------------------------------------------------------------------//
Config_xmlHandler::~Config_xmlHandler()
{
}

//----------------------------------------------------------------------------//
const String& Config_xmlHandler::getSchemaName() const
{
    return CEGUIConfigSchemaName;
}

//----------------------------------------------------------------------------//
const String& Config_xmlHandler::getDefaultResourceGroup() const
{
    static String ret = "";
    return ret;
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::elementStart(const String& element,
                                     const XMLAttributes& attributes)
{
    if (element == CEGUIConfigElement)
        handleCEGUIConfigElement(attributes);
    else if (element == LoggingElement)
        handleLoggingElement(attributes);
    else if (element == AutoLoadElement)
        handleAutoLoadElement(attributes);
    else if (element == ResourceDirectoryElement)
        handleResourceDirectoryElement(attributes);
    else if (element == DefaultResourceGroupElement)
        handleDefaultResourceGroupElement(attributes);
    else if (element == ScriptingElement)
        handleScriptingElement(attributes);
    else if (element == XMLParserElement)
        handleXMLParserElement(attributes);
    else if (element == ImageCodecElement)
        handleImageCodecElement(attributes);
    else if (element == DefaultFontElement)
        handleDefaultFontElement(attributes);
    else if (element == DefaultMouseCursorElement)
        handleDefaultMouseCursorElement(attributes);
    else if (element == DefaultTooltipElement)
        handleDefaultTooltipElement(attributes);
    else
        Logger::getSingleton().logEvent("Config_xmlHandler::elementStart: "
            "Unknown element encountered: <" + element + ">", Errors);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::elementEnd(const String& element)
{
    if (element == CEGUIConfigElement)
        Logger::getSingleton().logEvent(
            "---- Finished parse of CEGUI config file ----");
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleCEGUIConfigElement(const XMLAttributes& /*attr*/)
{
    Logger::getSingleton().logEvent(
        "---- Started parse of CEGUI config file ----");
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleLoggingElement(const XMLAttributes& attr)
{
    d_logFileName = attr.getValueAsString(FilenameAttribute, "");

    const String logLevel(attr.getValueAsString(LevelAttribute, ""));

    if (logLevel == "Errors")
        d_logLevel = Errors;
    else if (logLevel == "Informative")
        d_logLevel = Informative;
    else if (logLevel == "Insane")
        d_logLevel = Insane;
    else
        d_logLevel = Standard;
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleAutoLoadElement(const XMLAttributes& attr)
{
    AutoLoadResource ob;
    ob.type_string = attr.getValueAsString(TypeAttribute, "");
    ob.type = stringToResourceType(ob.type_string);
    ob.pattern = attr.getValueAsString(PatternAttribute, "*");
    ob.group = attr.getValueAsString(GroupAttribute, "");
    d_autoLoadResources.push_back(ob);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleResourceDirectoryElement(const XMLAttributes& attr)
{
    ResourceDirectory ob;
    ob.group = attr.getValueAsString(GroupAttribute, "");
    ob.directory = attr.getValueAsString(DirectoryAttribute, "./");
    d_resourceDirectories.push_back(ob);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleDefaultResourceGroupElement(const XMLAttributes& attr)
{
    DefaultResourceGroup ob;
    ob.type = stringToResourceType(attr.getValueAsString(TypeAttribute, ""));
    ob.group = attr.getValueAsString(GroupAttribute, "");
    d_defaultResourceGroups.push_back(ob);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleScriptingElement(const XMLAttributes& attr)
{
    d_scriptingInitScript =
        attr.getValueAsString(InitScriptAttribute, "");
    d_scriptingTerminateScript =
        attr.getValueAsString(TerminateScriptAttribute, "");
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleXMLParserElement(const XMLAttributes& attr)
{
    d_xmlParserName = attr.getValueAsString(NameAttribute, "");
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleImageCodecElement(const XMLAttributes& attr)
{
    d_imageCodecName = attr.getValueAsString(NameAttribute, "");
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleDefaultFontElement(const XMLAttributes& attr)
{
    d_defaultFont = attr.getValueAsString(NameAttribute, "");
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleDefaultMouseCursorElement(const XMLAttributes& attr)
{
    d_defaultMouseImage = attr.getValueAsString(ImageAttribute, "");
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::handleDefaultTooltipElement(const XMLAttributes& attr)
{
    d_defaultTooltipType = attr.getValueAsString(NameAttribute, "");
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::initialiseXMLParser() const
{
    if (!d_xmlParserName.empty())
        System::getSingleton().setXMLParser(d_xmlParserName);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::initialiseImageCodec() const
{
    if (!d_imageCodecName.empty())
        System::getSingleton().setImageCodec(d_imageCodecName);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::initialiseLogger(const String& default_filename) const
{
    Logger::getSingleton().setLoggingLevel(d_logLevel);
    Logger::getSingleton().setLogFilename(
        d_logFileName.empty() ? default_filename : d_logFileName);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::initialiseResourceGroupDirectories() const
{
    // if we are called, we dangerously assume that the resource provider is an
    // instace of DefaultResourceProvider...
    DefaultResourceProvider* rp = static_cast<DefaultResourceProvider*>
        (System::getSingleton().getResourceProvider());

    ResourceDirVector::const_iterator i = d_resourceDirectories.begin();
    for (; i != d_resourceDirectories.end(); ++i)
        rp->setResourceGroupDirectory((*i).group, (*i).directory);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::initialiseDefaultResourceGroups() const
{
    DefaultGroupVector::const_iterator i = d_defaultResourceGroups.begin();
    for (; i != d_defaultResourceGroups.end(); ++i)
    {
        switch ((*i).type)
        {
        case RT_IMAGESET:
            ImageManager::setImagesetDefaultResourceGroup((*i).group);
            break;

        case RT_FONT:
            Font::setDefaultResourceGroup((*i).group);
            break;

        case RT_SCHEME:
            Scheme::setDefaultResourceGroup((*i).group);
            break;

        case RT_LOOKNFEEL:
            WidgetLookManager::setDefaultResourceGroup((*i).group);
            break;

        case RT_LAYOUT:
            WindowManager::setDefaultResourceGroup((*i).group);
            break;

        case RT_SCRIPT:
            ScriptModule::setDefaultResourceGroup((*i).group);
            break;

        case RT_XMLSCHEMA:
            if (System::getSingleton().getXMLParser()->
                isPropertyPresent("SchemaDefaultResourceGroup"))
            {
                System::getSingleton().getXMLParser()->
                    setProperty("SchemaDefaultResourceGroup", (*i).group);
            }
            break;

        default:
            System::getSingleton().getResourceProvider()->
                setDefaultResourceGroup((*i).group);
            break;
        }
    }
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::loadAutoResources() const
{
    AutoResourceVector::const_iterator i = d_autoLoadResources.begin();
    for (; i != d_autoLoadResources.end(); ++i)
    {
        switch ((*i).type)
        {
        case RT_IMAGESET:
            autoLoadImagesets((*i).pattern, (*i).group);
            break;

        case RT_FONT:
            FontManager::getSingleton().createAll((*i).pattern, (*i).group);
            break;

        case RT_SCHEME:
            SchemeManager::getSingleton().createAll((*i).pattern, (*i).group);
            break;

        case RT_LOOKNFEEL:
            autoLoadLookNFeels((*i).pattern, (*i).group);
            break;

        default:
            CEGUI_THROW(InvalidRequestException(
                "AutoLoad of resource type '" + (*i).type_string +
                "' is not currently supported.Pattern was: " + (*i).pattern +
                " group was: " + (*i).group));
        }
    }
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::initialiseDefaultFont() const
{
    if (!d_defaultFont.empty())
        System::getSingleton().getDefaultGUIContext().
            setDefaultFont(d_defaultFont);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::initialiseDefaultMouseCursor() const
{
    if (!d_defaultMouseImage.empty())
        System::getSingleton().getDefaultGUIContext().getMouseCursor().
            setDefaultImage(d_defaultMouseImage);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::initialiseDefaulTooltip() const
{
    if (!d_defaultTooltipType.empty())
        System::getSingleton().getDefaultGUIContext().
            setDefaultTooltipType(d_defaultTooltipType);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::executeInitScript() const
{
    if (!d_scriptingInitScript.empty())
        System::getSingleton().executeScriptFile(d_scriptingInitScript);
}

//----------------------------------------------------------------------------//
const String& Config_xmlHandler::getTerminateScriptName() const
{
    return d_scriptingTerminateScript;
}

//----------------------------------------------------------------------------//
Config_xmlHandler::ResourceType Config_xmlHandler::stringToResourceType(
    const String& type) const
{
    if (type == "Imageset")
        return RT_IMAGESET;
    else if (type == "Font")
        return RT_FONT;
    else if (type == "Scheme")
        return RT_SCHEME;
    else if (type == "LookNFeel")
        return RT_LOOKNFEEL;
    else if (type == "Layout")
        return RT_LAYOUT;
    else if (type == "Script")
        return RT_SCRIPT;
    else if (type == "XMLSchema")
        return RT_XMLSCHEMA;
    else
        return RT_DEFAULT;
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::autoLoadLookNFeels(const String& pattern,
                                           const String& group) const
{
    std::vector<String> names;
    const size_t num = System::getSingleton().getResourceProvider()->
        getResourceGroupFileNames(names, pattern, group);

    for (size_t i = 0; i < num; ++i)
        WidgetLookManager::getSingleton().
            parseLookNFeelSpecificationFromFile(names[i], group);
}

//----------------------------------------------------------------------------//
void Config_xmlHandler::autoLoadImagesets(const String& pattern,
                                          const String& group) const
{
    std::vector<String> names;
    const size_t num = System::getSingleton().getResourceProvider()->
        getResourceGroupFileNames(names, pattern, group);

    for (size_t i = 0; i < num; ++i)
        ImageManager::getSingleton().loadImageset(names[i], group);
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
