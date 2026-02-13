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

FetchContent_MakeAvailable(glfw)

# Disable some things from glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

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

#################################################################################
# Add sources 
#################################################################################
set(KUBVC_SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)

message("Added our sources!")
include(CPack)