if (CEGUI_BUILD_IMAGECODEC_CORONA)
    add_subdirectory(Corona)
endif()

if (CEGUI_BUILD_IMAGECODEC_DEVIL)
    add_subdirectory(DevIL)
endif()

if (CEGUI_BUILD_IMAGECODEC_FREEIMAGE)
    add_subdirectory(FreeImage)
endif()

if (CEGUI_BUILD_IMAGECODEC_SILLY)
    add_subdirectory(SILLY)
endif()

if (CEGUI_BUILD_IMAGECODEC_STB)
    add_subdirectory(STB)
endif()

if (CEGUI_BUILD_IMAGECODEC_TGA)
    add_subdirectory(TGA)
endif()

if (CEGUI_BUILD_IMAGECODEC_PVR)
    add_subdirectory(PVR)
endif()

if (CEGUI_BUILD_IMAGECODEC_SDL2)
    add_subdirectory(SDL2)
endif()