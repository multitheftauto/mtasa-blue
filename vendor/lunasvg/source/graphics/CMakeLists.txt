target_sources(lunasvg
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/canvas.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/canvasimpl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/strokedata.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/paint.cpp"
)

target_include_directories(lunasvg
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}"
)
