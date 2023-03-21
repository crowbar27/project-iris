cmake_minimum_required(VERSION 3.16)

project(project-iris)

set(CMAKE_CXX_STANDARD 20)

option(USE_GL "Use OpenGL GUI" ON)


if(USE_GL)

# try adding glfw by its own cmake file
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(src/External/glfw)

endif()


add_executable(project-iris "")

target_sources(project-iris
    PRIVATE
        src/main.cpp
)


if(USE_GL)

SET (EXTERNAL_GLAD_FILES
        src/External/glad/include/glad/glad.h
        src/External/glad/include/KHR/khrplatform.h
        src/External/glad/src/glad.c
)

SET (EXTERNAL_IMGUI_FILES
        src/External/imgui/imconfig.h
        src/External/imgui/imgui.h
        src/External/imgui/imgui.cpp
        src/External/imgui/imgui_demo.cpp
        src/External/imgui/imgui_draw.cpp
        src/External/imgui/imgui_internal.h
        src/External/imgui/imgui_tables.cpp
        src/External/imgui/imgui_widgets.cpp
        src/External/imgui/backends/imgui_impl_glfw.h
        src/External/imgui/backends/imgui_impl_glfw.cpp
        src/External/imgui/backends/imgui_impl_opengl3.h
        src/External/imgui/backends/imgui_impl_opengl3.cpp
        src/External/imgui/imstb_rectpack.h
        src/External/imgui/imstb_textedit.h
        src/External/imgui/imstb_truetype.h
)

target_include_directories(
        project-iris PUBLIC
        "${PROJECT_SOURCE_DIR}/src/External/glad/include"
        "${PROJECT_SOURCE_DIR}/src/External/glfw/include"
        "${PROJECT_SOURCE_DIR}/src/External/imgui/"
)

target_link_libraries(project-iris PUBLIC glfw ${GLFW_LIBRARIES})

target_sources(project-iris
    PRIVATE
        ${EXTERNAL_GLAD_FILES}
        ${EXTERNAL_IMGUI_FILES}
)

endif()