#################################################################################
# Basic project setup
#################################################################################
cmake_minimum_required(VERSION 3.20)
project(KubVC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CPACK_PACKAGE_NAME "KubVC")
set(CPACK_PACKAGE_VERSION "1.0.0")

set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_MODULE_PATH}")
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_PREFIX_PATH}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

#################################################################################
# Third party 
#################################################################################
include(FetchContent)

message("Fetch third party libs...")

# Get glfw 
FetchContent_Declare(
	glfw
	GIT_REPOSITORY	https://github.com/glfw/glfw
	GIT_TAG 	"3.4"
)

# Disable some things from glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# Get opengl package
find_package(OpenGL REQUIRED)

# Get glew 
FetchContent_Declare(
	glew
	GIT_REPOSITORY https://github.com/Perlmint/glew-cmake
	GIT_TAG 	"glew-cmake-2.3.1"
)

FetchContent_MakeAvailable(glew)

# Get glm
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	"1.0.3"
)

FetchContent_MakeAvailable(glm)

# Get ImGui 
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.7-docking
    GIT_SHALLOW TRUE
)
	
FetchContent_MakeAvailable(imgui)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}         
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC 
    OpenGL::GL 
    glfw
)

# Get implot 
FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG v0.17
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(implot)

set(IMPLOT_SOURCES
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
    ${implot_SOURCE_DIR}/implot_demo.cpp
)

add_library(implot STATIC ${IMPLOT_SOURCES})
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PUBLIC imgui)

#################################################################################
# Add sources 
#################################################################################
set(KUBVC_SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)

message("Added our sources!")
include(CPack)