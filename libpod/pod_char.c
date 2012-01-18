#include "pod_char.h"



    // pod_char_copy
    //
    // Copy data to an array of pod_char_t from another array of pod_char_t,
    // This routine doesn't handle overlapping arrays (it's like memcpy, not
    // memmove).

void pod_char_copy(pod_char_t *to, pod_char_t *from, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++) {
        to[i] = from[i];
    }
}




