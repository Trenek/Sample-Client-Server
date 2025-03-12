find_package(Vulkan REQUIRED)

add_subdirectory(glfw EXCLUDE_FROM_ALL)
add_subdirectory(cglm EXCLUDE_FROM_ALL)

set(INCLUDE_DIRECTORIES
    "${Vulkan_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/stb"
    "${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader"
    "${CMAKE_CURRENT_SOURCE_DIR}/cgltf"
    "${CMAKE_CURRENT_SOURCE_DIR}/cglm/include/cglm"
)

set(LINK_LIBRARIES
    "${Vulkan_LIBRARIES}"
    glfw
    cglm
)

if (WIN32)
    set(LINK_LIBRARIES
        ${LINK_LIBRARIES}
        -lucrt
    )
else ()
    set(LINK_LIBRARIES
        ${LINK_LIBRARIES}
        -lm
    )
endif()

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} PARENT_SCOPE)
set(LINK_LIBRARIES ${LINK_LIBRARIES} PARENT_SCOPE)
