#include <stdbool.h>
#include "pod_list.h"



    // Initialize POD_STRING to 0xb0002, which is POD_OBJECT_TYPE + 2
    // This means that pod_list was the second class written after pod_object.

const int POD_LIST_TYPE = 0x0b0002;



    // pod_list_create
    //
    // Allocate memory for a new pod_list and initialize the members.
    //
    // Returns:
    //    The address of the new pod_list
    //    NULL if memory can't be allocated.

pod_list *pod_list_create(bool is_map)
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
        list->is_map = is_map;
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

pod_object *pod_list_append(pod_list *list, pod_object *object)
{
    pod_object *prev_last;

    // list should not be NULL
    // object should not be NULL
    // object should not already be a member of a list
    if (list == NULL) return NULL;
    if (object == NULL) return NULL;
    if (object->next != NULL || object->previous != NULL) return NULL;

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

    return object;
}



    // pod_list_pop
    //
    // Get the first item from the list, unlink it, and return it.

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



    // pod_list_pop_last
    //
    // Get the last item from the list, unlink it, and return it.

pod_object *pod_list_pop_last(pod_list *list)
{
    pod_object *object;

    object = list->last;
    if (object != NULL) {
        if (list->first == list->last) {
            list->first = list->last = NULL;
        } else {
            list->last = object->previous;
        }
        object->next = object->previous = NULL;
    }

    return object;
}



    // pod_list_push
    //
    // Insert an object into the front of a list.

pod_object *pod_list_push(pod_list *list, pod_object *object)
{
    pod_object *first_next;

    // object should not be NULL
    // object should not already be a member of a list

    if (object == NULL) return NULL;
    if (object->next != NULL || object->previous != NULL) return NULL;

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

    return object;
}



    // pod_list_get_key
    //
    // Look for a matching string in the list, and return it.

pod_string *pod_list_get_key(pod_list *list, pod_string *key, size_t *pos)
{
    int different;
    size_t index;
    pod_object *object;
    pod_string *string;

    index = 0;
    string = NULL;
    object = list->first;
    if (object != NULL) {
        while (object != list) {
            if (object->type == POD_STRING_TYPE)
                string = (pod_string *) object;
                different = pod_string_compare(key, string);
                if (!different == 0) {
                    break;
                }
            }
            index++;
            object = object->next;
        }
        if (object == list) {
            index = 0;
            string = NULL;
        }
    }

    *pos = index;
    return string;
}



    // pod_list_get
    //
    // Get the matching string (if any) and return the value associated w/ it.

pod_object *pod_list_get(pod_list *list, pod_string *key, bool *found)
{
    size_t pos;
    pod_object *object;
    pod_string *string;

    object = NULL;
    *found = false;
    string = pod_list_get_key(list, key, &pos);
    if (string != NULL) {
        object = string->value;
        *found = true;
    }

    return object;
}
