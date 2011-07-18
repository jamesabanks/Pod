#ifndef INCLUDE_POD_MAPPING_H
#define INCLUDE_POD_MAPPING_H

#include "pod_object.h"
#include "pod_string.h"



// A mapping is a key-value pair.  The key must always be a string, but the
// value can be any type of pod_object.


    // Type is POD_OBJECT_TYPE + 3

extern const int POD_MAPPING_TYPE;



struct pod_mapping;
typedef struct pod_mapping pod_mapping;

struct pod_mapping {
    pod_object o;
    pod_string *key;
    pod_object *value;
};


    // Constructor and destructor

extern pod_mapping *pod_mapping_create(void);
extern pod_mapping *pod_mapping_create_with(pod_string *key, pod_object *value);
extern void pod_mapping_destroy(void *mapping);



#endif /* INCLUDE_POD_MAPPING_H */

