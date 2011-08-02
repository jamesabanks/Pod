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



    // pod_map_define
    //
    // Map a key to a value.  If the key already exists, change the value.  If
    // the key doesn't exist, create a new mapping with the given key and
    // value and put it into the map.
    //
    // Returns:
    //      NULL            Key and/or value is NULL or didn't find a matching
    //                      key (there was no previous value).
    //      pod_object *    The previous value for the key.

pod_object *pod_map_define(pod_map *map, pod_string *key, pod_object *value)
{
    pod_mapping *mapping;
    pod_node *node;
    pod_object *old;

    if (key == NULL) return NULL;
    if (value == NULL) return NULL;

    old = NULL;
    mapping = pod_map_lookup_mapping(map, key);
    if (mapping != NULL) {
        old = mapping->value;
        mapping->value = value;
    } else {
        node = &map->header;
        mapping = pod_mapping_create_with(key, value);
        mapping->o.n.previous = node;
        mapping->o.n.next = node->next;
        node->next->previous = &mapping->o.n;
        node->next = &mapping->o.n;
    }

    return old;
}



    // pod_map_lookup_mapping
    //
    // Traverse the map, looking for a mapping which has a matching key.  A map
    // should only have one match.
    //
    // Returns:
    //      NULL            No match was found (or key was NULL).
    //      pod_mapping *   Pointer to a (the) mapping with a matching key.

pod_mapping *pod_map_lookup_mapping(pod_map *map, pod_string *key)
{
    pod_mapping *mapping;
    pod_node *node;
    int result;

    if (key == NULL) return NULL;
    /* key shouldn't be NULL */

    mapping = NULL;
    node = map->header.next;
    while (node != &map->header) {
        result = pod_string_compare(key, ((pod_mapping *) node)->key);
        if (result == 0) {
            mapping = (pod_mapping *) node;
            break;
        }
        node = node->next;
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

    /* if key is NULL, pod_map_lookup_mapping will return NULL */

    value = NULL;
    mapping = pod_map_lookup_mapping(map, key);
    if (mapping != NULL) {
        value = mapping->value;
    }

    return value;
}



    // pod_map_remove
    //
    // Remove the mapping with the provided key.
    //
    // Returns:
    //      NULL            Key is NULL or didn't find a matching key.
    //      pod_object *    The value of the deleted mapping.

pod_object *pod_map_remove(pod_map *map, pod_string *key)
{
    pod_mapping *mapping;
    pod_object *old;

    /* if key is NULL, pod_map_lookup_mapping will return NULL */

    old = NULL;
    mapping = pod_map_lookup_mapping(map, key);
    if (mapping != NULL) {
        pod_node_remove(&mapping->o.n);
        old = mapping->value;
        mapping->key = NULL;
        mapping->value = NULL;
        mapping->o.destroy(mapping);
    }

    return old;
}



    // pod_map_size
    //
    // Caculate the number of mappings in the map.
    //
    // Returns:
    //      size_t  The number of mappings in the map.

size_t pod_map_size(pod_map *map)
{
    pod_node *node;
    size_t size;

    node = map->header.next;
    size = 0;
    while (node != &map->header) {
        ++size;
        node = node->next;
    }

    return size;
}



    // pod_map_iterate
    //
    // Prepare the pod_map for iteration.  That means set the current member
    // to the first object in the map.  ('First object' only has meaning within
    // this file, technically speaking.)

void pod_map_iterate(pod_map *map)
{
    map->current = map->header.next;
}



    // pod_map_next
    //
    // A companion to pod_map_iterate.  Get the next object in the map.
    //
    // Returns:
    //      NULL            No more mappings exist.
    //      pod_mapping *   The next mapping

pod_mapping *pod_map_next(pod_map *map)
{
    pod_mapping *last;

    last = NULL;
    if (map->current != &map->header) {
        last = (pod_mapping *) map->current;
        map->current = map->current->next;
    }

    return last;
}
