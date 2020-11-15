if (CEGUI_BUILD_LUA_GENERATOR)
    add_subdirectory(Lua/support/tolua++bin)
endif()

if (CEGUI_BUILD_LUA_MODULE)
    add_subdirectory(Lua)
endif()

if (CEGUI_BUILD_PYTHON_MODULES)
    add_subdirectory(Python/bindings)
endif()

