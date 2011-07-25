#include <stddef.h>
#include <stdlib.h>
#include "pod_map.h"



// Initialize POD_MAP_TYPE to 0x64

const int POD_MAP_TYPE = 0x64;



    // pod_map_create
    //
    // Allocate memory for a new pod_map and initialize the members
    //
    // Returns:
    //      NULL        Couldn't allocate memory
    //      pod_map *   The address of the new, empty map

pod_map *pod_map_create(void)
{
    pod_map *map;

    map = (pod_map *) malloc(sizeof(pod_map));
    if (map != NULL) {
        map->o.n.previous = NULL;
        map->o.n.next = NULL;
        map->o.type = POD_MAP_TYPE;
        map->o.destroy = pod_map_destroy;
        map->header.previous = &map->header;
        map->header.next = &map->header;
        map->current = NULL;
    }

    return map;
}



    // pod_map_destroy
    //
    // If not empty, destroy each contained object. Then free the map.  Compare
    // to pod_list_pop.

void pod_map_destroy(void *target)
{
    pod_map *map;
    pod_node *node;
    pod_object *object;

    map = (pod_map *) target;
    node = map->header.next;
    while (node != &map->header) {
        object = (pod_object *) node;
        node = pod_node_remove(node);
        object->destroy(object);
    }
    free(map);
}



    // pod_map_lookup_mapping
    //
    // Traverse the map, looking for a mapping which has a matching key.  A map
    // should only have one match.
    //
    // Returns:
    //      NULL            No match was found
    //      pod_mapping *   Pointer to a (the) mapping with a matching key

pod_mapping *pod_map_lookup_mapping(pod_map *map, pod_string *key)
{
    pod_mapping *mapping;
    int result;

    mapping = map->first;
    if (mapping != NULL) {
        while ((pod_object *) mapping != (pod_object *) map) {
            result = pod_string_compare(key, mapping->key);
            if (result == 0) {
                break;
            }
        }
        if ((pod_object *) mapping == (pod_object *) map) {
            mapping = NULL;
        }
    }

    return mapping;
}



    // pod_map_lookup
    //
    // Find a mapping with a matching key and return its value.
    //
    // Returns:
    //      NULL            Didn't find a mapping with a matching key.
    //      pod_object *    The value from the mapping.

pod_object *pod_map_lookup(pod_map *map, pod_string *key)
{
    pod_mapping *mapping;
    pod_object *value;

    value = NULL;
    mapping = pod_map_lookup_mapping(map, key);
    if (mapping != NULL) {
        value = mapping->value;
    }

    return value;
}
