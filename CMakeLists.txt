cmake_minimum_required(VERSION 4.0)

project(Dissertation)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    "src/camera.cpp"
    "src/framebuffer.cpp"
    "src/input.cpp"
    "src/interface.cpp"
    "src/light.cpp"
    "src/lpv.cpp"
    "src/main.cpp"
    "src/model.cpp"
    "src/renderer.cpp"
    "src/shader.cpp"
    "src/stb_image.cpp"
    "glad/src/glad.c"
    "imgui/imgui.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/backends/imgui_impl_glfw.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
)

add_subdirectory("glm" EXCLUDE_FROM_ALL)
add_subdirectory("glfw" EXCLUDE_FROM_ALL)

set(ASSIMP_INSTALL OFF)
add_subdirectory("assimp" EXCLUDE_FROM_ALL)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/glad/include")
find_package(OpenGL REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glm
    assimp
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/data/"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
