#include <stdbool.h>
#include <stdlib.h>
#include "pod_list.h"



    // Initialize POD_STRING to 0x62, which is POD_OBJECT_TYPE + 2
    // This means that pod_list was the second class written after pod_object.

const int POD_LIST_TYPE = 0x62;



    // pod_list_create
    //
    // Allocate memory for a new pod_list and initialize the members.
    //
    // Returns:
    //    The address of the new pod_list
    //    NULL if memory can't be allocated.

pod_list *pod_list_create(void)
{
    size_t length;
    pod_list *list;

    length = sizeof(pod_list);
    list = (pod_list *) malloc(length);
    if (list != NULL) {
        list->o.type = POD_LIST_TYPE;
        list->o.next = NULL;
        list->o.previous = NULL;
        list->o.destroy = pod_list_destroy;
        list->first = NULL;
        list->last = NULL;
    }

    return list;
}



    // pod_list_destroy
    //
    // Destroy all remaining elements in the list.  Then free the memory used
    // by the list structure.

void pod_list_destroy(void *target)
{
    pod_list *list;
    pod_object *object;

    list = (pod_list *) target;
    object = pod_list_pop(list);
    while (object != NULL) {
        object->destroy(object);
        object = pod_list_pop(list);
    }
    free(list);
}



    // pod_list_append
    //
    // Place an object at the end of a list.  If the object is NULL, ignore it.
    // If the object's previous or next members are not NULL, ignore the object.

void pod_list_append(pod_list *list, pod_object *object)
{
    pod_object *prev_last;

    // object should not be NULL
    if (object == NULL) return;

    // object should not already be a member of a list
    if (object->next != NULL || object->previous != NULL) return;

    if (list->last == NULL) {
        object->previous = object->next = (pod_object *) list;
        list->last = list->first = object;
    } else {
        prev_last = list->last;
        prev_last->next = object;
        object->previous = prev_last;
        object->next = (pod_object *) list;
        list->last = object; 
    }
}



    // pod_list_peek
    //
    // Get the address of the first object in the list.
    //
    // Returns:
    //      NULL            The list was empty.
    //      pod_object *    The first item in the list.

pod_object *pod_list_peek(pod_list *list)
{
    return list->first;
}



    // pod_list_pop
    //
    // Remove the first item from the list and return it.
    //
    // Returns:
    //      NULL            The list was empty.
    //      pod_object *    The (previously) first item in the list.

pod_object *pod_list_pop(pod_list *list)
{
    pod_object *object;

    object = list->first;
    if (object != NULL) {
        if (list->first == list->last) {
            list->first = list->last = NULL;
        } else {
            list->first = object->next;
        }
        object->next = object->previous = NULL;
    }

    return object;
}



    // pod_list_push
    //
    // Insert an object into the front of a list.  If the object is NULL,
    // ignore it.  If the object's previous or next member is NULL, ignore the
    // object.

void pod_list_push(pod_list *list, pod_object *object)
{
    pod_object *first_next;

    // object should not be NULL
    if (object == NULL) return;

    // object should not already be a member of a list
    if (object->next != NULL || object->previous != NULL) return;

    if (list->first == NULL) {
        object->previous = object->next = (pod_object *) list;
        list->last = list->first = object;
    } else {
        first_next = list->first;
        first_next->previous = object;
        object->previous = (pod_object *) list;
        object->next = first_next;
        list->first = object;
    }
}



    // pod_list_find
    //
    // Find the object at the given position.
    //
    // Returns:
    //      NULL            The position doesn't exist.
    //      pod_object *    The object found at the position.

pod_object *pod_list_find(pod_list *list, size_t pos)
{
    pod_object *first_next;
    size_t p;

    
    first_next = list->first;
    p = 0;
    if (list->first != NULL) {
        while (p != pos && first_next != (pod_object *) list) {
            ++p;
            first_next = first_next->next;
        }
        if (p != pos) {
            first_next = NULL;
        }
    }

    return first_next;
}



    // pod_list_insert
    //
    // Insert a pod_object into the list at position pos.  Return the address
    // of the inserted object.  Inserting at position 0 is equivalent to (but
    // not identical) to pod_list_push.
    //
    // Returns:
    //      NULL            The object was NULL, the position doesn't exist, or
    //                        the object's previous or next member wasn't NULL.
    //      pod_object *    Object sucessfully inserted, and this is it.

pod_object *pod_list_insert(pod_list *list, size_t pos, pod_object *object)
{
    pod_object *first_next;
    size_t p;

    // object should not be NULL
    if (object == NULL) return NULL;

    // object should not be a member of a list
    if (object->next != NULL || object->previous != NULL) return NULL;
    
    if (pos == 0) {
        if (list->first == NULL) {
            object->previous = object->next = (pod_object *) list;
            list->last = list->first = object;
        } else {
            object->previous = (pod_object *) list;
            object->next = list->first;
            list->first = object;
        }
    } else {
        p = 1;
        first_next = list->first;
        while (p < pos && first_next->next != (pod_object *) list) {
            p++;
            first_next = first_next->next;
        }
        if (p == pos) {
            object->previous = first_next;
            object->next = first_next->next;
            first_next->next = object;
            if (list->last == first_next) {
                list->last = object;
            }
        } else {
            object = NULL;
        }
    }

    return object;
}



    // pod_list_remove
    // 
    // Remove the pod_object at position pos from the list.  Return the address
    // of the removed object.  Removing from position 0 is equivalent (but not
    // quite identical) to pod_list_pop.
    //
    // Returns:
    //      NULL            Position pos wasn't found.
    //      pod_object *    The removed object.

pod_object *pod_list_remove(pod_list *list, size_t pos)
{
    pod_object *first_next;
    size_t p;

    
    first_next = list->first;
    p = 0;
    if (list->first != NULL) {
        while (p != pos && first_next != (pod_object *) list) {
            ++p;
            first_next = first_next->next;
        }
        if (p != pos) {
            first_next = NULL;
        } else {
            if (list->first == first_next) {
            }
            if (list->last == first_next) {
            }
    }

    return first_next;
    pod_object *object;

    object = pod_list_find(list, pos);


