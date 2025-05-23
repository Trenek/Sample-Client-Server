set(ICON)

if (WIN32)
    set(ICON 
        "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc"
    )
# else ()
endif()

set(ICON ${ICON} PARENT_SCOPE)
