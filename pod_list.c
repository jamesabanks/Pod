#include <stdlib.h>
#include "pod_list.h"



    // Initialize POD_LIST_TYPE to 0x62, which is POD_OBJECT_TYPE + 2
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
            object->next->previous = (pod_object *) list;
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
    pod_object *object;
    size_t p;

    
    object = list->first;
    p = 0;
    if (object != NULL) {
    /* If the list is not empty */
        while (p != pos && object != (pod_object *) list) {
        /* Walk the list until we reach the pos-th position or the end */
            ++p;
            object = object->next;
        }
        if (p != pos) {
        /* We didn't reach the pos-th position so we reached the end */
            object = NULL;
        }
    }

    return object;
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
    pod_object *n;
    size_t p;

    // object should not be NULL
    if (object == NULL) return NULL;

    // object should not be a member of a list
    if (object->next != NULL || object->previous != NULL) return NULL;
    
    if (pos == 0) {
    /* If inserting at the very beginning (pos=0)... */
        if (list->first == NULL) {
        /* The list is empty */
            object->previous = object->next = (pod_object *) list;
            list->last = list->first = object;
        } else {
        /* Just insert at the front */
            object->previous = (pod_object *) list;
            object->next = list->first;
            list->first = object;
        }
    } else {
    /* Look for the place at pos, after which the object is to be inserted */
        p = 1;
        n = list->first;
        while (p < pos && n->next != (pod_object *) list) {
        /* Walk the list until we reach the pos-th position or the end */
            p++;
            n = n->next;
        }
        if (p == pos) {
        /* We reached the pos-th position */
            object->previous = n;
            object->next = n->next;
            n->next = object;
            if (list->last == n) {
                list->last = object;
            }
        } else {
        /* We reached the end instead */
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
    pod_object *n;
    size_t p;
 
    n = list->first;
    p = 0;
    if (n != NULL) {   
    /* If there is at least one object */
        while (p != pos && n != (pod_object *) list) {
        /* Walk the list until we've reached the pos-th position */
            ++p;
            n = n->next;
        }
        if (n == (pod_object *) list) {
        /* If we didn't reach the pos-th position, we reached the end.*/
            n = NULL;
        } else {
        /* N now points to the pos-th pod_object */
            if (n->previous == (pod_object *) list) {
            /* If this is the first object in the list... */
                if (n->next == (pod_object *) list) {
                    list->first = NULL;
                } else {
                    list->first = n->next;
                }
            } else {
            /* Previous object exists, point its next ptr to n's next ptr */
                n->previous->next = n->next;
            }
            if (n->next == (pod_object *) list) {
            /* If this is the last object in the list */
                if (n->previous == (pod_object *) list) {
                    list->last = NULL;
                } else {
                    list->last = n->previous;
                }
            } else {
            /* Next object exists, point its previous ptr to n's previous ptr */
                n->next->previous = n->previous;
            }
            /* N extracted, now clear its own pointers */
            n->previous = NULL;
            n->next = NULL;
        }
    }

    return n;
}



    // pod_list_replace
    //
    // Find the object at position pos and replace it with the provided object.
    //
    // Returns:
    //      NULL            Position pos doesn't exist.
    //      pod_object *    The object that was replaced.

pod_object *pod_list_replace(pod_list *list, size_t pos, pod_object *object)
{
    pod_object *n;
    size_t p;

    n = list->first;
    p = 0;
    if (n != NULL) {   
    /* If there is at least one object */
        while (p != pos && n != (pod_object *) list) {
        /* Walk the list until we've reached the pos-th position */
            ++p;
            n = n->next;
        }
        if (n == (pod_object *) list) {
        /* If we didn't reach the pos-th position, we reached the end.*/
            n = NULL;
        } else {
        /* N now points to the pos-th pod_object */
            object->previous = n->previous;
            if (n->previous == (pod_object *) list) {
            /* If this is the first object in the list */
                list->first = object;
            }
            n->previous = NULL;
            object->next = n->next;
            if (n->next == (pod_object *) list) {
            /* If this is the last object in the list */
                list->last = object;
            }
            n->next = NULL;
        }
    }

    return n;
}



    // pod_list_size
    //
    // Determine the number of objects in the list.
    //
    // Returns:
    //      size_t  The number of objects counted in the list.

size_t pod_list_size(pod_list *list)
{
    pod_object *object;
    size_t size; 

    object = list->first;
    size = 0;
    while (object->next != (pod_object *) list) {
        ++size;
        object = object->next;
    }

    return size;
}



    // pod_list_apply_all
    //
    // Call the provided function on the first object in the list.  Then call
    // the function on whatever the first call returned.  And so on, until the
    // called function returns the list.  Often what the apply function returns
    // will be the next member, but this isn't guaranteed.  In fact it can't
    // be, so we must the apply function to return the meaningful successor.
    
    // TODO Some helper functions may be helpful for writing apply functions,
    // such as insert, remove, and replace.

void pod_list_apply_all(pod_list *list, pod_list_apply *apply)
{
    pod_object *object;

    object = list->first;
    while (object != (pod_object *) list) {
        object = apply(list, object);
    }
}
