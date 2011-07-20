#ifndef INCLUDE_POD_LIST_H
#define INCLUDE_POD_LIST_H

#include <stddef.h>
#include "pod_object.h"



// I'm finding that struct blah-blah wastes too much space so I'll try it
// without (that is, use a typedef).


    // Type value is POD_OBJECT_TYPE + 2

extern const int POD_LIST_TYPE;



struct pod_list;
typedef struct pod_list pod_list;

struct pod_list {
    pod_object o;
    pod_object *first;
    pod_object *last;
};


    // One may need to walk a list and do something to the objects.

typedef pod_object *pod_list_apply(pod_list *list, pod_object *object);


    // Constructor and destructor

extern pod_list *pod_list_create(void);
extern void pod_list_destroy(void *target);


    // Other pod_list-related functions

extern void pod_list_append(pod_list *list, pod_object *object);
extern pod_object *pod_list_peek(pod_list *list);
extern pod_object *pod_list_pop(pod_list *list);
extern void pod_list_push(pod_list *list, pod_object *object);

extern pod_object *pod_list_find(pod_list *list, size_t pos);
extern pod_object *pod_list_insert(pod_list *list, size_t pos, pod_object *obj);
extern pod_object *pod_list_remove(pod_list *list, size_t pos);
extern pod_object *pod_list_replace(pod_list *list, size_t pos, pod_object *to);

extern size_t pod_list_size(pod_list *list);
extern void pod_list_apply_all(pod_list *list, pod_list_apply *apply);



#endif /* INCLUDE_POD_LIST_H */
