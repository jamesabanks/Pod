#ifndef INCLUDE_POD_STRING_H
#define INCLUDE_POD_STRING_H

#include <stddef.h>
#include "pod_object.h"
#include "pod_char.h"



// Strings are basically what they traditionally are: arrays of characters.  In
// this case, pod_char_t.  However, these strings are not null terminated.
// They have a maximum size (size) and a length in use (used).  In this sense,
// they are more like Pascal strings than C strings.

// TODO (I'm thinking about this...) Keys must be strings, so
// I stuck a value member in every string because I don't see the point of
// another struct (that would be the key-value pair).

// TODO The size element of the string is of type size_t, but the size of a
// string is really in units of sizeof(pod_char_t).  On OS X x86, size_t is 32
// bits.  (probably, I haven't verified this) intended for addresses at a byte
// level.  Is size_t good to use?

// TODO Other functions that might be useful: pod_string_append,
// pod_string_dup, pod_string_copy_to_wstring, pod_string_copy_from_wstring

// DONE pod_copy_string and friends should be pod_string_copy and such.  In
// this way namespace conflicts/confusions can be minimized.  Or the function
// can be more easily be associated with pod_string.

// TODO If malloc fails, it always returns NULL with errno set to ENOMEM, so
// pod_string_create doesn't need the error parameter.  Get rid of it. 


    // Type value is POD_OBJECT_TYPE + 1

extern const int POD_STRING_TYPE;


    // Flags are:
    // POD_INIT_ZERO - Initialize the buffer to zero during construction
    // POD_DESTROY_ZERO - Overwrite buffer with zeros before freeing memory
    // POD_COPY_ZERO - Write zeros to remaining space after buffer has been
    //  copied to.

enum pod_string_flags {
    POD_INIT_ZERO = 0x01,
    POD_DESTROY_ZERO = 0x02,
    POD_COPY_ZERO = 0x04
};


    // Allocation size for a pod_string is sizeof(struct pod_string) +
    // (n * sizeof(pod_char_t)).

struct pod_string {
    struct pod_object o;
    size_t size;
    size_t used;
    int flags;
//    struct pod_object *value;   // only used if this is a key in a KV pair
    pod_char_t buffer[];
};


    // Constructor and destructor

extern struct pod_string *pod_string_create(int *error, size_t size, int f);
extern void pod_string_destroy(void *string);


    // Other pod_string-related functions

extern int pod_string_compare(struct pod_string *a, struct pod_string *b);
extern int pod_string_compare_to_cstring(struct pod_string *ps, char *cs);
extern void pod_string_copy(struct pod_string *to, struct pod_string *from);
extern void pod_string_copy_from_cstring(struct pod_string *to, char *from);
extern void pod_string_copy_to_cstring(char *to, struct pod_string *from);



#endif /* INCLUDE_POD_STRING_H */

