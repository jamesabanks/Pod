#ifndef INCLUDE_POD_SCAN_STATE_H
#define INCLUDE_POD_SCAN_STATE_H



typedef enum pod_scan_state {
    POD_STATE_MASK = 0xf0,              /* binary  1111 0000 */
    POD_STATE_ESCAPE_MASK = 0x0f,       /* binary  0000 1111 */
    POD_STATE_ESCAPE_HEX = 0x0c,        /* binary  0000 1100 */
    POD_STATE_START = 0,
    POD_STATE_SIMPLE = 0x10,            /* binary  0001 0000 */
    POD_STATE_SIMPLE_ESCAPE = 0x18,     /* binary  0001 1000 */
    POD_STATE_SIMPLE_ESCAPE_HEX = 0x1c, /* binary  0001 1100 */
    POD_STATE_QUOTED = 0x20,            /* binary  0010 0000 */
    POD_STATE_QUOTED_ESCAPE = 0x28,     /* binary  0010 1000 */
    POD_STATE_QUOTED_ESCAPE_HEX = 0x2c, /* binary  0010 1100 */
    //pod_blurb = 0x30,                   /* binary  0011 0000 */
    pod_blurb_pre_size = 0x31,          /* binary  0011 0001 */
    pod_blurb_size = 0x32,              /* binary  0011 0010 */
    pod_blurb_post_size = 0x33,         /* binary  0011 0011 */
    pod_blurb_data = 0x34,              /* binary  0011 0100 */
    pod_end_escape = 0x40,
    pod_end_line = 0x41,
    pod_end_pod = 0x42
} pod_scan_state;




#endif /* INCLUDE_POD_SCAN_STATE_H */
