#include <scanner.h>

(remember, state is blurb)
for now,
when ']' is encountered,
    new state = lexer_initial
    return

scan_blurb_presize
scan_blurb_size
scan_blurb_endian
scan_blurb_prewordsize
scan_blurb_wordsize
scan_blurb_data

[sp* h* sp+ [l|b] sp+ h* sp oct* sp*]

