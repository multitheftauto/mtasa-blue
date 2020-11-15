#!/usr/bin/env python2

#/***********************************************************************
#    filename:   generateCEGUI.py
#    created:    13/8/2010
#    author:     Martin Preisler (with many bits taken from python ogre)
#
#    purpose:    Generates CEGUI binding code
#*************************************************************************/
#/***************************************************************************
# *   Copyright (C) 2004 - 2013 Paul D Turner & The CEGUI Development Team
# *
# *   Thanks to Roman Yakovenko for advices and great work on Py++!
# *   Thanks to Andy Miller for his python-ogre CEGUI bindings!
# *
# *   License: generator is GPL3 (python ogre code generators are also GPL)
# *            generated code is MIT as the rest of CEGUI

import os

from pyplusplus import function_transformers as ft
from pyplusplus import messages
from pyplusplus.module_builder import call_policies
from pygccxml import declarations

import common_utils

PACKAGE_NAME = "PyCEGUI"
PACKAGE_VERSION = common_utils.GLOBAL_PACKAGE_VERSION
MODULE_NAME = PACKAGE_NAME

OUTPUT_DIR = os.path.join(common_utils.OUTPUT_DIR, "CEGUI")

def filterDeclarations(mb):
    # by default we exclude everything and only include what we WANT in the module
    mb.global_ns.exclude()

    # because of std::pair<float, float> CEGUI::Thumb::getHorzRange()
    mb.global_ns.namespace("std").class_("pair<float, float>").include()
    mb.global_ns.namespace("std").class_("pair<CEGUI::String, CEGUI::String>").include()
    mb.global_ns.namespace("std").class_("pair<CEGUI::Image*, CEGUI::ImageFactory*>").include()

    CEGUI_ns = mb.global_ns.namespace("CEGUI")

    ### CORE ###

    # Affector.h
    affector = CEGUI_ns.class_("Affector")
    affector.include()

    # Animation.h
    animation = CEGUI_ns.class_("Animation")
    animation.include()

    # Animation_xmlHandler.h
    # not interesting for python users

    # AnimationInstance.h
    animationInstance = CEGUI_ns.class_("AnimationInstance")
    animationInstance.include()

    # AnimationManager.h
    animationManager = CEGUI_ns.class_("AnimationManager")
    animationManager.include()

    # Base.h
    # nothing interesting for python users
    # todo: maybe pixel align?

    # BasicImage.h
    basicImage = CEGUI_ns.class_("BasicImage")
    basicImage.include()

    # BasicInterpolators.h
    # nothing interesting for python users

    # BasicRenderedStringParser.h
    # todo: is this interesting for python?
    basicRenderedStringParser = CEGUI_ns.class_("BasicRenderedStringParser")
    basicRenderedStringParser.include()

    # BiDiVisualMapping.h
    # Not exposed since this might be disabled at configure time
    #CEGUI_ns.enum("BidiCharType").include()
    #bidiVisualMapping = CEGUI_ns.class_("BidiVisualMapping")
    #bidiVisualMapping.include()

    # BoundSlot.h
    boundSlot = CEGUI_ns.class_("BoundSlot")
    boundSlot.include()
    # also include ref counted variant
    CEGUI_ns.class_("RefCounted<CEGUI::BoundSlot>").include()

    # CentredRenderedString.h
    # todo: is this interesting for python?
    centredRenderedString = CEGUI_ns.class_("CentredRenderedString")
    centredRenderedString.include()

    # ChainedXMLHandler.h
    #chainedXMLHandler = CEGUI_ns.class_("ChainedXMLHandler")
    #chainedXMLHandler.include()

    # Clipboard.h
    #nativeClipboardProvider = CEGUI_ns.class_("NativeClipboardProvider")
    #nativeClipboardProvider.include()
    clipboard = CEGUI_ns.class_("Clipboard")
    clipboard.include()
    # we don't allow native clipboard subclassing/setting from python (for now?)
    clipboard.mem_fun("setNativeProvider").exclude()
    clipboard.mem_fun("getNativeProvider").exclude()

    # exclude methods that are hard to use from within python
    clipboard.mem_fun("setData").exclude()
    clipboard.mem_fun("getData").exclude()

    # Colour.h
    colour = CEGUI_ns.class_("Colour")
    colour.include()

    # ColourRect.h
    colourRect = CEGUI_ns.class_("ColourRect")
    colourRect.include()

    # Config.h
    # nothing interesting for python

    # Config_xmlHandler.h
    # nothing interesting for python

    # CoordConverter.h
    coordConverter = CEGUI_ns.class_("CoordConverter")
    coordConverter.include()

    # DataContainer.h
    rawDataContainer = CEGUI_ns.class_("RawDataContainer")
    rawDataContainer.include()
    rawDataContainer.mem_funs().exclude()

    # DefaultLogger.h
    defaultLogger = CEGUI_ns.class_("DefaultLogger")
    defaultLogger.include()

    # DefaultRenderedStringParser.h
    # not interesting for python

    # DefaultResourceProvider.h
    defaultResourceProvider = CEGUI_ns.class_("DefaultResourceProvider")
    defaultResourceProvider.include()

    # DynamicModule.h
    # not doable in python

    # /Element.h
    element = CEGUI_ns.class_("Element")
    element.include()
    element.class_("CachedRectf").constructors().exclude()

    # Event.h
    event = CEGUI_ns.class_("Event")
    event.include()

    # EventArgs.h
    # handled separately, all classes ending with "EventArgs" are included

    # EventSet.h
    eventSet = CEGUI_ns.class_("EventSet")
    eventSet.include()
    # this is done via custom code
    eventSet.mem_funs("subscribeEvent").exclude()
    eventSet.noncopyable = True

    # we use depth first to make sure classes "deep in the hierarchy" gets their casts tried first
    def collectEventArgsDerived(node):
        ret = []
        for derived in sorted(node.derived):
            ret.extend(collectEventArgsDerived(derived.related_class))

        ret.append(node)

        return ret

    eventArgsCastCode = ""
    for derived in collectEventArgsDerived(CEGUI_ns.class_("EventArgs")):
        eventArgsCastCode += "        if (dynamic_cast<const CEGUI::%s*>(&args))\n" % (derived.name)
        eventArgsCastCode += "        {\n"
        eventArgsCastCode += "            return boost::python::call<bool>(d_callable, static_cast<const CEGUI::%s&>(args));\n" % (derived.name)
        eventArgsCastCode += "        }\n\n"

    eventSet.add_declaration_code(
"""
class PythonEventSubscription
{
public:
    PythonEventSubscription(PyObject* callable):
        d_callable(boost::python::incref(callable))
    {}

    PythonEventSubscription(const PythonEventSubscription& other):
        d_callable(boost::python::incref(other.d_callable))
    {}

    ~PythonEventSubscription()
    {
        boost::python::decref(d_callable);
    }

    bool operator() (const CEGUI::EventArgs& args) const
    {
        // FIXME: We have to cast, otherwise only base class gets to python!

        // I don't understand why this is happening, I think boost::python should use typeid(args).name() and deduce that it's a
        // derived class, not CEGUI::EventArgs base class
        // However this is not happening so I have to go through all EventArgs classes and try casting one after another
""" + eventArgsCastCode + """

        // We could not cast to anything!
        return false;
    }

    PyObject* d_callable;
};

class PythonEventConnection
{
public:
    PythonEventConnection():
        d_connection()
    {}

    PythonEventConnection(const CEGUI::Event::Connection& connection):
        d_connection(connection)
    {}

    PythonEventConnection(const PythonEventConnection& v):
        d_connection(v.d_connection)
    {}

    bool connected()
    {
        return d_connection.isValid() ? d_connection->connected() : false;
    }

    void disconnect()
    {
        // TODO: Throw on invalid disconnects?
        if (d_connection.isValid())
        {
            d_connection->disconnect();
        }
    }

private:
    CEGUI::Event::Connection d_connection;
};

PythonEventConnection EventSet_subscribeEvent(CEGUI::EventSet* self, const CEGUI::String& name, PyObject* callable)
{
    return PythonEventConnection(self->subscribeEvent(name, PythonEventSubscription(callable)));
}
"""
    )
    eventSet.add_registration_code(
"""
def( "subscribeEvent", &EventSet_subscribeEvent);

    {   // PythonEventConnection

        typedef bp::class_< PythonEventConnection > PythonEventConnection_exposer_t;
        PythonEventConnection_exposer_t PythonEventConnection_exposer = PythonEventConnection_exposer_t( "PythonEventConnection" );
        bp::scope PythonEventConnection_scope( PythonEventConnection_exposer );
        PythonEventConnection_exposer.def( bp::init<>() );
        {

            typedef bool ( PythonEventConnection::*connected_function_type )(  ) ;

            PythonEventConnection_exposer.def(
                "connected"
                , connected_function_type( &PythonEventConnection::connected ) );

        }
        {

            typedef void ( PythonEventConnection::*disconnect_function_type )(  ) ;

            PythonEventConnection_exposer.def(
                "disconnect"
                , disconnect_function_type( &PythonEventConnection::disconnect ) );
        }
    }
"""
    )

    # Exceptions.h
    # handled separately

    # FactoryModule.h
    # not doable in python

    # Font.h
    font = CEGUI_ns.class_("Font", recursive = False)
    font.include()

    # Font_xmlHandler.h
    # not interesting for python

    # FontGlyph.h
    fontGlyph = CEGUI_ns.class_("FontGlyph")
    fontGlyph.include()

    # FontManager.h
    fontManager = CEGUI_ns.class_("FontManager")
    fontManager.include()

    # FormattedRenderedString.
    # todo: is this interesting for python?

    # ForwardRefs.h
    # hell no

    # FreeFunctionSlot.h
    # handled differently elsewhere, see EventSet

    # FreeTypeFont.h
    # not interesting for python

    # FribidiVisualMapping.h
    # not interesting for python

    # FunctorCopySlot.h
    # handled differently elsewhere, see EventSet

    # FunctorPointerSlot.h
    # handled differently elsewhere, see EventSet

    # FunctorReferenceBinder.h
    # handled differently elsewhere, see EventSet

    # FunctorReferenceSlot.h
    # handled differently elsewhere, see EventSet

    # GeometryBuffer.h
    geometryBuffer = CEGUI_ns.class_("GeometryBuffer")
    geometryBuffer.include()

    # GlobalEventSet.h
    globalEventSet = CEGUI_ns.class_("GlobalEventSet")
    globalEventSet.include()

    # GUIContext.h
    guiContext = CEGUI_ns.class_("GUIContext")
    guiContext.include()
    for decl in guiContext.mem_funs("getMouseCursor"):
        if decl.has_const:
            decl.exclude()

    # GUILayout_xmlHandler.h
    # not needed in python

    # Image.h
    autoScaledModeEnum = CEGUI_ns.enum("AutoScaledMode")
    autoScaledModeEnum.include()
    image = CEGUI_ns.class_("Image")
    image.include()

    # ImageCodec.h
    imageCodec = CEGUI_ns.class_("ImageCodec")
    imageCodec.include()

    # ImageManager.h
    imageManager = CEGUI_ns.class_("ImageManager")
    imageManager.include()

    # InputEvent.h
    key = CEGUI_ns.class_("Key")
    key.include()

    mouseButtonEnum = CEGUI_ns.enum("MouseButton")
    mouseButtonEnum.include()

    # Interpolator.h
    interpolator = CEGUI_ns.class_("Interpolator")
    interpolator.include()

    # IteratorBase.h
    # all iterators are sorted later

    # JustifiedRenderedString.h
    # not needed in python

    # KeyFrame.h
    keyFrame = CEGUI_ns.class_("KeyFrame")
    keyFrame.include()

    # LeftAlignedRenderedString.h
    # not needed in python

    # Logger.h
    logger = CEGUI_ns.class_("Logger")
    logger.include()

    loggingLevelEnum = CEGUI_ns.enum("LoggingLevel")
    loggingLevelEnum.include()

    # MemberFunctionSlot.h
    # sorted elsewhere, see EventSet

    # MinibidiVisualMapping.h
    # not needed for python

    # MinizipResourceProvider.h
    # not needed for python

    # MouseCursor.h
    mouseCursor = CEGUI_ns.class_("MouseCursor")
    mouseCursor.include()
    mouseCursor.noncopyable = True

    # NamedElement.h
    namedElement = CEGUI_ns.class_("NamedElement")
    namedElement.include()

    # NamedXMLResourceManager.h
    xmlResourceExistsActionEnum = CEGUI_ns.enum("XMLResourceExistsAction")
    xmlResourceExistsActionEnum.include()

    resourceEventSet = CEGUI_ns.class_("ResourceEventSet")
    resourceEventSet.include()

    # PCRERegexMatcher.h
    # not needed in python

    # PixmapFont.h
    # not needed in python

    # Property.h
    propertyReceiver = CEGUI_ns.class_("PropertyReceiver")
    propertyReceiver.include()

    property = CEGUI_ns.class_("Property")
    property.include()

    # PropertyHelper.h
    propertyHelper = CEGUI_ns.class_("PropertyHelper_wrapper")
    propertyHelper.include()
    propertyHelper.rename("PropertyHelper")

    # PropertySet.h
    propertySet = CEGUI_ns.class_("PropertySet")
    propertySet.include()

    # Quaternion.h
    quaternion = CEGUI_ns.class_("Quaternion")
    quaternion.include()

    # Rect.h
    rectf = CEGUI_ns.class_("Rect<float>")
    rectf.rename("Rectf")
    rectf.include()

    urect = CEGUI_ns.class_("Rect<CEGUI::UDim>")
    urect.rename("URect")
    urect.include()
    # UDim doesn't have the necessary operators for this
    urect.member_function("getIntersection").exclude()
    urect.member_function("isPointInRect").exclude()
    urect.member_function("constrainSizeMin").exclude()
    urect.member_function("constrainSizeMax").exclude()
    urect.member_function("constrainSize").exclude()

    # RefCounted.h
    # handled elsewhere

    # RefexMatcher.h
    regexMatcher = CEGUI_ns.class_("RegexMatcher")
    regexMatcher.include()

    # RenderedString.h
    renderedString = CEGUI_ns.class_("RenderedString")
    renderedString.include()

    # RenderedStringComponent.h
    renderedStringComponent = CEGUI_ns.class_("RenderedStringComponent")
    renderedStringComponent.include()

    # RenderedStringImageComponent.h
    renderedStringImageComponent = CEGUI_ns.class_("RenderedStringImageComponent")
    renderedStringImageComponent.include()

    # RenderedStringTextComponent.h
    renderedStringTextComponent = CEGUI_ns.class_("RenderedStringTextComponent")
    renderedStringTextComponent.include()

    # RenderedStringParser.h
    renderedStringParser = CEGUI_ns.class_("RenderedStringParser")
    renderedStringParser.include()

    # RenderedStringWidgetComponent.h
    renderedStringWidgetComponent = CEGUI_ns.class_("RenderedStringWidgetComponent")
    renderedStringWidgetComponent.include()

    # RenderEffect.h
    renderEffect = CEGUI_ns.class_("RenderEffect")
    renderEffect.include()

    # RenderEffectFactory.h
    renderEffectFactory = CEGUI_ns.class_("RenderEffectFactory")
    renderEffectFactory.include()

    # RenderEffectManager.h
    renderEffectManager = CEGUI_ns.class_("RenderEffectManager")
    renderEffectManager.include()

    # Renderer.h
    quadSplitModeEnum = CEGUI_ns.enum("QuadSplitMode")
    quadSplitModeEnum.include()

    blendModeEnum = CEGUI_ns.enum("BlendMode")
    blendModeEnum.include()

    renderer = CEGUI_ns.class_("Renderer")
    renderer.include()

    # RenderingContext.h
    renderingContext = CEGUI_ns.class_("RenderingContext")
    renderingContext.include()

    # RenderingSurface.h
    CEGUI_ns.enum("RenderQueueID").include()
    renderingSurface = CEGUI_ns.class_("RenderingSurface")
    renderingSurface.include()

    # RenderingWindow.h
    renderingWindow = CEGUI_ns.class_("RenderingWindow")
    renderingWindow.include()

    # RenderQueue.h
    renderQueue = CEGUI_ns.class_("RenderQueue")
    renderQueue.include()

    # RenderTarget.h
    renderTarget = CEGUI_ns.class_("RenderTarget")
    renderTarget.include()

    # ResourceProvider.h
    resourceProvider = CEGUI_ns.class_("ResourceProvider")
    resourceProvider.include()

    # RightAlignedRenderedString.h
    # not needed in python

    # Scheme.h
    scheme = CEGUI_ns.class_("Scheme")
    scheme.include()

    # SchemeManager.h
    schemeManager = CEGUI_ns.class_("SchemeManager")
    schemeManager.include()

    # ScriptModule.h
    scriptModule = CEGUI_ns.class_("ScriptModule")
    scriptModule.include()

    # SimpleTimer.h
    simpleTimer = CEGUI_ns.class_("SimpleTimer")
    simpleTimer.include()

    # Singleton.h
    # handled separately

    # Size.h
    CEGUI_ns.enum("AspectMode").include()
    size = CEGUI_ns.class_("Size<float>")
    size.rename("Sizef")
    size.include()

    usize = CEGUI_ns.class_("Size<CEGUI::UDim>")
    usize.rename("USize")
    usize.include()
    # UDim doesn't have the necessary operators for this
    usize.member_function("clamp").exclude()
    usize.member_function("scaleToAspect").exclude()

    # SlotFunctorBase.h
    # not needed in python

    # String.h
    string = CEGUI_ns.class_("String")
    #string.include()

    # String.h
    stringTranscoder = CEGUI_ns.class_("StringTranscoder")
    # not useful from Python and very hard to wrap
    #stringTranscoder.include()

    # SubscriberSlot.h
    # todo: probably not needed with current setup, should we exclude this?
    subscriberSlot = CEGUI_ns.class_("SubscriberSlot")
    subscriberSlot.include()
    # exclude the constructor that takes function pointer as argument
    for c in subscriberSlot.constructors():
        if len(c.arguments) > 0:
            c.exclude()

    # System.h
    system = CEGUI_ns.class_("System")
    system.include()
    common_utils.excludeAllPrivate(system)
    system.mem_fun("getStringTranscoder").exclude()

    # SystemKeys.h
    systemKeys = CEGUI_ns.class_("SystemKeys")
    systemKeys.include()

    # Texture.h
    texture = CEGUI_ns.class_("Texture")
    texture.include()

    # TextureTarget.h
    textureTarget = CEGUI_ns.class_("TextureTarget")
    textureTarget.include()

    # TextUtils.h
    textUtils = CEGUI_ns.class_("TextUtils")
    textUtils.include()

    # TplWindowFactory.h
    # TplWindowRendererFactory.h
    # TplWRFactoryRegisterer.h
    # python doesn't like templates :-)

    # UDim.h
    udim = CEGUI_ns.class_("UDim")
    udim.include()

    ubox = CEGUI_ns.class_("UBox")
    ubox.include()

    # Vector.h
    vector2f = CEGUI_ns.class_("Vector2<float>")
    vector2f.rename("Vector2f")
    vector2f.include()

    vector3f = CEGUI_ns.class_("Vector3<float>")
    vector3f.rename("Vector3f")
    vector3f.include()

    uvector2 = CEGUI_ns.class_("Vector2<CEGUI::UDim>")
    uvector2.rename("UVector2")
    uvector2.include()

    # Version.h
    # nothing usable for python

    # Vertex.h
    vertex = CEGUI_ns.class_("Vertex")
    vertex.include()

    # WidgetModule.h
    # not needed in python

    # Window.h
    CEGUI_ns.enum("VerticalAlignment").include()
    CEGUI_ns.enum("HorizontalAlignment").include()
    CEGUI_ns.enum("WindowUpdateMode").include()

    window = CEGUI_ns.class_("Window")
    window.include()
    # BidiVisualMapping is excluded from python
    window.mem_fun("getBidiVisualMapping").exclude()
    # python doesn't like void*
    window.mem_fun("setUserData").exclude()
    window.mem_fun("getUserData").exclude()
    # todo: check that getUserData is really a python object
    window.add_declaration_code(
"""
void
Window_setUserData ( ::CEGUI::Window & me, PyObject * data ) {
    me.setUserData ( data );
    }

PyObject *
Window_getUserData ( ::CEGUI::Window & me) {
    void *  data = me.getUserData (  );
    Py_INCREF( (PyObject *) data );     // I'm passing a reference to this object so better inc the ref :)
    return  (PyObject *) data;
    }

typedef bool ( ::CEGUI::Window::*isChild_string_function_type )( const ::CEGUI::String& ) const;
typedef bool ( ::CEGUI::Window::*isChild_ptr_function_type )( const ::CEGUI::Element* ) const;

typedef bool ( ::CEGUI::Window::*isAncestor_string_function_type )( const ::CEGUI::String& ) const;
typedef bool ( ::CEGUI::Window::*isAncestor_ptr_function_type )( const ::CEGUI::Element* ) const;

typedef void ( ::CEGUI::Window::*removeChild_string_function_type )( const ::CEGUI::String& );
typedef void ( ::CEGUI::Window::*removeChild_ptr_function_type )( ::CEGUI::Element* );
"""
    )
    window.add_registration_code("""def ("setUserData", &::Window_setUserData);""")
    window.add_registration_code("""def ("getUserData", &::Window_getUserData);""")

    window.add_registration_code("""def ("isChild", isChild_string_function_type(&::CEGUI::Window::isChild));""")
    window.add_registration_code("""def ("isChild", isChild_ptr_function_type(&::CEGUI::Window::isChild));""")

    window.add_registration_code("""def ("isAncestor", isAncestor_string_function_type(&::CEGUI::Window::isAncestor));""")
    window.add_registration_code("""def ("isAncestor", isAncestor_ptr_function_type(&::CEGUI::Window::isAncestor));""")

    window.add_registration_code("""def ("removeChild", removeChild_string_function_type(&::CEGUI::Window::removeChild));""")
    window.add_registration_code("""def ("removeChild", removeChild_ptr_function_type(&::CEGUI::Window::removeChild));""")

    # WindowFactory.h
    windowFactory = CEGUI_ns.class_("WindowFactory")
    windowFactory.include()

    # WindowFactoryManager.h
    windowFactoryManager = CEGUI_ns.class_("WindowFactoryManager")
    windowFactoryManager.include()

    # WindowManager.h
    windowManager = CEGUI_ns.class_("WindowManager")
    windowManager.include()
    # we do all the layout loading in custom code because we can't use PropertyCallback in python

    windowManager.mem_fun("loadLayoutFromContainer").exclude()
    windowManager.add_declaration_code(
"""
CEGUI::Window*
WindowManager_loadLayoutFromContainer(::CEGUI::WindowManager & me,
    const CEGUI::RawDataContainer& container)
{
    return me.loadLayoutFromContainer(container);
}
"""
    )
    windowManager.add_registration_code(
"""
def ("loadLayoutFromContainer", &::WindowManager_loadLayoutFromContainer,\
        (bp::arg("source")), \
        bp::return_value_policy<bp::reference_existing_object, bp::default_call_policies>());
"""
    )

    windowManager.mem_fun("loadLayoutFromFile").exclude()
    windowManager.add_declaration_code(
"""
CEGUI::Window*
WindowManager_loadLayoutFromFile(::CEGUI::WindowManager & me,
    const CEGUI::String& filename, const CEGUI::String& resourceGroup = "")
{
    return me.loadLayoutFromFile(filename, resourceGroup);
}
"""
    )
    windowManager.add_registration_code(
"""
def ("loadLayoutFromFile", &::WindowManager_loadLayoutFromFile,\
        (bp::arg("filename"), bp::arg("resourceGroup") = ""), \
        bp::return_value_policy<bp::reference_existing_object, bp::default_call_policies>());
"""
    )

    windowManager.mem_fun("loadLayoutFromString").exclude()
    windowManager.add_declaration_code(
"""
CEGUI::Window*
WindowManager_loadLayoutFromString(::CEGUI::WindowManager & me,
    const CEGUI::String& source)
{
    return me.loadLayoutFromString(source);
}
"""
    )
    windowManager.add_registration_code(
"""
def ("loadLayoutFromString", &::WindowManager_loadLayoutFromString,\
        (bp::arg("source")), \
        bp::return_value_policy<bp::reference_existing_object, bp::default_call_policies>());
"""
    )

    # WindowProperties.h
    # not needed in python

    # WindowRenderer.h
    windowRenderer = CEGUI_ns.class_("WindowRenderer", recursive = False)
    windowRenderer.include()

    windowRendererFactory = CEGUI_ns.class_("WindowRendererFactory")
    windowRendererFactory.include()

    # WindowRendererManager.h
    windowRendererManager = CEGUI_ns.class_("WindowRendererManager")
    windowRendererManager.include()

    # WindowRendererModule.h
    # not needed in python

    # WRFactoryRegisterer.h
    # not needed in python

    # XMLAttributes.h
    xmlAttributes = CEGUI_ns.class_("XMLAttributes")
    xmlAttributes.include()

    # XMLHandler.h
    xmlHandler = CEGUI_ns.class_("XMLHandler")
    xmlHandler.include()

    # XMLParser.h
    xmlParser = CEGUI_ns.class_("XMLParser")
    xmlParser.include()

    # XMLSerializer.h
    xmlSerializer = CEGUI_ns.class_("XMLSerializer")
    xmlSerializer.include()

    # all EventArgs
    for cls in CEGUI_ns.classes():
        if cls.name.endswith("EventArgs"):
            cls.include()

    # all Iterator classes
    for cls in CEGUI_ns.classes():
        if not declarations.templates.is_instantiation(cls.name):
            continue

        tplName = declarations.templates.name(cls.name)
        if not tplName.endswith("Iterator"):
            continue

        # now cls is sure to be "Iterator" class
        cls.include()

        # we only allow default parameter-less constructors
        # se lets exclude every constructor that has parameters
        cls.constructors(lambda decl: bool(decl.arguments), allow_empty = True, recursive = False).exclude()

        if tplName.endswith("BaseIterator"):
            import hashlib

            # note: I don't like what I am doing here but it's a lazy solution to
            #       an annoying problem of windows not being able to handle long paths :-/
            #
            #       These are base classes, never instantiated anyways so I think
            #       it doesn't do much except look nasty :-D
            cls.rename("ConstBaseIterator_" + hashlib.md5(cls.name).hexdigest())

        else:
            # we have to make aliases for operator++ and operator--, these are not supported by python
            # instead of operator++, we use next(self) and instead of operator--, we use previous(self)
            cls.add_declaration_code(
"""
void Iterator_next(::CEGUI::%s& t)
{
    t++;
}

void Iterator_previous(::CEGUI::%s& t)
{
    t--;
}
""" % (cls.name, cls.name))
            cls.add_registration_code('def("next", &::Iterator_next)')
            cls.add_registration_code('def("previous", &::Iterator_previous)')

    ### ELEMENTS ###

    # we always skip the Properties files since they are of no use to python users

    # elements/ButtonBase.h
    buttonBase = CEGUI_ns.class_("ButtonBase")
    buttonBase.include()

    # elements/Combobox.h
    combobox = CEGUI_ns.class_("Combobox")
    combobox.include()

    # elements/ComboDropList.h
    comboDropList = CEGUI_ns.class_("ComboDropList")
    comboDropList.include()

    # elements/DefaultWindow.h
    defaultWindow = CEGUI_ns.class_("DefaultWindow")
    defaultWindow.include()

    # elements/DragContainer.h
    dragContainer = CEGUI_ns.class_("DragContainer")
    dragContainer.include()

    # elements/Editbox.h
    editbox = CEGUI_ns.class_("Editbox")
    editbox.include()

    # elements/FrameWindow.h
    frameWindow = CEGUI_ns.class_("FrameWindow")
    frameWindow.include()

    # elements/GridLayoutContainer.h
    gridLayoutContainer = CEGUI_ns.class_("GridLayoutContainer")
    gridLayoutContainer.include()
    # FIXME: Temporary, this should work with transformations
    gridLayoutContainer.mem_fun("mapFromIdxToGrid").exclude()

    # elements/GroupBox.h
    groupBox = CEGUI_ns.class_("GroupBox")
    groupBox.include()

    # elements/HorizontalLayoutContainer.h
    horizontalLayoutContainer = CEGUI_ns.class_("HorizontalLayoutContainer")
    horizontalLayoutContainer.include()

    # elements/ItemEntry.h
    itemEntry = CEGUI_ns.class_("ItemEntry")
    itemEntry.include()

    # elements/ItemListBase.h
    itemListBase = CEGUI_ns.class_("ItemListBase")
    itemListBase.include()
    # needs function pointer as argument
    itemListBase.mem_fun("setSortCallback").exclude()

    # elements/ItemListbox.h
    itemListbox = CEGUI_ns.class_("ItemListbox")
    itemListbox.include()

    # elements/LayoutContainer.h
    layoutContainer = CEGUI_ns.class_("LayoutContainer")
    layoutContainer.include()

    # elements/Listbox.h
    listbox = CEGUI_ns.class_("Listbox")
    listbox.include()
    # we have to handle this separately because CEGUI is nasty and will deallocate this for us unless
    # we remove it from it, also we have to prevent python from destroying the object
    listbox.mem_fun("addItem").exclude()
    listbox.mem_fun("removeItem").exclude()
    listbox.add_declaration_code(
    """
    void
    Listbox_addItem(CEGUI::Listbox& self, PyObject* item)
    {
        CEGUI::ListboxItem* nativeItem = boost::python::extract<CEGUI::ListboxItem*>(boost::python::incref(item));
        // passed from python so don't delete it!
        nativeItem->setAutoDeleted(false);

        self.addItem(nativeItem);
    }

    void
    Listbox_removeItem(CEGUI::Listbox& self, PyObject* item)
    {
        CEGUI::ListboxItem* nativeItem = boost::python::extract<CEGUI::ListboxItem*>(item);
        // don't delete it, python will take care of it
        nativeItem->setAutoDeleted(false);

        self.removeItem(nativeItem);
        boost::python::decref(item);
    }
    """
    )
    listbox.add_registration_code('def ("addItem", &::Listbox_addItem, (bp::arg("item")));')
    listbox.add_registration_code('def ("removeItem", &::Listbox_removeItem, (bp::arg("item")));')

    # elements/ListboxItem.h
    listboxItem = CEGUI_ns.class_("ListboxItem")
    listboxItem.include()

    # elements/ListboxTextItem.h
    listboxTextItem = CEGUI_ns.class_("ListboxTextItem")
    listboxTextItem.include()

    # elements/ListHeader.h
    listHeader = CEGUI_ns.class_("ListHeader")
    listHeader.include()

    # elements/ListHeaderSegment.h
    listHeaderSegment = CEGUI_ns.class_("ListHeaderSegment")
    listHeaderSegment.include()

    # elements/Menubar.h
    menubar = CEGUI_ns.class_("Menubar")
    menubar.include()

    # elements/MenuBase.h
    menuBase = CEGUI_ns.class_("MenuBase")
    menuBase.include()

    # elements/MenuItem.h
    menuItem = CEGUI_ns.class_("MenuItem")
    menuItem.include()

    # elements/MultiColumnList.h
    CEGUI_ns.class_("MCLGridRef").include()
    multiColumnList = CEGUI_ns.class_("MultiColumnList")
    multiColumnList.include()
    # internal private class
    multiColumnList.class_("ListRow").exclude()
    # internal
    multiColumnList.mem_fun("pred_descend").exclude()

    # elements/MultiLineEditbox.h
    multiLineEditbox = CEGUI_ns.class_("MultiLineEditbox")
    multiLineEditbox.include()

    # elements/PopupMenu.h
    popupMenu = CEGUI_ns.class_("PopupMenu")
    popupMenu.include()

    # elements/ProgressBar.h
    progressBar = CEGUI_ns.class_("ProgressBar")
    progressBar.include()

    # elements/PushButton.h
    pushButton = CEGUI_ns.class_("PushButton")
    pushButton.include()

    # elements/RadioButton.h
    radioButton = CEGUI_ns.class_("RadioButton")
    radioButton.include()

    # elements/ScrollablePane.h
    scrollablePane = CEGUI_ns.class_("ScrollablePane")
    scrollablePane.include()

    # elements/Scrollbar.h
    scrollbar = CEGUI_ns.class_("Scrollbar")
    scrollbar.include()

    # elements/ScrolledContainer.h
    scrolledContainer = CEGUI_ns.class_("ScrolledContainer")
    scrolledContainer.include()

    # elements/ScrolledItemListBase.h
    scrolledItemListBase = CEGUI_ns.class_("ScrolledItemListBase")
    scrolledItemListBase.include()

    # elements/SequentialLayoutContainer.h
    sequentialLayoutContainer = CEGUI_ns.class_("SequentialLayoutContainer")
    sequentialLayoutContainer.include()

    # elements/Slider.h
    slider = CEGUI_ns.class_("Slider")
    slider.include()

    # elements/Spinner.h
    spinner = CEGUI_ns.class_("Spinner")
    spinner.include()

    # elements/TabButton.h
    tabButton = CEGUI_ns.class_("TabButton")
    tabButton.include()

    # elements/TabButton.h
    tabControl = CEGUI_ns.class_("TabControl")
    tabControl.include()

    # elements/Thumb.h
    thumb = CEGUI_ns.class_("Thumb")
    thumb.include()

    # elements/Titlebar.h
    titlebar = CEGUI_ns.class_("Titlebar")
    titlebar.include()

    # elements/ToggleButton.h
    toggleButton = CEGUI_ns.class_("ToggleButton")
    toggleButton.include()

    # elements/Tooltip.h
    tooltip = CEGUI_ns.class_("Tooltip", recursive = False)
    tooltip.include()

    # elements/Tree.h
    tree = CEGUI_ns.class_("Tree")
    tree.include()
    # todo: this doesn't work, needs function transformation
    tree.mem_fun("getNextSelectedItemFromList").exclude()

    # elements/TreeItem.h
    treeItem = CEGUI_ns.class_("TreeItem")
    treeItem.include()

    # elements/VerticalLayoutContainer.h
    verticalLayoutContainer = CEGUI_ns.class_("VerticalLayoutContainer")
    verticalLayoutContainer.include()

    ### FALAGARD ###

    # falagard/FalComponentBase.h
    falagardComponentBase = CEGUI_ns.class_("FalagardComponentBase")
    falagardComponentBase.include()

    # falagard/FalDimensions.h
    CEGUI_ns.class_("BaseDim").include()
    CEGUI_ns.class_("OperatorDim").include()
    CEGUI_ns.class_("AbsoluteDim").include()
    CEGUI_ns.class_("ImageDim").include()
    CEGUI_ns.class_("ImagePropertyDim").include()
    CEGUI_ns.class_("WidgetDim").include()
    CEGUI_ns.class_("UnifiedDim").include()
    CEGUI_ns.class_("FontDim").include()
    CEGUI_ns.class_("PropertyDim").include()
    CEGUI_ns.class_("Dimension").include()
    CEGUI_ns.class_("ComponentArea").include()

    # falagard/FalEnums.h
    CEGUI_ns.enum("DimensionType").include()
    CEGUI_ns.enum("VerticalFormatting").include()
    CEGUI_ns.enum("HorizontalFormatting").include()
    CEGUI_ns.enum("VerticalTextFormatting").include()
    CEGUI_ns.enum("HorizontalTextFormatting").include()
    CEGUI_ns.enum("FontMetricType").include()
    CEGUI_ns.enum("DimensionOperator").include()
    CEGUI_ns.enum("FrameImageComponent").include()
    CEGUI_ns.enum("ChildEventAction").include()

    # falagard/EventAction.h
    eventAction = CEGUI_ns.class_("EventAction")
    eventAction.include()

    # falagard/FalEventLinkDefinition.h
    eventLinkDefinition = CEGUI_ns.class_("EventLinkDefinition")
    eventLinkDefinition.include()

    # falagard/FalFrameComponent.h
    frameComponent = CEGUI_ns.class_("FrameComponent")
    frameComponent.include()

    # falagard/FalImageryComponent.h
    imageryComponent = CEGUI_ns.class_("ImageryComponent")
    imageryComponent.include()

    # falagard/FalImagerySection.h
    imagerySection = CEGUI_ns.class_("ImagerySection")
    imagerySection.include()

    # falagard/FalLayerSpecification.h
    layerSpecification = CEGUI_ns.class_("LayerSpecification")
    layerSpecification.include()

    # falagard/FalNamedArea.h
    namedArea = CEGUI_ns.class_("NamedArea")
    namedArea.include()

    # TypedProperty.h
    # falagard/FalPropertyDefinition.h
    # falagard/FalPropertyLinkDefinition.h
    for cls in CEGUI_ns.classes():
        if not declarations.templates.is_instantiation(cls.name):
            continue

        tplName = declarations.templates.name(cls.name)

        desired_classes = ["TypedProperty", "FalagardPropertyBase", "PropertyDefinition", "PropertyLinkDefinition"]

        for desired_class in desired_classes:
            if tplName.startswith(desired_class):
                cls.include()

                if tplName.startswith("TypedProperty"):
                    # gccxml has issues with the T() default arg in the ctor
                    # since users are unlikely to construct TypedProperty from
                    # Python we can just exclude the ctors
                    cls.constructors().exclude()

    # falagard/FalPropertyDefinitionBase.h
    propertyDefinitionBase = CEGUI_ns.class_("PropertyDefinitionBase")
    propertyDefinitionBase.include()

    # falagard/FalPropertyInitialiser.h
    propertyInitialiser = CEGUI_ns.class_("PropertyInitialiser")
    propertyInitialiser.include()

    # falagard/FalSectionSpecification.h
    sectionSpecification = CEGUI_ns.class_("SectionSpecification")
    sectionSpecification.include()

    # falagard/FalStateImagery.h
    stateImagery = CEGUI_ns.class_("StateImagery")
    stateImagery.include()

    # falagard/FalTextComponent.h
    textComponent = CEGUI_ns.class_("TextComponent")
    textComponent.include()

    # falagard/FalWidgetComponent.h
    widgetComponent = CEGUI_ns.class_("WidgetComponent")
    widgetComponent.include()

    # falagard/FalWidgetLookFeel.h
    widgetLookFeel = CEGUI_ns.class_("WidgetLookFeel")
    widgetLookFeel.include()

    # falagard/FalWidgetLookManager.h
    widgetLookManager = CEGUI_ns.class_("WidgetLookManager")
    widgetLookManager.include()

    # falagard/FalXMLEnumHelper.h
    falagardXMLHelper = CEGUI_ns.class_("FalagardXMLHelper_wrapper")
    falagardXMLHelper.include()
    falagardXMLHelper.rename("FalagardXMLHelper")

    # todo: hack fixes
    # taken from python ogre, causes AttributeError at import if not excluded
    for cls in CEGUI_ns.classes():
        try:
            cls.variable("EventNamespace").exclude()
        except:
            pass
    ## turns out that in SOME classes this also fails registration (Combodroplist for example)
    for cls in CEGUI_ns.classes():
        try:
            cls.variable("WidgetTypeName").exclude()
        except:
            pass

    # taken from python ogre
    ## lets work around a bug in GCCXMl - http://language-binding.net/pygccxml/design.html#patchers
    draws = mb.mem_funs("draw")   # find all the draw functions
    for draw in draws:
        for arg in draw.arguments:
            if arg.default_value == "0ffffffff":
                arg.default_value = "0xffffffff"

    for cls in CEGUI_ns.classes():
        if cls.name.startswith("Singleton"):
            cls.include()

        if cls.name.startswith("NamedXMLResourceManager"):
            cls.include()
            # WORKAROUND: would not compile on Windows, pyplusplus substitutes the internal _Rb_tree_iterator
            cls.mem_fun("destroyObject").exclude()

    # no need for this function, just use getSingleton
    mb.mem_funs("getSingletonPtr").exclude()

    # include workaround functions from the CEGUI namespace
    CEGUI_ns.class_("Workarounds").include();

    # at no point should you need any private methods
    common_utils.excludeAllPrivate(CEGUI_ns)

