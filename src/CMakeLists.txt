file(GLOB_RECURSE SOURCES *.cpp)

set(IS_DEBUG FALSE)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
endif()



if (WIN32)
    set(RESOURCES_RC_PATH "${PROJECT_SOURCE_DIR}/src/resources/app.rc")
    
    # setup executable for win32
    add_executable(KubVcApp WIN32 
        ${SOURCES}
        ${RESOURCES_RC_PATH}
    )

    # disable console 
    if (NOT IS_DEBUG)
        if (MSVC) 
            target_link_options(KubVcApp PRIVATE /ENTRY:mainCRTStartup)
        elseif(MINGW)
            target_link_options(KubVcApp PRIVATE -mwindows)  
        endif()
    endif()
else()
    # Window icon is embedded in the code (application/window.cpp),
    # desktop/menu icon is installed via install() rules at the end of this file
    add_executable(KubVcApp ${SOURCES})

    # Generate embedded icon data (resources/app_icon_data.h) from app.ico if it's missing.
    # To regenerate after changing app.ico, just delete the header.
    set(ICON_DATA_HEADER "${PROJECT_SOURCE_DIR}/src/resources/app_icon_data.h")
    if (NOT EXISTS "${ICON_DATA_HEADER}")
        find_package(Python3 REQUIRED COMPONENTS Interpreter)
        message(STATUS "Generating ${ICON_DATA_HEADER}")
        execute_process(
            COMMAND "${Python3_EXECUTABLE}"
                    "${PROJECT_SOURCE_DIR}/src/resources/gen_icon_data.py"
                    "${PROJECT_SOURCE_DIR}/src/resources/app.ico"
                    "${ICON_DATA_HEADER}"
            RESULT_VARIABLE ICON_GEN_RESULT
            ERROR_VARIABLE ICON_GEN_ERROR
        )
        if (NOT ICON_GEN_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to generate app_icon_data.h: ${ICON_GEN_ERROR}")
        endif()
    endif()
endif()

target_include_directories(KubVcApp
    PUBLIC ${KUBVC_SOURCES_DIR}
    PUBLIC ${glew_SOURCE_DIR}/include
    PUBLIC ${imgui_SOURCE_DIR}
    PUBLIC ${imgui_SOURCE_DIR}/backends
    PUBLIC ${implot_SOURCE_DIR}
    PUBLIC ${ImGuiFileDialog_SOURCE_DIR}
    PUBLIC ${magic_enum_SOURCE_DIR}/include
    PUBLIC ${korobok_SOURCE_DIR}/include
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
        magic_enum::magic_enum
        #korobok
)

if (IS_DEBUG) 
    target_compile_definitions(KubVcApp PRIVATE 
        GLEW_STATIC
        USE_STD_FILESYSTEM
        DEBUG
        #KUB_LEXER_DEBUG_ENABLE_TOKEN_PRINT
        #KUB_ENABLE_LEXER_DEBUG_LOG
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


if (UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    set(LINUX_RES_DIR "${PROJECT_SOURCE_DIR}/src/resources/linux")

    install(FILES "${LINUX_RES_DIR}/kubvc.desktop"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
    install(DIRECTORY "${LINUX_RES_DIR}/icons/hicolor"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons")
endif()
