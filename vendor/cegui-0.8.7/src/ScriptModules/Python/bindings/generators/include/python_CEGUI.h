#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "CEGUI/CEGUI.h"
#include "CEGUI/falagard/XMLEnumHelper.h"
#include "CEGUI/falagard/PropertyDefinition.h"
#include "CEGUI/falagard/PropertyLinkDefinition.h"

namespace boost
{
    /*
     * Workaround that allows users to inherit CEGUI classes with methods
     * that deal with strings (almost all classes)
     *
     * The situation:
     * boost python can't recognize boost::ref(const CEGUI::String&) and doesn't
     * auto convert it to python unicode string. This works for all types that are
     * registered normally (CEGUI::String is always converted, the class is unknown
     * in python).
     *
     * The solution:
     * Make an overloaded version of boost::ref that just returns what it gets.
     *
     * Is this horrible?
     * YES!
     */
    inline const CEGUI::String& ref(const CEGUI::String& v)
    {
        return v;
    }
}

namespace CEGUI
{

class PropertyHelper_wrapper
{
public:
    static float stringToFloat(const String& str)
    {
        return PropertyHelper<float>::fromString(str);
    }
    static unsigned int stringToUint(const String& str)
    {
        return PropertyHelper<uint>::fromString(str);
    }
    static bool stringToBool(const String& str)
    {
        return PropertyHelper<bool>::fromString(str);
    }
    static Sizef stringToSize(const String& str)
    {
        return PropertyHelper<Sizef >::fromString(str);
    }
    static Vector2f stringToVector2(const String& str)
    {
        return PropertyHelper<Vector2f >::fromString(str);
    }
    static Rectf stringToRect(const String& str)
    {
        return PropertyHelper<Rectf >::fromString(str);
    }
    static const Image* stringToImage(const String& str)
    {
        return PropertyHelper<Image*>::fromString(str);
    }
    static Colour stringToColour(const String& str)
    {
        return PropertyHelper<Colour>::fromString(str);
    }
    static ColourRect stringToColourRect(const String& str)
    {
        return PropertyHelper<ColourRect>::fromString(str);
    }
    static UDim stringToUDim(const String& str)
    {
        return PropertyHelper<UDim>::fromString(str);
    }
    static UVector2 stringToUVector2(const String& str)
    {
        return PropertyHelper<UVector2>::fromString(str);
    }
    static URect stringToURect(const String& str)
    {
        return PropertyHelper<URect>::fromString(str);
    }
    static USize stringToUSize(const String& str)
    {
        return PropertyHelper<USize>::fromString(str);
    }
    static UBox stringToUBox(const String& str)
    {
        return PropertyHelper<UBox>::fromString(str);
    }
    static VerticalTextFormatting stringToVerticalTextFormatting(const String& str)
    {
        return PropertyHelper<VerticalTextFormatting>::fromString(str);
    }
    static HorizontalTextFormatting stringToHorizontalTextFormatting(const String& str)
    {
        return PropertyHelper<HorizontalTextFormatting>::fromString(str);
    }
    static VerticalFormatting stringToVerticalFormatting(const String& str)
    {
        return PropertyHelper<VerticalFormatting>::fromString(str);
    }
    static HorizontalFormatting stringToHorizontalFormatting(const String& str)
    {
        return PropertyHelper<HorizontalFormatting>::fromString(str);
    }
    static HorizontalAlignment stringToHorizontalAlignment(const String& str)
    {
        return PropertyHelper<HorizontalAlignment>::fromString(str);
    }
    static VerticalAlignment stringToVerticalAlignment(const String& str)
    {
        return PropertyHelper<VerticalAlignment>::fromString(str);
    }
    static const Font* stringToFont(const String& str)
    {
        return PropertyHelper<Font*>::fromString(str);
    }
    static AspectMode stringToAspectMode(const String& str)
    {
        return PropertyHelper<AspectMode>::fromString(str);
    }

