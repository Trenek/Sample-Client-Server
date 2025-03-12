set(SOURCE_DIRECTORIES
    "${CMAKE_CURRENT_SOURCE_DIR}/camera"
    "${CMAKE_CURRENT_SOURCE_DIR}/graphics"
        "${CMAKE_CURRENT_SOURCE_DIR}/graphics/descriptors"
        "${CMAKE_CURRENT_SOURCE_DIR}/graphics/buffers"
        "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup"
            "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/window"
            "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/GraphicsSetup"
                "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/GraphicsSetup/functions"
                    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/GraphicsSetup/functions/bufferOperations"
                    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/GraphicsSetup/functions/commonOperations"
                    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/GraphicsSetup/functions/imageOperations"
                    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/GraphicsSetup/functions/depthHelpers"
                "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/GraphicsSetup/queueFamilyIndices"
                "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/GraphicsSetup/swapChain"
                "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/GraphicsSetup/swapChainSupportDetails"
            "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/deltaTime"
            "${CMAKE_CURRENT_SOURCE_DIR}/graphics/setup/windowControl"
        "${CMAKE_CURRENT_SOURCE_DIR}/graphics/model"
            "${CMAKE_CURRENT_SOURCE_DIR}/graphics/model/builder"
                "${CMAKE_CURRENT_SOURCE_DIR}/graphics/model/builder/loader"
                "${CMAKE_CURRENT_SOURCE_DIR}/graphics/model/builder/type"
            "${CMAKE_CURRENT_SOURCE_DIR}/graphics/model/functions"
            "${CMAKE_CURRENT_SOURCE_DIR}/graphics/model/descriptors"
    "${CMAKE_CURRENT_SOURCE_DIR}/basicThings"
    "${CMAKE_CURRENT_SOURCE_DIR}/state"
)

list(TRANSFORM SOURCE_DIRECTORIES APPEND "/*.c" OUTPUT_VARIABLE SOURCE_FILES)
file(GLOB SOURCE_FILES ${SOURCE_FILES})

set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
set(SOURCE_DIRECTORIES ${SOURCE_DIRECTORIES} PARENT_SCOPE)
