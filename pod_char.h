#ifndef INCLUDE_POD_CHAR_H
#define INCLUDE_POD_CHAR_H

#include <stdint.h> /* for uint32 */



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

typedef unsigned long pod_char_t;



#endif /* INCLUDE_POD_CHAR_H */

