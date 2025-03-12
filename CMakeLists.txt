#################################################################################
# Basic project setup
#################################################################################
cmake_minimum_required(VERSION 3.29)
project(KubVC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CPACK_PACKAGE_NAME "KubVC")
set(CPACK_PACKAGE_VERSION "0.0.0")

set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_MODULE_PATH}")
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_PREFIX_PATH}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
message("Passed basic project setup")

#################################################################################
# Include third party 
#################################################################################
set(LIBS_DIR_PATH "${PROJECT_SOURCE_DIR}/libs")

set(LIBS_GLFW_DIR "${LIBS_DIR_PATH}/glfw")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${LIBS_GLFW_DIR})

find_package(OpenGL REQUIRED)
set(LIBS_GLEW_DIR "${PROJECT_SOURCE_DIR}/libs/glew")
add_subdirectory("${LIBS_GLEW_DIR}/build/cmake")
target_include_directories(glew PUBLIC ${LIBS_GLEW_DIR}/include)
message("Add OpenGL and GLEW ${LIBS_GLEW_DIR}")

set(LIBS_GLM_DIR "${LIBS_DIR_PATH}/glm/")
message("Add GLM ${LIBS_GLM_DIR}")
add_subdirectory(${LIBS_GLM_DIR}/glm)

message("Included third party!")
#################################################################################
# Add sources 
#################################################################################
set(KUBVC_SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)

message("Added our sources!")
include(CPack)