    static String floatToString(float val)
    {
        return PropertyHelper<float>::toString(val);
    }
    static String uintToString(unsigned int val)
    {
        return PropertyHelper<uint>::toString(val);
    }
    static String boolToString(bool val)
    {
        return PropertyHelper<bool>::toString(val);
    }
    static String sizeToString(const Sizef& val)
    {
        return PropertyHelper<Sizef >::toString(val);
    }
    static String vector2ToString(const Vector2f& val)
    {
        return PropertyHelper<Vector2f >::toString(val);
    }
    static String rectToString(const Rectf& val)
    {
        return PropertyHelper<Rectf >::toString(val);
    }
    static String imageToString(const Image* val)
    {
        return PropertyHelper<Image*>::toString(val);
    }
    static String colourToString(const Colour& val)
    {
        return PropertyHelper<Colour>::toString(val);
    }
    static String colourRectToString(const ColourRect& val)
    {
        return PropertyHelper<ColourRect>::toString(val);
    }
    static String udimToString(const UDim& val)
    {
        return PropertyHelper<UDim>::toString(val);
    }
    static String uvector2ToString(const UVector2& val)
    {
        return PropertyHelper<UVector2>::toString(val);
    }
    static String urectToString(const URect& val)
    {
        return PropertyHelper<URect>::toString(val);
    }
    static String usizeToString(const USize& val)
    {
        return PropertyHelper<USize>::toString(val);
    }
    static String uboxToString(const UBox& val)
    {
        return PropertyHelper<UBox>::toString(val);
    }
    static String verticalTextFormattingToString(VerticalTextFormatting val)
    {
        return PropertyHelper<VerticalTextFormatting>::toString(val);
    }
    static String horizontalTextFormattingToString(HorizontalTextFormatting val)
    {
        return PropertyHelper<HorizontalTextFormatting>::toString(val);
    }
    static String verticalFormattingToString(VerticalFormatting val)
    {
        return PropertyHelper<VerticalFormatting>::toString(val);
    }
    static String horizontalFormattingToString(HorizontalFormatting val)
    {
        return PropertyHelper<HorizontalFormatting>::toString(val);
    }
    static String verticalAlignmentToString(VerticalAlignment val)
    {
        return PropertyHelper<VerticalAlignment>::toString(val);
    }
    static String horizontalAlignmentToString(HorizontalAlignment val)
    {
        return PropertyHelper<HorizontalAlignment>::toString(val);
    }
    static String fontToString(const Font* val)
    {
        return PropertyHelper<Font*>::toString(val);
    }
    static String aspectmodeToString(AspectMode val)
    {
        return PropertyHelper<AspectMode>::toString(val);
    }

};


class FalagardXMLHelper_wrapper
{
public:
    static VerticalFormatting stringToVertFormat(const String& str)
    {
        return FalagardXMLHelper<VerticalFormatting>::fromString(str);
    }
    static HorizontalFormatting stringToHorzFormat(const String& str)
    {
        return FalagardXMLHelper<HorizontalFormatting>::fromString(str);
    }
    static VerticalAlignment stringToVertAlignment(const String& str)
    {
        return FalagardXMLHelper<VerticalAlignment>::fromString(str);
    }
    static HorizontalAlignment stringToHorzAlignment(const String& str)
    {
        return FalagardXMLHelper<HorizontalAlignment>::fromString(str);
    }
    static DimensionType stringToDimensionType(const String& str)
    {
        return FalagardXMLHelper<DimensionType>::fromString(str);
    }
    static VerticalTextFormatting stringToVertTextFormat(const String& str)
    {
        return FalagardXMLHelper<VerticalTextFormatting>::fromString(str);
    }
    static HorizontalTextFormatting stringToHorzTextFormat(const String& str)
    {
        return FalagardXMLHelper<HorizontalTextFormatting>::fromString(str);
    }
    static FontMetricType stringToFontMetricType(const String& str)
    {
        return FalagardXMLHelper<FontMetricType>::fromString(str);
    }
    static DimensionOperator stringToDimensionOperator(const String& str)
    {
        return FalagardXMLHelper<DimensionOperator>::fromString(str);
    }
    static FrameImageComponent stringToFrameImageComponent(const String& str)
    {
        return FalagardXMLHelper<FrameImageComponent>::fromString(str);
    }
    static ChildEventAction stringToChildEventAction(const String& str)
    {
        return FalagardXMLHelper<ChildEventAction>::fromString(str);
    }

    static String vertFormatToString(VerticalFormatting val)
    {
        return FalagardXMLHelper<VerticalFormatting>::toString(val);
    }
    static String horzFormatToString(HorizontalFormatting val)
    {
        return FalagardXMLHelper<HorizontalFormatting>::toString(val);
    }
    static String vertAlignmentToString(VerticalAlignment val)
    {
        return FalagardXMLHelper<VerticalAlignment>::toString(val);
    }
    static String horzAlignmentToString(HorizontalAlignment val)
    {
        return FalagardXMLHelper<HorizontalAlignment>::toString(val);
    }
    static String dimensionTypeToString(DimensionType val)
    {
        return FalagardXMLHelper<DimensionType>::toString(val);
    }
    static String vertTextFormatToString(VerticalTextFormatting val)
    {
        return FalagardXMLHelper<VerticalTextFormatting>::toString(val);
    }
    static String horzTextFormatToString(HorizontalTextFormatting val)
    {
        return FalagardXMLHelper<HorizontalTextFormatting>::toString(val);
    }
    static String fontMetricTypeToString(FontMetricType val)
    {
        return FalagardXMLHelper<FontMetricType>::toString(val);
    }
    static String dimensionOperatorToString(DimensionOperator val)
    {
        return FalagardXMLHelper<DimensionOperator>::toString(val);
    }
    static String frameImageComponentToString(FrameImageComponent val)
    {
        return FalagardXMLHelper<FrameImageComponent>::toString(val);
    }
    static String childEventActionToStringn(ChildEventAction val)
    {
        return FalagardXMLHelper<ChildEventAction>::toString(val);
    }
};

class Workarounds
{
    public:
        static PropertyDefinitionBase* PropertyDefinitionBaseMapGet(WidgetLookFeel::PropertyDefinitionBasePointerMap& map, const WidgetLookFeel::PropertyDefinitionBasePointerMap::key_type& key)
        {
            return map[key];
        }

