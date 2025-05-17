#include <cglm.h>

#include "buffer.h"

struct GraphicsSetup;

struct Mesh {
    void *vertices;
    size_t sizeOfVertex;
    uint16_t *indices;
    size_t verticesQuantity;
    size_t indicesQuantity;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

struct timeFrame {
    size_t qData;
    size_t qValues;
    int interpolationType;

    struct {
        float time;
        float *values;
    } *data;
};

struct jointData {
    struct timeFrame transformation[3];

    mat4 inverseMatrix;
    int16_t father;
    int16_t isJoint;
};

struct colisionBox {
    char *name;

    size_t qVertex;
    void **vertex;
};

struct actualModel {
    struct buffer localMesh;

    size_t qAnim;
    size_t qJoint;
    void *anim; // struct jointData

    uint32_t meshQuantity;
    struct Mesh *mesh;

    size_t qHitbox;
    size_t qHurtBox;

    struct colisionBox *hitBox;
    struct colisionBox *hurtBox;
};

void loadModels(size_t quantity, struct actualModel model[quantity], const char *modelPath[quantity], struct GraphicsSetup *vulkan);
void destroyActualModels(VkDevice device, uint32_t modelQuantity, struct actualModel *model);
