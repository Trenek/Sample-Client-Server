find_package(Vulkan REQUIRED)

add_subdirectory(glfw EXCLUDE_FROM_ALL)
add_subdirectory(cglm EXCLUDE_FROM_ALL)
add_subdirectory(freetype EXCLUDE_FROM_ALL)
add_subdirectory(miniaudio EXCLUDE_FROM_ALL)
add_subdirectory(openGJK EXCLUDE_FROM_ALL)

set(INCLUDE_DIRECTORIES
    "${Vulkan_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/stb"
    "${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader"
    "${CMAKE_CURRENT_SOURCE_DIR}/cgltf"
    "${CMAKE_CURRENT_SOURCE_DIR}/cglm/include/cglm"
    "${CMAKE_CURRENT_SOURCE_DIR}/miniaudio"
    "${CMAKE_CURRENT_SOURCE_DIR}/openGJK/include/openGJK"
)

set(LINK_LIBRARIES
    "${Vulkan_LIBRARIES}"
    glfw
    cglm
    freetype
    miniaudio
    obj_openGJK
)

if (WIN32)
    set(LINK_LIBRARIES
        ${LINK_LIBRARIES}
        -mcrtdll=ucrt
        msvcrt
    )
else ()
    set(LINK_LIBRARIES
        ${LINK_LIBRARIES}
        -lm
    )
endif()

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} PARENT_SCOPE)
set(LINK_LIBRARIES ${LINK_LIBRARIES} PARENT_SCOPE)