        static PropertyInitialiser* PropertyInitialiserMapGet(WidgetLookFeel::PropertyInitialiserPointerMap& map, const WidgetLookFeel::PropertyInitialiserPointerMap::key_type& key)
        {
            return map[key];
        }

        static WidgetComponent* WidgetComponentMapGet(WidgetLookFeel::WidgetComponentPointerMap& map, const WidgetLookFeel::WidgetComponentPointerMap::key_type& key)
        {
            return map[key];
        }

        static ImagerySection* ImagerySectionMapGet(WidgetLookFeel::ImagerySectionPointerMap& map, const WidgetLookFeel::ImagerySectionPointerMap::key_type& key)
        {
            return map[key];
        }

        static NamedArea* NamedAreaMapGet(WidgetLookFeel::NamedAreaPointerMap& map, const WidgetLookFeel::NamedAreaPointerMap::key_type& key)
        {
            return map[key];
        }

        static StateImagery* StateImageryMapGet(WidgetLookFeel::StateImageryPointerMap& map, const WidgetLookFeel::StateImageryPointerMap::key_type& key)
        {
            return map[key];
        }

        static EventLinkDefinition* EventLinkDefinitionMapGet(WidgetLookFeel::EventLinkDefinitionPointerMap& map, const WidgetLookFeel::EventLinkDefinitionPointerMap::key_type& key)
        {
            return map[key];
        }

        static WidgetLookFeel* WidgetLookFeelMapGet(WidgetLookManager::WidgetLookPointerMap& map, const WidgetLookManager::WidgetLookPointerMap::key_type& key)
        {
            return map[key];
        }
};

}

// First we create a magic namespace to hold all our aliases
namespace pyplusplus
{
    namespace aliases
    {
        typedef ::CEGUI::RefCounted< CEGUI::FormattedRenderedString > RefCountedFormattedRenderedString;

        typedef ::CEGUI::NamedXMLResourceManager<CEGUI::Font, CEGUI::Font_xmlHandler> NamedXMLResourceManagerFont;
        typedef ::CEGUI::NamedXMLResourceManager<CEGUI::Scheme, CEGUI::Scheme_xmlHandler> NamedXMLResourceManagerScheme;

        typedef ::CEGUI::Vector2<float> Vector2;
        typedef ::CEGUI::Vector3<float> Vector3;
        typedef ::CEGUI::Size<float> Size;
        typedef ::CEGUI::Rect<float> Rect;
        typedef ::CEGUI::Vector2<CEGUI::UDim> UVector2;
        typedef ::CEGUI::Rect<CEGUI::UDim> URect;
        typedef ::CEGUI::Size<CEGUI::UDim> USize;

        typedef std::vector<CEGUI::ListboxItem*
            CEGUI_VECTOR_ALLOC(CEGUI::ListboxItem*)> ListBoxItemPtrVector;
        // typedef std::vector<CEGUI::Scheme::AliasMapping> AliasMappingVector;
        typedef std::vector<CEGUI::PropertyInitialiser
            CEGUI_VECTOR_ALLOC(CEGUI::PropertyInitialiser)> PropertyInitialiserVector;
        // typedef std::vector<CEGUI::Scheme::FalagardMapping> FalagardMappingVector;
        // typedef std::vector<CEGUI::Scheme::LoadableUIElement> LoadableUIElementVector;
        // typedef std::vector<CEGUI::Scheme::UIElementFactory> UIElementFactoryVector;
        // typedef std::vector<CEGUI::Scheme::UIModule> UIModuleVector;

        typedef std::vector<CEGUI::TextComponent* CEGUI_VECTOR_ALLOC(CEGUI::TextComponent*)> TextComponentVector;
        typedef std::vector<CEGUI::ImageryComponent* CEGUI_VECTOR_ALLOC(CEGUI::ImageryComponent*)> ImageryComponentVector;
        typedef std::vector<CEGUI::FrameComponent* CEGUI_VECTOR_ALLOC(CEGUI::FrameComponent*)> FrameComponentVector;
        typedef std::vector<CEGUI::LayerSpecification* CEGUI_VECTOR_ALLOC(CEGUI::LayerSpecification*)> LayerSpecificationVector;
        typedef std::vector<CEGUI::SectionSpecification* CEGUI_VECTOR_ALLOC(CEGUI::SectionSpecification*)> SectionSpecificationVector;

