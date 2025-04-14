set(SOURCE_DIRECTORIES
    "${CMAKE_CURRENT_SOURCE_DIR}/camera"
    "${CMAKE_CURRENT_SOURCE_DIR}/engine"
        "${CMAKE_CURRENT_SOURCE_DIR}/engine/assets"
            "${CMAKE_CURRENT_SOURCE_DIR}/engine/assets/models"
                "${CMAKE_CURRENT_SOURCE_DIR}/engine/assets/models/loader"
            "${CMAKE_CURRENT_SOURCE_DIR}/engine/assets/textures"
                "${CMAKE_CURRENT_SOURCE_DIR}/engine/assets/textures/functions"
        "${CMAKE_CURRENT_SOURCE_DIR}/engine/descriptors"
        "${CMAKE_CURRENT_SOURCE_DIR}/engine/bufferDefinitions"
        "${CMAKE_CURRENT_SOURCE_DIR}/engine/buffers"
        "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup"
            "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/window"
            "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/GraphicsSetup"
                "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/GraphicsSetup/functions"
                    "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/GraphicsSetup/functions/bufferOperations"
                    "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/GraphicsSetup/functions/commonOperations"
                    "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/GraphicsSetup/functions/imageOperations"
                    "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/GraphicsSetup/functions/depthHelpers"
                "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/GraphicsSetup/queueFamilyIndices"
                "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/GraphicsSetup/swapChain"
                "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/GraphicsSetup/swapChainSupportDetails"
            "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/deltaTime"
            "${CMAKE_CURRENT_SOURCE_DIR}/engine/setup/windowControl"
        "${CMAKE_CURRENT_SOURCE_DIR}/engine/pipeline"
        "${CMAKE_CURRENT_SOURCE_DIR}/engine/entities/string"
        "${CMAKE_CURRENT_SOURCE_DIR}/engine/entities/model"
            "${CMAKE_CURRENT_SOURCE_DIR}/engine/pipeline/functions"
    "${CMAKE_CURRENT_SOURCE_DIR}/basicThings"
    "${CMAKE_CURRENT_SOURCE_DIR}/state"
)

list(TRANSFORM SOURCE_DIRECTORIES APPEND "/*.c" OUTPUT_VARIABLE SOURCE_FILES)
file(GLOB SOURCE_FILES ${SOURCE_FILES})

set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
set(SOURCE_DIRECTORIES ${SOURCE_DIRECTORIES} PARENT_SCOPE)