def configureExceptions(mb):
    exception = mb.namespace("CEGUI").class_("Exception")
    exception.include()
    exception.mem_funs().exclude()
    exception.mem_fun("setStdErrEnabled").include()
    exception.mem_fun("isStdErrEnabled").include()
    exception.translate_exception_to_string("PyExc_RuntimeError", "exc.what()")

def generate():
    ### disable unnecessary warnings
    # can't pass function pointer
    messages.disable(messages.W1004)
    # operator not supported
    messages.disable(messages.W1014)
    # py++ will create a wrapper
    messages.disable(messages.W1023, messages.W1025, messages.W1026, messages.W1027, messages.W1031)
    # static pointer member can't be exported
    messages.disable(messages.W1035)
    # immutable pointer can't be exposed
    messages.disable(messages.W1036)
    # pointer to function can't be exposed
    messages.disable(messages.W1036, messages.W1037)
    # can't be overridden in python
    messages.disable(messages.W1049)

    mb = common_utils.createModuleBuilder("python_CEGUI.h", ["CEGUIBASE_EXPORTS"])
    CEGUI_ns = mb.global_ns.namespace("CEGUI")
    # first thing to do - converters!
    # !!! They have to be registered first, otherwise it will all fail horribly !!!
    common_utils.addStringConverters(mb)
    common_utils.addSupportForString(mb)

    filterDeclarations(mb)
    configureExceptions(mb)

    common_utils.setDefaultCallPolicies(CEGUI_ns)

    ## add additional version information to the module to help identify it correctly
    # todo: this should be done automatically
    common_utils.addVersionInfo(mb, PACKAGE_NAME, PACKAGE_VERSION)

    # Creating code creator. After this step you should not modify/customize declarations.
    mb.build_code_creator(module_name = MODULE_NAME, doc_extractor = common_utils.createDocumentationExtractor())

    common_utils.writeModule(mb, OUTPUT_DIR)

if __name__ == "__main__":
    common_utils.verbose_generate("CEGUIBase", generate)
