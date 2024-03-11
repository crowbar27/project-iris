cmake_minimum_required(VERSION 3.16)


# vcpkg
include(FetchContent)
mark_as_advanced(FORCE
  FETCHCONTENT_BASE_DIR
  FETCHCONTENT_FULLY_DISCONNECTED
  FETCHCONTENT_QUIET
  FETCHCONTENT_UPDATES_DISCONNECTED)

# Require git for download
find_package(Git REQUIRED)

FetchContent_Declare(vcpkg-download
  GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
  GIT_TAG 215a2535590f1f63788ac9bd2ed58ad15e6afdff
  GIT_SHALLOW FALSE
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/vcpkg)
FetchContent_GetProperties(vcpkg-download)
if (NOT vcpkg-download_POPULATED)
  message(STATUS "Fetch vcpkg ...")
  FetchContent_Populate(vcpkg-download)
  mark_as_advanced(FORCE
    FETCHCONTENT_SOURCE_DIR_VCPKG-DOWNLOAD
    FETCHCONTENT_UPDATES_DISCONNECTED_VCPKG-DOWNLOAD)
endif ()

set(VCPKG_BOOTSTRAP_OPTIONS "-disableMetrics")
set(VCPKG_INSTALL_OPTIONS "--clean-after-build" "--no-print-usage")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_BINARY_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(ENV{VCPKG_FORCE_DOWNLOADED_BINARIES} ON)


project(project-iris)

set(CMAKE_CXX_STANDARD 20)

option(USE_GL "Use OpenGL GUI" ON)


if(USE_GL)

# try adding glfw by its own cmake file
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(src/External/glfw)

add_subdirectory(src/External/portable-file-dialogs)

endif()

find_package(HighFive REQUIRED)
find_package(cppzmq)

add_executable(project-iris "")

target_sources(project-iris
    PRIVATE
        src/config.h
        src/main.cpp
        src/message_types.h
)

target_link_libraries(project-iris PUBLIC HighFive cppzmq)

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
        "${PROJECT_SOURCE_DIR}/src/External/portable-file-dialogs/"
)

target_link_libraries(project-iris PUBLIC glfw ${GLFW_LIBRARIES})

target_sources(project-iris
    PRIVATE
        ${EXTERNAL_GLAD_FILES}
        ${EXTERNAL_IMGUI_FILES}
)

endif()