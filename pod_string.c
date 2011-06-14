#include <errno.h>
#include <stdlib.h>
#include "pod_string.h"



    // Initialize POD_STRING_TYPE to 0xb0001, which is POD_OBJECT_TYPE + 1
    // This means that pod_string was the first class written after pod_object.

const int POD_STRING_TYPE = 0x0b0001;



    // pod_construct_string
    //
    // Allocate memory for a new pod_string.  Initialize the members.
    //
    // Returns:
    //    The address of the new pod_string
    //    NULL if memory can't be allocated (with errno stored in error)

struct pod_string *pod_construct_string(int *error, size_t size, int flags)
{
    int i;
    size_t length;
    struct pod_string *string;

    length = sizeof(struct pod_string) + (size * sizeof(pod_char_t));
    string = (struct pod_string *) malloc(length);
    if (string == NULL) {
        *error = errno;
    } else {
        *error = 0;
        string->o.type = POD_STRING_TYPE;
        string->o.next = NULL;
        string->o.previous = NULL;
        string->o.destroy = &pod_destroy_string;
        string->size = size;
        string->used = 0;
        string->flags = flags;
        if (flags & POD_INIT_ZERO) {
            for (i = 0; i < size; i++) {
                string->buffer[i] = 0;
            }
        }
    }

    return string;
}



    // pod_destroy_string
    //
    // Zero out the entire structure if desired.  Then free the memory.

void pod_destroy_string(void *function)
{
    int i;
    struct pod_string *string;
    size_t size;

    string = (struct pod_string *) function;
    if (string->flags & POD_DESTROY_ZERO) {    
        size = sizeof(struct pod_string) + (string->size * sizeof(pod_char_t));
        for (i = 0; i < size; i++) {
            ((char *) string)[i] = 0;
        }
    }
    free(string);
}



    // pod_compare_strings
    //
    // Compare two pod_strings, called a and b.  The "size" member of a
    // pod_string isn't used and doesn't matter (though it is the upper bound
    // for the "used" member).  The "used" member is the length of the string.
    // The return values are the same as strcmp.
    //
    // Returns:
    //   -1 if a < b
    //    0 if a = b
    //    1 if a > b
    //   -1 if a is shorter than b (and equal to b until a ends)
    //    1 if a is longer than b (and equal to b until b ends)

int pod_compare_strings(struct pod_string *a, struct pod_string *b)
{
    size_t amount;
    size_t i;
    int result;

    if (a->used < b->used) {
        amount = a->used;
    } else {
        amount = b->used;
    }
    result = 0;
    for (i = 0; i < amount; i++) {
        if (a->buffer[i] < b->buffer[i]) {
            result = -1;
            break;
        } else if (a->buffer[i] > b->buffer[i]) {
            result = 1;
            break;
        }
    }
    if (result == 0) {
        if (a->used < b->used) {
            result = -1;
        } else if (a->used > b->used) {
            result = 1;
        }
    }

    return result;
}



    // pod_compare_string_to_cstring
    //
    // Compare a pod_string (with elements of pod_char_t) to a C string (a null
    // terminated array of char).  A pod_char_t element is demoted to a
    // standard char value for each comparison.
    //
    // Returns:
    //   -1 if ps < cs
    //    0 if ps = cs
    //    1 if ps > cs
    //   -1 if ps is shorter than cs (and equal to cs until ps ends)
    //    1 if ps is longer than cs (and equal to cs until cs ends)

int pod_compare_string_to_cstring(struct pod_string *ps, char *cs)
{
    char c;
    size_t i;
    int result;

    result = 0;
    for (i = 0; i < ps->used; i++) {
        if (cs[i] == 0) {
            result = 1;
            break;
        }
        c = (char) ps->buffer[i];
        if (c < cs[i]) {
            result = -1;
            break;
        } else if (c > cs[i]) {
            result = 1;
            break;
        }
    }
    if (result == 0) {
        if (cs[i] != 0) {
            result = -1;
        }
    }

    return result;
}



    // pod_copy_chars
    //
    // Copy data to an array of pod_char_t from another array of pod_char_t,
    // This routine doesn't handle overlapping arrays (it's like memcpy, not
    // memmove).

void pod_copy_chars(pod_char_t *to, pod_char_t *from, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++) {
        to[i] = from[i];
    }
}



void pod_copy_string(struct pod_string *to, struct pod_string *from)
{
    size_t amount;
    size_t i;

    if (to->size < from->used) {
        amount = to->size;
    } else {
        amount = from->used;
    }
    for (i = 0; i < amount; i++) {
        to->buffer[i] = from->buffer[i];
    }
    if (to->flags & POD_COPY_ZERO) {
        if (amount < to->size) {
            for (i = amount; i < to->size; i++) {
                to->buffer[i] = 0;
            }
        }
    }
    to->used = amount;
}



void pod_copy_string_from_cstring(struct pod_string *to, char *from)
{
    size_t amount;
    size_t i;

    for (i = 0; i < to->size; i++) {
        if (from[i] == 0) {
            break;
        }
        to->buffer[i] = (pod_char_t) from[i];
    }
    if (to->flags & POD_COPY_ZERO) {
        amount = i;
        if (amount < to->size) {
            for (i = amount; i < to->size; i++) {
                to->buffer[i] = 0;
            }
        }
        to->used = amount;
    } else {
        to->used = i;   
    }
}



void pod_copy_string_to_cstring(char *to, struct pod_string *from)
{
    size_t i;

    for (i = 0; i < from->used; i++) {
        to[i] = (char) from->buffer[i];
    }
    to[i] = 0;
}

