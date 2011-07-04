#ifndef INCLUDE_POD_LIST_H
#define INCLUDE_POD_LIST_H

#include "pod_object.h"
#include "pod_string.h"



// I'm finding that struct blah-blah wastes too much space so I'll try it
// without (that is, use a typedef).

// TODO Other functions that might be useful: pod_list_replace_at,
// pod_list_duplicate, pod_list_delete_at, pod_list_find_and_return_pos,
// pod_list_find_next_and_return_pos

// TODO What about a remove flag for find?


    // Type value is POD_OBJECT_TYPE + 2

extern const int POD_LIST_TYPE;



struct pod_list;
typedef struct pod_list pod_list;

struct pod_list {
    pod_object o;
    pod_object *first;
    pod_object *last;
};


    // Constructor and destructor

extern pod_list *pod_list_create(void);
extern void pod_list_destroy(void *target);


    // Other pod_list-related functions

extern pod_object *pod_list_append(pod_list *list, pod_object *object);
extern pod_object *pod_list_pop(pod_list *list);
extern pod_object *pod_list_pop_last(pod_list *list);
extern pod_object *pod_list_push(pod_list *list, pod_object *object);

extern pod_object *pod_list_get(pod_list *list, pod_string *key);
extern pod_object *pod_list_set(pod_list *list, pod_string *key, pod_object *o);

extern pod_object *pod_list_at(pod_list *list, size_t pos);
extern pod_object *pod_list_change(pod_list *list, size_t pos, pod_object *to);
extern pod_object *pod_list_remove(pod_list *list, size_t pos);

extern pod_object *pod_list_next(pod_list *list, pod_string *key, size_t *pos);

extern size_t pod_list_size(pod_list *list);



#endif
