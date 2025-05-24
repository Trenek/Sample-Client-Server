cmake_minimum_required(VERSION 3.10)

project(Arena-Fighter VERSION 1.0 LANGUAGES C)

add_subdirectory(dependencies)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(source)
add_subdirectory(icon)

add_executable(Arena-Fighter
    ${SOURCE_FILES} 
    ${ICON}
    "${CMAKE_SOURCE_DIR}/source/main.c"
)

target_link_libraries(Arena-Fighter ${LINK_LIBRARIES})
target_include_directories(Arena-Fighter PRIVATE 
    ${INCLUDE_DIRECTORIES}
    ${SOURCE_DIRECTORIES}
)
set(COMPILE_OPTIONS
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
    set(COMPILE_OPTIONS 
        ${COMPILE_OPTIONS}
        -O3
    )
    if (WIN32)
        target_link_options(Arena-Fighter PRIVATE -mwindows)
    endif()
else()
    set(COMPILE_OPTIONS 
        ${COMPILE_OPTIONS}
        -g
    )
endif()

target_compile_options(Arena-Fighter PUBLIC ${COMPILE_OPTIONS})

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
    COMMAND glslc ${SHADER_SRC}/playerAnimation.vert -o ${SHADER_DST}/playerAnimation.spv
    COMMAND glslc ${SHADER_SRC}/playerShader.frag -o ${SHADER_DST}/playerFrag.spv
    COMMAND glslc ${SHADER_SRC}/shaderText.frag -o ${SHADER_DST}/fragText.spv
    COMMAND glslc ${SHADER_SRC}/shader2d.frag -o ${SHADER_DST}/frag2d.spv
    COMMAND glslc ${SHADER_SRC}/shader2d.vert -o ${SHADER_DST}/vert2d.spv
    COMMAND glslc ${SHADER_SRC}/text.vert -o ${SHADER_DST}/textV.spv
    COMMAND glslc ${SHADER_SRC}/text2d.vert -o ${SHADER_DST}/text2dV.spv
    COMMAND glslc ${SHADER_SRC}/text.frag -o ${SHADER_DST}/textF.spv
    COMMAND glslc ${SHADER_SRC}/skybox.vert -o ${SHADER_DST}/skyboxV.spv
    COMMAND glslc ${SHADER_SRC}/skybox.frag -o ${SHADER_DST}/skyboxF.spv
    COMMAND glslc ${SHADER_SRC}/animation.vert -o ${SHADER_DST}/animation.spv
    COMMAND glslc ${SHADER_SRC}/debugShader.frag -o ${SHADER_DST}/debugFrag.spv
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/textures ${CMAKE_BINARY_DIR}/textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/models
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/fonts ${CMAKE_BINARY_DIR}/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/music ${CMAKE_BINARY_DIR}/music
)

add_dependencies(Arena-Fighter shaders)
