/******************************************************************** 
 *  pod_scan_append_to_buffer.c
 *  Copyright (c) 2011-2012, James A. Banks
 *  All rights reserved.
 *  See file LICENSE for details.
 ********************************************************************/

#include "pod_char.h"
#include "pod_string.h"



    // pod_scan_append_to_buffer
    //
    // Add a character to the buffer if the buffer has room.  Return a warning
    // if there isn't room.
    //
    // Returns:
    //      int     The error id of any problem that occurred (see pod_log.h)

int pod_scan_append_to_buffer(pod_string *buffer, pod_char_t c)
{
    int warning;

    warning = 0;
    if (buffer->used < buffer->size) {
        pod_string_append_char(buffer, c);
    } else {
        warning = 1; // TODO Terminal
    }

    return warning;
}
