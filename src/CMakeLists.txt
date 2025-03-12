file(GLOB_RECURSE SOURCES *.cpp)

add_executable(KubVcApp ${SOURCES})

target_include_directories(KubVcApp
    PUBLIC ${LIBS_GLEW_DIR}/include
    PUBLIC ${LIBS_GLFW_DIR}/include
    PUBLIC ${LIBS_GLM_DIR}
    PUBLIC ${KUBVC_SOURCES_DIR}
)

target_compile_features(KubVcApp PRIVATE cxx_std_20)
   
target_link_libraries(KubVcApp
    glfw
    glew
)
