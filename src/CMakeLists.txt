file(GLOB_RECURSE SOURCES *.cpp)

add_executable(KubVcApp ${SOURCES})

target_include_directories(KubVcApp
    PUBLIC ${KUBVC_SOURCES_DIR}
    PUBLIC ${glew_SOURCE_DIR}/include
)

target_compile_features(KubVcApp PRIVATE cxx_std_20)
#add_compile_definitions(KubVcApp DEBUG)
target_link_libraries(KubVcApp
    glm::glm
    glfw
    libglew_static
)

target_compile_definitions(KubVcApp PRIVATE GLEW_STATIC)

# Add fonts to builded application
file(GLOB_RECURSE RES_FONTS resources/*.ttf)
message("Found fonts: ${RES_FONTS}")

set(BUILD_FONTS_DIR "$<TARGET_FILE_DIR:KubVcApp>/fonts")
set(SOURCES_RESOURCES_FONTS_DIR "${PROJECT_SOURCE_DIR}/src/resources/fonts")

add_custom_target(CopyFonts ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BUILD_FONTS_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SOURCES_RESOURCES_FONTS_DIR}" "${BUILD_FONTS_DIR}"
    COMMENT "Copying TTF files..."
)