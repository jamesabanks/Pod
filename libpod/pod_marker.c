#include <stdlib.h>
#include "pod_marker.h"



    // pod_marker_create
    //
    // Allocate and initialize a new pod_marker.
    //
    // Returns:
    //      NULL            Memory can't be allocated
    //      pod_marker *    The address of a new pod_marker

pod_marker *pod_marker_create(void)
{
    pod_marker *marker;

    marker = (pod_marker *) malloc(sizeof(pod_marker));
    if (marker != NULL) {
        marker->o.n.previous = NULL;
        marker->o.n.next = NULL;
        marker->o.type = POD_MARKER_TYPE;
        marker->o.destroy = pod_marker_destroy;
        marker->state = POD_MARKER_BEGIN;
        marker->object = NULL;
        marker->next_child = NULL;
        marker->index = 0;
        marker->escape = 0;
    }

    return marker;
}



    // pod_marker_destroy

void pod_marker_destroy(void *target)
{
    pod_marker *marker;
    pod_object *object;

    marker = (pod_marker *) target;
    object = marker->object;
    if (object != NULL) {
        object->destroy(object);
    }
    object = marker->next_child;
    if (object != NULL) {
        object->destroy(object);
    }
    free(marker);
}
