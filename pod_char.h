#ifndef INCLUDE_POD_CHAR_H
#define INCLUDE_POD_CHAR_H

#include <stdint.h> /* for uint32 */
#include <stddef.h>



// Everything that Pod processes is, as far as it is concerned, an unsigned
// 32-bit integer.  Promotions, demotions, conversions, and whatever else need
// to be taken care of before hand.  The input characters {,},=,\,", and
// assorted whitespace have the promoted ASCII values.  I intend this to
// work with UTF-32.  With over 4 billion possible characters, I'm hoping it
// will be enough for a while.

// Well, actually, hmm.  There is nothing magical about this.  8-bit integers
// are fine for some (most? a few? anything that uses octets?) uses.  Maybe
// this typedef belongs in its own file.  For now I'll use uint32 from
// stdint.h.  So actually, as far as Pod is concerned, everything is an
// integer at least able to represent ASCII.  A single size of integer per
// program.

// A stream of anything can be converted to individual units, as long as
// appropriate input characters map to the appropriate ASCII values.  For
// instance, UTF-8 can be read in and converted to 32-bit values.  If the
// character is more than 1 byte then multiple bytes are converted to a single
// 32-bit value.

typedef uint32_t pod_char_t;



    // pod_char_t related functions

extern void pod_char_copy(pod_char_t *to, pod_char_t *from, size_t n);



    // useful #defines
    //
    // Is using a macro (or #define) better than just typing out the
    // definition?

#define POD_CHAR_NULL ((pod_char_t) '\0')
#define POD_CHAR_TAB ((pod_char_t) '\t') // ?
#define POD_CHAR_NEWLINE ((pod_char_t) '\n')
#define POD_CHAR_RETURN ((pod_char_t) '\r')
#define POD_CHAR_EOB ((pod_char_t) '')

#define POD_CHAR_IS_PRINTING(c) ((c) >= 32 && (c) != 128) ? 1 : 0
#define POD_CHAR(c) ((pod_char_t) (c))



#endif /* INCLUDE_POD_CHAR_H */

