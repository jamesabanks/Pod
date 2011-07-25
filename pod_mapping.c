#include <stdlib.h>
#include "pod_mapping.h"



// Initialize POD_MAPPING_TYPE to 0x63

const int POD_MAPPING_TYPE = 0x63;



    // pod_mapping_create
    //
    // Allocate memory for a new pod_mapping and initialize the members.
    //
    // Returns:
    //      NULL            Memory can't be allocated
    //      pod_mapping *   The address of a new pod_mapping

pod_mapping *pod_mapping_create(void)
{
    pod_mapping *mapping;

    mapping = (pod_mapping *) malloc(sizeof(pod_mapping));
    if (mapping != NULL) {
        mapping->o.n.previous = NULL;
        mapping->o.n.next = NULL;
        mapping->o.type = POD_MAPPING_TYPE;
        mapping->o.destroy = pod_mapping_destroy;
        mapping->key = NULL;
        mapping->value = NULL;
    }

    return mapping;
}



    // pod_mapping_create_with
    //
    // Create a new mapping with the key and value members initialized to the
    // provided values.
    //
    // Returns:
    //      NULL            Memory can't be allocated
    //      pod_mapping *   The address of the new, initialized pod_mapping

pod_mapping *pod_mapping_create_with(pod_string *key, pod_object *value)
{
    pod_mapping *mapping;

    mapping = pod_mapping_create();
    if (mapping != NULL) {
        mapping->key = key;
        mapping->value = value;
    }

    return mapping;
}



    // pod_mapping_destroy
    //
    // Destroy the contents, if any, then free the memory.

void pod_mapping_destroy(void *target)
{
    pod_mapping *mapping;

    mapping = (pod_mapping *) target;
    if (mapping->key == NULL) {
        mapping->key->o.destroy(mapping->key);
        mapping->key = NULL;
    }
    if (mapping->value == NULL) {
        mapping->value->destroy(mapping->value);
        mapping->value = NULL;
    }
    free(mapping);
}

