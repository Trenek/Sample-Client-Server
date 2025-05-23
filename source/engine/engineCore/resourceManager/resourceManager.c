#include <stdlib.h>
#include <string.h>

#include "resourceManager.h"

void addResource(struct ResourceManager *this, const char *name, void *mem, void (*cleanup)(void *)) {
    this->qResources += 1;

    if (this->resources) {
        if (this->max < this->qResources) {
            this->max = this->qResources;
            this->resources = realloc(this->resources, sizeof(struct resource) * this->max);
        }
    }
    else {
        this->max = this->qResources;
        this->resources = calloc(this->max, sizeof(struct resource));
    }

    this->resources[this->qResources - 1].this = mem;
    this->resources[this->qResources - 1].cleanup = cleanup;
    this->resources[this->qResources - 1].name = calloc(strlen(name) + 1, sizeof(char));
    strcpy(this->resources[this->qResources - 1].name, name);
}

void cleanupResource(struct ResourceManager *this, const char *name) {
    size_t i = 0;

    while (i < this->qResources && 0 != strcmp(this->resources[i].name, name)) i += 1;
    if (i < this->qResources) {
        if (this->resources[i].cleanup) {
            this->resources[i].cleanup(this->resources[i].this);
        }
        free(this->resources[i].name);
        while (i < this->qResources - 1) {
            this->resources[i] = this->resources[i + 1];

            i += 1;
        }

        this->qResources -= 1;
    }
}

void cleanupResourcesOrg(struct ResourceManager *this) {
    for (size_t i = 0; i < this->qResources; i += 1) {
        if (this->resources[i].cleanup) {
            this->resources[i].cleanup(this->resources[i].this);
        }
        free(this->resources[i].name);
    }

    free(this->resources);
}

void cleanupResources(void *this) {
    cleanupResourcesOrg(this);

    free(this);
}

void *findResource(struct ResourceManager *this, const char *resource) {
    void *result = NULL;
    
    for (size_t i = 0; result == NULL && i < this->qResources; i += 1) {
        if (0 == strcmp(this->resources[i].name, resource)) {
            result = this->resources[i].this;
        }
    }

    return result;
}