        typedef ::CEGUI::Event::Connection Connection;
        typedef ::CEGUI::Event::Subscriber Subscriber;
        typedef ::CEGUI::EventSet::EventIterator EventIterator;

        typedef ::CEGUI::FontManager::FontIterator FontIterator;
        typedef ::CEGUI::ImageManager::ImageIterator ImageIterator;
        typedef ::CEGUI::PropertySet::PropertyIterator PropertyIterator;
        typedef ::CEGUI::Scheme::LoadableUIElementIterator LoadableUIElementIterator;
        typedef ::CEGUI::SchemeManager::SchemeIterator SchemeIterator;
        typedef ::CEGUI::WindowFactoryManager::WindowFactoryIterator WindowFactoryIterator;
        typedef ::CEGUI::WindowFactoryManager::TypeAliasIterator TypeAliasIterator;
        typedef ::CEGUI::WindowFactoryManager::FalagardMappingIterator FalagardMappingIterator;
        typedef ::CEGUI::WindowManager::WindowIterator WindowIterator;
        typedef ::CEGUI::WidgetLookFeel::PropertyDefinitionIterator PropertyDefinitionIterator;

        typedef std::pair<float, float> StdPairFloatFloat;
        typedef std::pair<CEGUI::String, CEGUI::String> StdPairCEGUIStringString;
        typedef std::pair<CEGUI::Image*, CEGUI::ImageFactory*> StdPairCEGUIImageImageFactory;

        typedef std::map<std::string, std::string
            CEGUI_MAP_ALLOC(std::string, std::string)> StringMap;
        typedef std::vector<std::string
            CEGUI_VECTOR_ALLOC(std::string)> StringVector;

        typedef CEGUI::Singleton<CEGUI::AnimationManager> SingletonAnimationManager;
        typedef CEGUI::Singleton<CEGUI::Logger> SingletonLogger;
        typedef CEGUI::Singleton<CEGUI::FontManager> SingletonFontManager;
        typedef CEGUI::Singleton<CEGUI::GlobalEventSet> SingletonGlobalEventSet;
        typedef CEGUI::Singleton<CEGUI::ImageManager> SingletonImageManager;
        typedef CEGUI::Singleton<CEGUI::MouseCursor> SingletonMouseCursor;
        typedef CEGUI::Singleton<CEGUI::RenderEffectManager> SingletonRenderEffectManager;
        typedef CEGUI::Singleton<CEGUI::SchemeManager> SingletonSchemeManager;
        typedef CEGUI::Singleton<CEGUI::System> SingletonSystem;
        typedef CEGUI::Singleton<CEGUI::WidgetLookManager> SingletonWidgetLookManager;
        typedef CEGUI::Singleton<CEGUI::WindowFactoryManager> SingletonWindowFactoryManager;
        typedef CEGUI::Singleton<CEGUI::WindowManager> SingletonWindowManager;
        typedef CEGUI::Singleton<CEGUI::WindowRendererManager> SingletonWindowRendererManager;

