#include <stdbool.h>
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

    list = (pod_list *) malloc(sizeof(pod_list));
    if (list != NULL) {
        list->o.n.previous = &list->o.n;
        list->o.n.next = &list->o.n;
        list->o.type = POD_LIST_TYPE;
        list->o.destroy = pod_list_destroy;
        list->header.previous = &list->header;
        list->header.next = &list->header;
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
    // Place an object at the end of a list.  If the object is NULL, ignore it
    // and return.  If the object has a node.previous or node.next member,
    // ignore the object and return.

void pod_list_append(pod_list *list, pod_object *object)
{
    pod_node *node;

    if (object == NULL) return;
    // object should not be NULL

    if (object->n.next != NULL || object->n.previous != NULL) return;
    // object should not already be a member of a list

    node = &list->header;
    object->n.previous = node->previous;
    object->n.next = node;
    node->previous->next = &object->n;
    node->previous = &object->n;
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
    pod_node *node;
    pod_object *object;

    object = NULL;
    node = &list->header;
    if (node->next != node) {
        object = (pod_object *) node->next;
    }

    return object;
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
    pod_node *node;
    pod_object *object;

    object = NULL;
    node = list->header.next;
    if (node != &list->header) {
        object = (pod_object *) node;
        node = pod_node_remove(node);
    }

    return object;
}



    // pod_list_push
    //
    // Insert an object into the front of a list.  If the object is NULL,
    // ignore it and return.  If the object has a node.previous or node.next
    // member, ignore the object and return.

void pod_list_push(pod_list *list, pod_object *object)
{
    pod_node *node;

    if (object == NULL) return;
    // object should not be NULL

    if (object->n.next != NULL || object->n.previous != NULL) return;
    // object should not already be a member of a list

    node = &list->header;
    object->n.previous = node;
    object->n.next = node->next;
    node->next->previous = &object->n;
    node->next = &object->n;
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
    pod_node *node;
    pod_object *object;
    size_t p;

    node = &list->header;
    object = NULL;
    if (node != node->next) {
    /* If the list is not empty */
        node = node->next;
        p = 0;
        /* We'll label the first object number 0 */
        while ((p != pos) && (node != &list->header)) {
        /* Walk the list until we reach the pos-th position or the end */
            node = node->next;
            ++p;
        }
        if (p == pos) {
        /* We reached the pos-th position */
            object = (pod_object *) node;
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
    //                      the object's previous or next member wasn't NULL.
    //      pod_object *    Object sucessfully inserted, and this is it.

pod_object *pod_list_insert(pod_list *list, size_t pos, pod_object *object)
{
    bool found;
    pod_node *node;
    size_t p;

    if (object == NULL) return NULL;
    /* object should not be NULL */

    if (object->n.next != NULL || object->n.previous != NULL) return NULL;
    /* object should not be a member of a list */
    
    node = &list->header;
    p = 0;
    /* We'll label the gap between the header and the first node as 0, and we
       insert after the found node.  We can always insert at 0, as this is
       after the header.  If there are 0 objects, we can insert in one place,
       1 object means 2 places, and so on. */
    found = true;
    while (p != pos) {
    /* Walk the list looking for pos */
        ++p;
        node = node->next;
        if (node == &list->header) {
        /* didn't find pos as we got back to the header */
            found = false;
            break;
        }
    }
    if (found) {
    /* Insert after the found node */
        object->n.previous = node;
        object->n.next = node->next;
        node->next->previous = &object->n;
        node->next = &object->n;
    } else {
        object = NULL;
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
    //      NULL            Position pos doesn't exist.
    //      pod_object *    The removed object.

pod_object *pod_list_remove(pod_list *list, size_t pos)
{
    pod_object *at_pos;

    at_pos = pod_list_find(list, pos);
    if (at_pos != NULL) {
        pod_node_remove(&at_pos->n);
     /* at_pos->n.previous->next = at_pos->n.next;
        at_pos->n.next->previous = at_pos->n.previous;
        at_pos->n.previous = at_pos->n.next = NULL; */
    }

    return at_pos;
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
    pod_object *at_pos;

    at_pos = pod_list_find(list, pos);
    if (at_pos != NULL) {
        object->n.previous = at_pos->n.previous;
        object->n.next = at_pos->n.next;
        at_pos->n.previous->next = &object->n;
        at_pos->n.next->previous = &object->n;
        at_pos->n.previous = at_pos->n.next = NULL;
    }

    return at_pos;
}



    // pod_list_size
    //
    // Determine the number of objects in the list.
    //
    // Returns:
    //      size_t  The number of objects counted in the list.

size_t pod_list_size(pod_list *list)
{
    pod_node *node;
    size_t size; 

    node = list->header.next;
    size = 0;
    while (node != &list->header) {
        ++size;
        node = node->next;
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
    pod_node *node;
    pod_object *object;

    node = list->header.next;
    while (node != &list->header) {
        object = (pod_object *) node;
        object = apply(list, object);
        node = object->n.next;
    }
}
