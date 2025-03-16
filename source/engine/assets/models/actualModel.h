#include "buffer.h"

struct GraphicsSetup;

struct Mesh {
    struct Vertex *vertices;
    uint16_t *indices;
    size_t verticesQuantity;
    size_t indicesQuantity;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

struct actualModel {
    struct buffer localMesh;

    uint32_t meshQuantity;
    struct Mesh *mesh;
};

void loadModels(size_t quantity, struct actualModel model[quantity], const char *modelPath[quantity], struct GraphicsSetup *vulkan);
void destroyActualModels(VkDevice device, uint32_t modelQuantity, struct actualModel *model);