        typedef unsigned int Group;
        typedef std::map<CEGUI::String, CEGUI::String, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::String)> ResourceGroupMap;
        typedef CEGUI::RefCounted<CEGUI::BoundSlot> Connection;
        typedef CEGUI::SubscriberSlot Subscriber;
        typedef std::multimap<Group, Connection
            CEGUI_MULTIMAP_ALLOC(Group, Connection)> SlotContainer;
        typedef std::map<CEGUI::String, CEGUI::Event*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::String)>  EventMap;
        typedef std::map<CEGUI::String, CEGUI::Font*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::Font*)> FontRegistry;
        typedef std::pair<CEGUI::Window*, bool> WindowStackEntry;
        typedef std::vector<WindowStackEntry
            CEGUI_VECTOR_ALLOC(WindowStackEntry)> WindowStack;
        typedef std::map<CEGUI::String, CEGUI::Property*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::Property*)> PropertyRegistry;
        typedef std::map<CEGUI::String, CEGUI::String, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::String)> UserStringMap;
        typedef std::set<CEGUI::String, CEGUI::StringFastLessCompare> StringSet;
        typedef std::vector<CEGUI::String
            CEGUI_VECTOR_ALLOC(CEGUI::String)> TargetTypeStack;     //!< Type used to implement stack of target type names.
        typedef std::map<CEGUI::String, CEGUI::Window*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::Window*)> WindowRegistry; //!< Type used to implement registry of Window objects
        typedef std::vector<CEGUI::Window*
            CEGUI_VECTOR_ALLOC(CEGUI::Window*)>    WindowVector;   //!< Type to use for a collection of Window pointers.
        typedef std::vector<CEGUI::Property*
            CEGUI_VECTOR_ALLOC(CEGUI::Property*)> PropertyList;
        typedef std::map<CEGUI::String, CEGUI::WindowRendererFactory*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::WindowRendererFactory*)> WR_Registry;
        typedef std::map<CEGUI::String, CEGUI::String, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::String)> AttributeMap;


        typedef std::map<CEGUI::String, CEGUI::EventLinkDefinition*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::EventLinkDefinition*)> EventLinkDefinitionMap;
        typedef std::map<CEGUI::String, CEGUI::ImagerySection*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::ImagerySection*)> ImagerySectionMap;
        typedef std::map<CEGUI::String, CEGUI::NamedArea*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::NamedArea*)> NamedAreaMap;
        typedef std::map<CEGUI::String, CEGUI::PropertyDefinitionBase*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::PropertyDefinitionBase*)> PropertyDefinitionBaseMap;
        typedef std::map<CEGUI::String, CEGUI::PropertyInitialiser*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::PropertyInitialiser*)> PropertyInitialiserMap;
        typedef std::map<CEGUI::String, CEGUI::StateImagery*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::StateImagery*)> StateImageryMap;
        typedef std::map<CEGUI::String, CEGUI::WidgetComponent*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::WidgetComponent*)> WidgetComponentMap;
        typedef std::map<CEGUI::String, CEGUI::WidgetLookFeel*, CEGUI::StringFastLessCompare
            CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::WidgetLookFeel*)> WidgetLookFeelMap;

        typedef CEGUI::ConstVectorIterator<std::vector<CEGUI::PropertyInitialiser
           CEGUI_VECTOR_ALLOC(CEGUI::PropertyInitialiser)> > PropertyInitialiserVectorIterator;

        typedef CEGUI::TypedProperty<float> TypedPropertyFloat;
        typedef CEGUI::FalagardPropertyBase<float> FalagardPropertyBaseFloat;
        typedef CEGUI::PropertyDefinition<float> PropertyDefinitionFloat;
        typedef CEGUI::PropertyLinkDefinition<float> PropertyLinkDefinitionFloat;

        typedef CEGUI::TypedProperty<unsigned int> TypedPropertyUint;
        typedef CEGUI::FalagardPropertyBase<unsigned int> FalagardPropertyBaseUint;
        typedef CEGUI::PropertyDefinition<unsigned int> PropertyDefinitionUint;
        typedef CEGUI::PropertyLinkDefinition<unsigned int> PropertyLinkDefinitionUint;

        typedef CEGUI::TypedProperty<bool> TypedPropertyBool;
        typedef CEGUI::FalagardPropertyBase<bool> FalagardPropertyBaseBool;
        typedef CEGUI::PropertyDefinition<bool> PropertyDefinitionBool;
        typedef CEGUI::PropertyLinkDefinition<bool> PropertyLinkDefinitionBool;

        typedef CEGUI::TypedProperty<CEGUI::Sizef> TypedPropertySizef;
        typedef CEGUI::FalagardPropertyBase<CEGUI::Sizef> FalagardPropertyBaseSizef;
        typedef CEGUI::PropertyDefinition<CEGUI::Sizef> PropertyDefinitionSizef;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::Sizef> PropertyLinkDefinitionSizef;

        typedef CEGUI::TypedProperty<CEGUI::Vector2f> TypedPropertyVector2f;
        typedef CEGUI::FalagardPropertyBase<CEGUI::Vector2f> FalagardPropertyBaseVector2f;
        typedef CEGUI::PropertyDefinition<CEGUI::Vector2f> PropertyDefinitionVector2f;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::Vector2f> PropertyLinkDefinitionVector2f;

        typedef CEGUI::TypedProperty<CEGUI::Rectf> TypedPropertyRectf;
        typedef CEGUI::FalagardPropertyBase<CEGUI::Rectf> FalagardPropertyBaseRectf;
        typedef CEGUI::PropertyDefinition<CEGUI::Rectf> PropertyDefinitionRectf;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::Rectf> PropertyLinkDefinitionRectf;

        typedef CEGUI::TypedProperty<CEGUI::Image*> TypedPropertyImage;
        typedef CEGUI::FalagardPropertyBase<CEGUI::Image*> FalagardPropertyBaseImage;
        typedef CEGUI::PropertyDefinition<CEGUI::Image*> PropertyDefinitionImage;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::Image*> PropertyLinkDefinitionImage;

        typedef CEGUI::TypedProperty<CEGUI::Colour> TypedPropertyColour;
        typedef CEGUI::FalagardPropertyBase<CEGUI::Colour> FalagardPropertyBaseColour;
        typedef CEGUI::PropertyDefinition<CEGUI::Colour> PropertyDefinitionColour;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::Colour> PropertyLinkDefinitionColour;

        typedef CEGUI::TypedProperty<CEGUI::ColourRect> TypedPropertyColourRect;
        typedef CEGUI::FalagardPropertyBase<CEGUI::ColourRect> FalagardPropertyBaseColourRect;
        typedef CEGUI::PropertyDefinition<CEGUI::ColourRect> PropertyDefinitionColourRect;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::ColourRect> PropertyLinkDefinitionColourRect;

        typedef CEGUI::TypedProperty<CEGUI::UDim> TypedPropertyColourUDim;
        typedef CEGUI::FalagardPropertyBase<CEGUI::UDim> FalagardPropertyBaseColourUDim;
        typedef CEGUI::PropertyDefinition<CEGUI::UDim> PropertyDefinitionColourUDim;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::UDim> PropertyLinkDefinitionColourUDim;

        typedef CEGUI::TypedProperty<CEGUI::UVector2> TypedPropertyColourUVector2;
        typedef CEGUI::FalagardPropertyBase<CEGUI::UVector2> FalagardPropertyBaseColourUVector2;
        typedef CEGUI::PropertyDefinition<CEGUI::UVector2> PropertyDefinitionColourUVector2;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::UVector2> PropertyLinkDefinitionColourUVector2;

        typedef CEGUI::TypedProperty<CEGUI::URect> TypedPropertyColourURect;
        typedef CEGUI::FalagardPropertyBase<CEGUI::URect> FalagardPropertyBaseColourURect;
        typedef CEGUI::PropertyDefinition<CEGUI::URect> PropertyDefinitionColourURect;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::URect> PropertyLinkDefinitionColourURect;

        typedef CEGUI::TypedProperty<CEGUI::UBox> TypedPropertyColourUBox;
        typedef CEGUI::FalagardPropertyBase<CEGUI::UBox> FalagardPropertyBaseColourUBox;
        typedef CEGUI::PropertyDefinition<CEGUI::UBox> PropertyDefinitionColourUBox;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::UBox> PropertyLinkDefinitionColourUBox;

        typedef CEGUI::TypedProperty<CEGUI::String> TypedPropertyString;
        typedef CEGUI::FalagardPropertyBase<CEGUI::String> FalagardPropertyBaseString;
        typedef CEGUI::PropertyDefinition<CEGUI::String> PropertyDefinitionString;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::String> PropertyLinkDefinitionString;

        typedef CEGUI::TypedProperty<CEGUI::VerticalTextFormatting> TypedPropertyVerticalTextFormatting;
        typedef CEGUI::FalagardPropertyBase<CEGUI::VerticalTextFormatting> FalagardPropertyBaseVerticalTextFormatting;
        typedef CEGUI::PropertyDefinition<CEGUI::VerticalTextFormatting> PropertyDefinitionVerticalTextFormatting;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::VerticalTextFormatting> PropertyLinkDefinitionVerticalTextFormatting;

        typedef CEGUI::TypedProperty<CEGUI::HorizontalTextFormatting> TypedPropertyHorizontalTextFormatting;
        typedef CEGUI::FalagardPropertyBase<CEGUI::HorizontalTextFormatting> FalagardPropertyBaseHorizontalTextFormatting;
        typedef CEGUI::PropertyDefinition<CEGUI::HorizontalTextFormatting> PropertyDefinitionHorizontalTextFormatting;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::HorizontalTextFormatting> PropertyLinkDefinitionHorizontalTextFormatting;

        typedef CEGUI::TypedProperty<CEGUI::VerticalFormatting> TypedPropertyVerticalFormatting;
        typedef CEGUI::FalagardPropertyBase<CEGUI::VerticalFormatting> FalagardPropertyBaseVerticalFormatting;
        typedef CEGUI::PropertyDefinition<CEGUI::VerticalFormatting> PropertyDefinitionVerticalFormatting;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::VerticalFormatting> PropertyLinkDefinitionVerticalFormatting;

        typedef CEGUI::TypedProperty<CEGUI::HorizontalFormatting> TypedPropertyHorizontalFormatting;
        typedef CEGUI::FalagardPropertyBase<CEGUI::HorizontalFormatting> FalagardPropertyBaseHorizontalFormatting;
        typedef CEGUI::PropertyDefinition<CEGUI::HorizontalFormatting> PropertyDefinitionHorizontalFormatting;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::HorizontalFormatting> PropertyLinkDefinitionHorizontalFormatting;

        typedef CEGUI::TypedProperty<CEGUI::Font*> TypedPropertyFont;
        typedef CEGUI::FalagardPropertyBase<CEGUI::Font*> FalagardPropertyBaseFont;
        typedef CEGUI::PropertyDefinition<CEGUI::Font*> PropertyDefinitionFont;
        typedef CEGUI::PropertyLinkDefinition<CEGUI::Font*> PropertyLinkDefinitionFont;
    }
}

