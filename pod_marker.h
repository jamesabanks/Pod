#ifndef INCLUDE_POD_MARKER_H
#define INCLUDE POD_MARKER_H

#include "pod_object.h"



#define POD_MARKER_BEGIN    (-1)
#define POD_MARKER_MIDDLE   (-2)
#define POD_MARKER_END      (-3)



    // Type value is POD_OBJECT_TYPE + 5

extern const int POD_MARKER_TYPE;



typedef struct pod_marker {
    pod_object o;
    pod_object *object;
    pod_object *next_child;
    int mark;
} pod_marker;


    // Constructor and destructor

extern pod_marker *pod_marker pod_create_marker(void);
extern void pod_marker_destroy(void *target);



#endif /* INCLUDE_POD_MARKER */
