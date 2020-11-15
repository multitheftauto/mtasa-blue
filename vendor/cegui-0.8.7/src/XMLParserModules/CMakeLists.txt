if (CEGUI_BUILD_XMLPARSER_EXPAT)
    add_subdirectory(Expat)
endif()

if (CEGUI_BUILD_XMLPARSER_XERCES)
    add_subdirectory(Xerces)
endif()

if (CEGUI_BUILD_XMLPARSER_LIBXML2)
    add_subdirectory(Libxml2)
endif()

if (CEGUI_BUILD_XMLPARSER_TINYXML)
    add_subdirectory(TinyXML)
endif()

if (CEGUI_BUILD_XMLPARSER_RAPIDXML)
    add_subdirectory(RapidXML)
endif()