// then we exposed everything needed (and more) to ensure GCCXML makes them visible to Py++
//

// this is a gcc-only workaround, don't do this at build time
#ifdef __GCCXML__

namespace python_CEGUI
{
    namespace details
    {
        inline void instantiate()
        {
            sizeof( ::std::pair<float, float>);
            sizeof( ::CEGUI::Event::Connection);
            sizeof( ::CEGUI::Event::Subscriber);

            sizeof( ::CEGUI::FontManager::FontIterator);
            sizeof( ::CEGUI::ImageManager::ImageIterator);
            sizeof( ::CEGUI::PropertySet::PropertyIterator);
            sizeof( ::CEGUI::Scheme::LoadableUIElementIterator);
            sizeof( ::CEGUI::SchemeManager::SchemeIterator);
            sizeof( ::CEGUI::WindowFactoryManager::WindowFactoryIterator);
            sizeof( ::CEGUI::WindowFactoryManager::TypeAliasIterator);
            sizeof( ::CEGUI::WindowFactoryManager::FalagardMappingIterator);
            sizeof( ::CEGUI::WindowManager::WindowIterator);
            sizeof( ::CEGUI::EventSet::EventIterator );
            sizeof( ::CEGUI::String::iterator );
            sizeof( ::CEGUI::Vector2< float > );
            sizeof( ::CEGUI::Vector3< float > );
            sizeof( ::CEGUI::Size< float > );
            sizeof( ::CEGUI::Rect< float > );
            sizeof( ::CEGUI::Vector2< CEGUI::UDim > );
            sizeof( ::CEGUI::Rect< CEGUI::UDim > );
            sizeof( ::CEGUI::Size< CEGUI::UDim > );

            sizeof( ::std::pair<float, float> );
            sizeof( ::std::pair<CEGUI::String, CEGUI::String> );
            sizeof( ::std::pair<CEGUI::Image*, CEGUI::ImageFactory*> );

            // seems that doing an sizeof ( ::CEGUI::EventSet::Iterator ); doesn't expose the class :(
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::Property*, CEGUI::StringFastLessCompare   CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::Property*) > > );
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::Scheme*, CEGUI::StringFastLessCompare     CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::Scheme*) > > );
            //sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::Window*, CEGUI::StringFastLessCompare     CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::Window*) > > );
            sizeof( CEGUI::ConstVectorIterator<std::vector<CEGUI::Window*  CEGUI_VECTOR_ALLOC(Window*)> >);
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::WindowFactoryManager::FalagardWindowMapping, CEGUI::StringFastLessCompare CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::WindowFactoryManager::FalagardWindowMapping) > > );
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::Event*, CEGUI::StringFastLessCompare      CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::Event*) > > );
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::WindowFactoryManager::AliasTargetStack, CEGUI::StringFastLessCompare CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::WindowFactoryManager::AliasTargetStack) > > );
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::WindowFactory*, CEGUI::StringFastLessCompare CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::WindowFactory*) > > );
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::Font*, CEGUI::StringFastLessCompare       CEGUI_MAP_ALLOC(CEGUI::String, CEGUI::Font*) > > );
            sizeof( CEGUI::ConstVectorIterator<std::multiset<CEGUI::LayerSpecification, std::less<CEGUI::LayerSpecification>  CEGUI_VECTOR_ALLOC(CEGUI::LayerSpecification) > > );
            sizeof( CEGUI::ConstVectorIterator<std::vector<CEGUI::ImageryComponent  CEGUI_VECTOR_ALLOC(CEGUI::ImageryComponent) > > );
            sizeof( CEGUI::ConstVectorIterator<std::vector<CEGUI::EventAction  CEGUI_VECTOR_ALLOC(CEGUI::EventAction) > > );
            sizeof( CEGUI::ConstVectorIterator<std::set<CEGUI::String, CEGUI::StringFastLessCompare, std::allocator<CEGUI::String> > > );
            sizeof( CEGUI::ConstVectorIterator<std::vector<CEGUI::FrameComponent  CEGUI_VECTOR_ALLOC(CEGUI::FrameComponent) > > );
            sizeof( CEGUI::ConstVectorIterator<CEGUI::NamedDefinitionCollator<CEGUI::String, const CEGUI::PropertyInitialiser*> > );
            sizeof( CEGUI::ConstVectorIterator<CEGUI::NamedDefinitionCollator<CEGUI::String, CEGUI::PropertyDefinitionBase*> > );
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::ImagerySection, CEGUI::StringFastLessCompare  CEGUI_MAP_ALLOC(const CEGUI::String, CEGUI::ImagerySection) > > );
            //sizeof( CEGUI::ConstVectorIterator<std::vector<CEGUI::PropertyInitialiser  CEGUI_VECTOR_ALLOC(CEGUI::PropertyInitialiser) > > );
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::NamedArea, CEGUI::StringFastLessCompare  CEGUI_MAP_ALLOC(const CEGUI::String, CEGUI::NamedArea) > > );
            sizeof( CEGUI::ConstVectorIterator<CEGUI::NamedDefinitionCollator<CEGUI::String, const CEGUI::WidgetComponent*> > );
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::WidgetLookFeel, CEGUI::StringFastLessCompare  CEGUI_MAP_ALLOC(const CEGUI::String, CEGUI::WidgetLookFeel) > > );
            sizeof( CEGUI::ConstVectorIterator<CEGUI::NamedDefinitionCollator<CEGUI::String, const CEGUI::EventLinkDefinition*> > );
            sizeof( CEGUI::ConstMapIterator<std::map<CEGUI::String, CEGUI::StateImagery, CEGUI::StringFastLessCompare  CEGUI_MAP_ALLOC(const CEGUI::String, CEGUI::StateImagery) > > );
            sizeof( CEGUI::ConstVectorIterator<std::vector<CEGUI::SectionSpecification  CEGUI_VECTOR_ALLOC(CEGUI::SectionSpecification) > > );
            sizeof( CEGUI::ConstVectorIterator<std::vector<CEGUI::TextComponent  CEGUI_VECTOR_ALLOC(CEGUI::TextComponent) > > );

            sizeof( CEGUI::TypedProperty<float> );
            sizeof( CEGUI::FalagardPropertyBase<float> );
            sizeof( CEGUI::PropertyDefinition<float> );
            sizeof( CEGUI::PropertyLinkDefinition<float> );

            sizeof( CEGUI::TypedProperty<unsigned int> );
            sizeof( CEGUI::FalagardPropertyBase<unsigned int> );
            sizeof( CEGUI::PropertyDefinition<unsigned int> );
            sizeof( CEGUI::PropertyLinkDefinition<unsigned int> );

            sizeof( CEGUI::TypedProperty<bool> );
            sizeof( CEGUI::FalagardPropertyBase<bool> );
            sizeof( CEGUI::PropertyDefinition<bool> );
            sizeof( CEGUI::PropertyLinkDefinition<bool> );

            sizeof( CEGUI::TypedProperty<CEGUI::Sizef> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::Sizef> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::Sizef> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::Sizef> );

            sizeof( CEGUI::TypedProperty<CEGUI::Vector2f> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::Vector2f> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::Vector2f> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::Vector2f> );

            sizeof( CEGUI::TypedProperty<CEGUI::Rectf> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::Rectf> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::Rectf> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::Rectf> );

            sizeof( CEGUI::TypedProperty<CEGUI::Image*> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::Image*> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::Image*> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::Image*> );

            sizeof( CEGUI::TypedProperty<CEGUI::Colour> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::Colour> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::Colour> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::Colour> );

            sizeof( CEGUI::TypedProperty<CEGUI::ColourRect> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::ColourRect> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::ColourRect> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::ColourRect> );

            sizeof( CEGUI::TypedProperty<CEGUI::UDim> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::UDim> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::UDim> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::UDim> );

            sizeof( CEGUI::TypedProperty<CEGUI::UVector2> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::UVector2> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::UVector2> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::UVector2> );

            sizeof( CEGUI::TypedProperty<CEGUI::URect> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::URect> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::URect> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::URect> );

            sizeof( CEGUI::TypedProperty<CEGUI::UBox> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::UBox> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::UBox> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::UBox> );

            sizeof( CEGUI::TypedProperty<CEGUI::String> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::String> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::String> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::String> );

            sizeof( CEGUI::TypedProperty<CEGUI::VerticalTextFormatting> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::VerticalTextFormatting> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::VerticalTextFormatting> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::VerticalTextFormatting> );

            sizeof( CEGUI::TypedProperty<CEGUI::HorizontalTextFormatting> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::HorizontalTextFormatting> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::HorizontalTextFormatting> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::HorizontalTextFormatting> );

            sizeof( CEGUI::TypedProperty<CEGUI::VerticalFormatting> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::VerticalFormatting> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::VerticalFormatting> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::VerticalFormatting> );

            sizeof( CEGUI::TypedProperty<CEGUI::HorizontalFormatting> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::HorizontalFormatting> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::HorizontalFormatting> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::HorizontalFormatting> );

            sizeof( CEGUI::TypedProperty<CEGUI::Font*> );
            sizeof( CEGUI::FalagardPropertyBase<CEGUI::Font*> );
            sizeof( CEGUI::PropertyDefinition<CEGUI::Font*> );
            sizeof( CEGUI::PropertyLinkDefinition<CEGUI::Font*> );
        }
    }
}

#endif

