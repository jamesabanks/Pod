#include <errno.h>
#include <stdlib.h>
#include "pod_string.h"



    // Initialize POD_STRING_TYPE to 0xb0001, which is POD_OBJECT_TYPE + 1
    // This means that pod_string was the first class written after pod_object.

const int POD_STRING_TYPE = 0x0b0001;



    // pod_string_create
    //
    // Allocate memory for a new pod_string.  Initialize the members.
    //
    // Returns:
    //    The address of the new pod_string
    //    NULL if memory can't be allocated (with errno stored in error)

pod_string *pod_string_create(size_t size, int flags)
{
    int i;
    size_t length;
    pod_string *string;

    length = sizeof(pod_string) + (size * sizeof(pod_char_t));
    string = (pod_string *) malloc(length);
    if (string != NULL) {
        string->o.type = POD_STRING_TYPE;
        string->o.next = NULL;
        string->o.previous = NULL;
        string->o.destroy = pod_string_destroy;
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



    // pod_string_destroy
    //
    // Zero out the entire structure if desired.  Then free the memory.

void pod_string_destroy(void *target)
{
    int i;
    pod_string *string;
    size_t size;

    string = (pod_string *) target;
    if (string->flags & POD_DESTROY_ZERO) {    
        size = sizeof(pod_string) + (string->size * sizeof(pod_char_t));
        for (i = 0; i < size; i++) {
            ((char *) string)[i] = 0;
        }
    }
    free(string);
}



    // pod_string_append_char
    //
    // If there's room, add a pod_char_t onto the end of a pod_string.  If
    // there isn't room, fail silently.

void pod_string_append_char(pod_string *string, pod_char_t c)
{
    if (string->used < string->size) {
        string->buffer[string->used] = c;
        string->used++;
    }
}



    // pod_string_clear
    //
    // Fill the buffer up to used with zeros, then reset used to zero.

void pod_string_clear(pod_string *string)
{
    size_t i;
    size_t used;

    used = string->used;
    for (i = 0; i < used; i++) {
        string->buffer[i] = 0;
    }
    string->used = 0;
}
    


    // pod_string_compare
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

int pod_string_compare(pod_string *a, pod_string *b)
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



    // pod_string_compare_to_cstring
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

int pod_string_compare_to_cstring(pod_string *ps, char *cs)
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



    // pod_string_copy
    // 
    // Copy a pod_string to a pod_string.  The pod_strings can be different
    // useds or sizes.

void pod_string_copy(pod_string *to, pod_string *from)
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



    // pod_string_copy_from_cstring
    //
    // Copy to a pod_string from an array of char.  This operation is
    // self-limiting because it won't copy more than to->size characters.
    // Which might be more than desired, but it won't cause a runaway loop.

void pod_string_copy_from_cstring(pod_string *to, char *from)
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



    // pod_string_copy_to_cstring
    //
    // Copy from a pod_string to a provided array of char.  This mean that I'm
    // assuming the array of char has ALREADY been allocated either on the
    // stack or the heap, and it is NOT this function's responsibility.  A
    // trailing '\0' is appended.  So the target should have room for the
    // string's contents (string->used) plus 1 (the '\0').

void pod_string_copy_to_cstring(char *to, pod_string *from)
{
    size_t i;

    for (i = 0; i < from->used; i++) {
        to[i] = (char) from->buffer[i];
    }
    to[i] = '\0';
}



    // pod_string_dup
    //
    // Make an exact copy, including contents, of a pod_string.  Contents does
    // NOT include whatever is between used and size.  The flags used to create
    // the original pod_string are used to create the duplicate.  The new
    // string has the same size as the original.

pod_string *pod_string_dup(pod_string *string)
{
    pod_string *new_string;

    new_string = pod_string_create(string->size, string->flags);
    if (new_string != NULL) {
        pod_string_copy(new_string, string);
    }

    return new_string;
}



    // pod_string_duptext
    //
    // Make a copy of a pod_string.  The new string will have the same text
    // as the original, but the size of the new string will only be big enough
    // to hold the text.  There will be no unused part as there might be in the
    // original.

pod_string *pod_string_duptext(pod_string *string)
{
    pod_string *new_string;

    new_string = pod_string_create(string->used, string->flags);
    if (new_string != NULL) {
        pod_string_copy(new_string, string);
    }

    return new_string;
}
