#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <tinyobj_loader_c.h>

#include "actualModel.h"
#include "myMalloc.h"

#include "Vertex.h"

#define BFR(x) ((struct AnimVertex *)(x))

//#define GOOD_OLD
#ifdef GOOD_OLD
    //#define ORIGINAL
    #ifdef ORIGINAL
    float my_local_color[][3] = {
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f }
    };
    #else
    float my_local_color[][3] = {
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 1.0f }
    };
    #endif
#endif

#ifdef _WIN64
#include <windows.h>
static char* mmap_file(size_t* len, const char* filename) {
    char* fileMapViewChar = NULL;
    HANDLE file = CreateFileA(
        filename, GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL
    );

    if (file != INVALID_HANDLE_VALUE) { /* E.g. Model may not have materials. */
        HANDLE fileMapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
        assert(fileMapping != INVALID_HANDLE_VALUE);

        LPVOID fileMapView = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
        fileMapViewChar = (char*)fileMapView;
        assert(fileMapView != NULL);

        DWORD file_size = GetFileSize(file, NULL);
        *len = (size_t)file_size;
    }

    return fileMapViewChar;
}

#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static char* mmap_file(size_t* len, const char* filename) {
    struct stat sb;
    char* p;
    int fd;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        return NULL;
    }

    if (!S_ISREG(sb.st_mode)) {
        fprintf(stderr, "%s is not a file\n", filename);
        return NULL;
    }

    p = (char*)mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if (p == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    if (close(fd) == -1) {
        perror("close");
        return NULL;
    }

    (*len) = sb.st_size;

    return p;
}

#endif

static void get_file_data(void* ctx, const char* filename, [[maybe_unused]] const int is_mtl,
                          [[maybe_unused]]const char* obj_filename, char** data, size_t* len) {
    // NOTE: If you allocate the buffer with malloc(),
    // You can define your own memory management struct and pass it through `ctx`
    // to store the pointer and free memories at clean up stage(when you quit an
    // app)
    // This example uses mmap(), so no free() required.
    (void)ctx;

    if (!filename) {
        fprintf(stderr, "null filename\n");
        *data = NULL;
        *len = 0;
    }
    else {
        *data = mmap_file(len, filename);
    }
}

void objLoadModel(const char *objectPath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t *shapes;
    size_t num_shapes = 0;
    tinyobj_material_t *materials;
    size_t num_materials;

    tinyobj_parse_obj(
        &attrib,
        &shapes,
        &num_shapes,
        &materials,
        &num_materials,
        objectPath,
        get_file_data,
        NULL,
        TINYOBJ_FLAG_TRIANGULATE
    );

    float (*attribVertices)[3] = (float (*)[3])attrib.vertices;
    float (*attribTexcoords)[2] = (float (*)[2])attrib.texcoords;

    model->meshQuantity = 1;
    model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);
    model->mesh[0].sizeOfVertex = sizeof(struct AnimVertex);
    model->mesh[0].vertices = malloc(sizeof(struct AnimVertex) * attrib.num_vertices);
    model->mesh[0].indices = malloc(sizeof(uint16_t) * attrib.num_faces);
    model->mesh[0].verticesQuantity = attrib.num_vertices;
    model->mesh[0].indicesQuantity = attrib.num_faces;

//#define NOT_OPTIMAL
#ifdef NOT_OPTIMAL
    for (size_t i = 0; i < attrib.num_faces; i += 1) {
        model->mesh[0].vertices[i] = (struct Vertex){
            .pos = {
                attribVertices[attrib.faces[i].v_idx][0],
                attribVertices[attrib.faces[i].v_idx][1],
                attribVertices[attrib.faces[i].v_idx][2]
            },
            .texCoord = {
                       attribTexcoords[attrib.faces[i].vt_idx][0],
                1.0f - attribTexcoords[attrib.faces[i].vt_idx][1],
            },
            #ifdef GOOD_OLD
            .color = {
                my_local_color[i % 8][0],
                my_local_color[i % 8][1],
                my_local_color[i % 8][2]
            }
            #else
            .color = { 1.0f, 1.0f, 1.0f }
            #endif
        };

        model->mesh[0].indices[i] = i;
    }
#else
    for (size_t i = 0; i < attrib.num_vertices; i += 1) {
        BFR(model->mesh[0].vertices)[i] = (struct AnimVertex){
            .pos = {
                attribVertices[i][0],
                attribVertices[i][1],
                attribVertices[i][2]
            },
            .weights = { 0.0f, 0.0f, 0.0f, 0.0f },
            .bone = { 0, 0, 0, 0 },
            .texCoord = { 0.0f, 0.0f },
            #ifdef GOOD_OLD
            .color = {
                my_local_color[i % 8][0],
                my_local_color[i % 8][1],
                my_local_color[i % 8][2]
            }
            #else
            .color = { 1.0f, 1.0f, 1.0f }
            #endif
        };
    }
    
    for (size_t i = 0; i < attrib.num_faces; i += 1) {
        BFR(model->mesh[0].vertices)[attrib.faces[i].v_idx].texCoord[0] = 0.0f + attribTexcoords[attrib.faces[i].vt_idx][0];
        BFR(model->mesh[0].vertices)[attrib.faces[i].v_idx].texCoord[1] = 1.0f - attribTexcoords[attrib.faces[i].vt_idx][1];

        model->mesh[0].indices[i] = attrib.faces[i].v_idx;
    }
#endif
    createBuffers(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, model->meshQuantity * sizeof(mat4), model->localMesh.buffers, model->localMesh.buffersMemory, model->localMesh.buffersMapped, device, physicalDevice, surface);

    for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
        glm_mat4_identity(((mat4 **)model->localMesh.buffersMapped)[k][0]);
    }

    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);
}
