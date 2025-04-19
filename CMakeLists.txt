cmake_minimum_required(VERSION 3.10)

project(Test VERSION 1.0 LANGUAGES C)

add_subdirectory(dependencies)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(source)

add_executable(Test
    ${SOURCE_FILES} 
    "${CMAKE_SOURCE_DIR}/source/main.c"
)

target_link_libraries(Test ${LINK_LIBRARIES})
target_include_directories(Test PRIVATE 
    ${INCLUDE_DIRECTORIES}
    ${SOURCE_DIRECTORIES}
)
target_compile_options(Test PUBLIC
    -g
    -W
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wunused
    -Wno-logical-op-parentheses
    -Wno-parentheses
    -Werror
    -Wno-comment
    -pedantic
    -pedantic-errors
    -D_GLIBCXX_DEBUG
    -D_GLIBCXX_DEBUG_PEDANTIC
    -D_UCRT
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_options(Test PRIVATE -mwindows)
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

add_custom_target(shaders DEPENDS
    ${CMAKE_SOURCE_DIR}/shaders
    ${CMAKE_SOURCE_DIR}/textures
    ${CMAKE_SOURCE_DIR}/models
)

set(SHADER_SRC "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_DST "${CMAKE_BINARY_DIR}/shaders")

add_custom_command(TARGET shaders POST_BUILD
    COMMAND glslc ${SHADER_SRC}/shader.vert -o ${SHADER_DST}/vert.spv
    COMMAND glslc ${SHADER_SRC}/shader.frag -o ${SHADER_DST}/frag.spv
    COMMAND glslc ${SHADER_SRC}/shader2d.frag -o ${SHADER_DST}/frag2d.spv
    COMMAND glslc ${SHADER_SRC}/shader2d.vert -o ${SHADER_DST}/vert2d.spv
    COMMAND glslc ${SHADER_SRC}/text.vert -o ${SHADER_DST}/textV.spv
    COMMAND glslc ${SHADER_SRC}/text2d.vert -o ${SHADER_DST}/text2dV.spv
    COMMAND glslc ${SHADER_SRC}/text.frag -o ${SHADER_DST}/textF.spv
    COMMAND glslc ${SHADER_SRC}/skybox.vert -o ${SHADER_DST}/skyboxV.spv
    COMMAND glslc ${SHADER_SRC}/skybox.frag -o ${SHADER_DST}/skyboxF.spv
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/textures ${CMAKE_BINARY_DIR}/textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/models
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/fonts ${CMAKE_BINARY_DIR}/fonts
)

add_dependencies(Test shaders)
