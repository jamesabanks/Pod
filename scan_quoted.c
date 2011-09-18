#include "scan.h"



    // scan_quoted
    //
    // Pod has the opening quote.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)
    
int scan_quoted(pod_stream *stream, pod_char_t c)
{
    int warning;

    warning = 0;
    switch (c) {
        case '\n': // Ends a quoted string
        case '\r':
            // Error: missing the '"' which ends the quoted string.
            warning = 1;
            stream->state = pod_start;
            break;
        case '"':  // A quote ends the quoted string.
            stream->state = pod_start;
            break;
        case '\\':
            stream->state = pod_quoted_escape;
            break;
        case '':
            if (!pod_string_is_empty(stream->buffer)) {
                pod_stream_add_token(stream, pod_token_string);
            }
            pod_stream_add_token(stream, pod_token_pod_sync);
            stream->state = pod_start;
            // Error: missing the '"' which ends the quoted string.
            warning = 1;
            break;
        default:
            pod_string_append_char(stream->buffer, c);
            break;
    }

    return warning;
}
