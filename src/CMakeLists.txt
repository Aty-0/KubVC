file(GLOB_RECURSE SOURCES *.cpp)

set(IS_DEBUG FALSE)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
endif()

if (NOT IS_DEBUG)
    add_executable(KubVcApp WIN32 ${SOURCES})
    if (MSVC) 
        target_link_options(KubVcApp PRIVATE /ENTRY:mainCRTStartup)
    elseif(MINGW)
        target_link_options(KubVcApp PRIVATE -mwindows)  
    endif()
else()
    add_executable(KubVcApp ${SOURCES})
endif()

target_include_directories(KubVcApp
    PUBLIC ${KUBVC_SOURCES_DIR}
    PUBLIC ${glew_SOURCE_DIR}/include
    PUBLIC ${imgui_SOURCE_DIR}
    PUBLIC ${imgui_SOURCE_DIR}/backends
    PUBLIC ${implot_SOURCE_DIR}
    PUBLIC ${ImGuiFileDialog_SOURCE_DIR}
)

target_compile_features(KubVcApp PRIVATE cxx_std_20)
target_link_libraries(KubVcApp
    PRIVATE
        glm::glm
        glfw
        libglew_static
        imgui
        implot
        ImGuiFileDialog
        IconFontHeaders
)

if (IS_DEBUG) 
    target_compile_definitions(KubVcApp PRIVATE 
        GLEW_STATIC
        USE_STD_FILESYSTEM
        DEBUG
        #KUB_LEXER_DEBUG_ENABLE_TOKEN_PRINT
        KUB_ENABLE_LEXER_DEBUG_LOG
        #KUB_USE_ANSI_COLORS
        )
else()
    target_compile_definitions(KubVcApp PRIVATE 
        GLEW_STATIC
        USE_STD_FILESYSTEM
    )
endif()

if (MSVC)
    target_compile_options(KubVcApp PRIVATE /W4)
else()
    target_compile_options(KubVcApp PRIVATE -Wall -Wextra -Wpedantic)
endif()


# Add fonts to builded application
file(GLOB_RECURSE RES_FONTS resources/*.ttf)
message("Found fonts: ${RES_FONTS}")

set(BUILD_FONTS_DIR "$<TARGET_FILE_DIR:KubVcApp>/fonts")
set(SOURCES_RESOURCES_FONTS_DIR "${PROJECT_SOURCE_DIR}/src/resources/fonts")

add_custom_target(CopyFonts ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BUILD_FONTS_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SOURCES_RESOURCES_FONTS_DIR}" "${BUILD_FONTS_DIR}"
    COMMENT "Copying TTF files... to ${BUILD_FONTS_DIR}"
)

add_dependencies(KubVcApp CopyFonts)