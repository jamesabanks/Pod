#ifndef INCLUDE_POD_MARKER_H
#define INCLUDE POD_MARKER_H

#include "pod_object.h"



typedef enum {
    POD_MARKER_BEGIN = 1,
    POD_MARKER_MIDDLE,
    POD_MARKER_ESCAPE,
    POD_MARKER_CHAR,
    POD_MARKER_NULL,
    POD_MARKER_FINAL,
    POD_MARKER_END,
} pod_marker_state_t;



    // Type value is POD_OBJECT_TYPE + 5

extern const int POD_MARKER_TYPE;



typedef struct pod_marker {
    pod_object          o;
    pod_marker_state_t  state;
    pod_object          *object;
    pod_object          *next_child;
    int                 index;
    pod_char_t          escape;
} pod_marker;


    // Constructor and destructor

extern pod_marker *pod_marker pod_create_marker(void);
extern void pod_marker_destroy(void *target);



#endif /* INCLUDE_POD_MARKER */
