#include <stdint.h>

struct resource {
    char *name;
    void *this;

    void (*cleanup)(void *);
};

struct ResourceManager {
    size_t max;
    size_t qResources;

    struct resource *resources;
};

void addResource(struct ResourceManager *this, const char *name, void *mem, void (*cleanup)(void *));
void cleanupResource(struct ResourceManager *this, const char *name);
void cleanupResourcesOrg(struct ResourceManager *this);
void cleanupResources(void *this);

void *findResource(struct ResourceManager *this, const char *resource);
