#ifndef INCLUDE_POD_MAP_H
#define INCLUDE_POD_MAP_H

#include "pod_object.h"
#include "pod_mapping.h"
#include "pod_string.h"



// A map is a container that contains only mappings.  There is no order, and
// the contents are referenced by key.


    // Type is POD_OBJECT_TYPE + 4 or 0x0bad0004

extern const int POD_MAP_TYPE;



struct pod_map;
typedef struct pod_map pod_map;

struct pod_map {
    pod_object o;
    pod_mapping *first;
    pod_mapping *last;
    pod_mapping *current;
};


    // Constructor and destructor

extern pod_map *pod_map_create(void);
extern void pod_map_destroy(void *target);


    // Other pod_map-related functions

extern void pod_map_add(pod_map *map, pod_string *key, pod_object *value);
extern void pod_map_add_mapping(pod_map *map, pod_mapping *mapping);
extern pod_mapping *pod_map_extract(pod_map *map, pod_string *key);
extern pod_object *pod_map_lookup(pod_string *key);
extern pod_mapping *pod_map_lookup_mapping(pod_string *key);

extern void pod_map_iterate(pod_map *map);
extern pod_mapping *pod_map_next(pod_map *map);

extern size_t pod_map_size(pod_map *map);



#endif /* INCLUDE_POD_MAP_H
