#include "pod_char.h"
#include "pod_scan.h"



    // pod_scan_quoted
    //
    // Pod has the opening quote.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)
    
int pod_scan_quoted(pod_stream *stream, pod_char_t c)
{
    int warning;

    warning = 0;
    switch (c) {
        case POD_CHAR_NEWLINE: // Ends a quoted string
        case POD_CHAR_RETURN:
            // Error: missing the '"' which ends the quoted string.
            warning = 1; // TODO
            stream->s_state = POD_STATE_START;
            break;
        case POD_CHAR('"'):  // A quote ends the quoted string.
            stream->s_state = POD_STATE_START;
            break;
        case POD_CHAR('\\'):
            stream->s_state = POD_STATE_QUOTED_ESCAPE;
            break;
        case POD_CHAR_EOB:
            if (!pod_string_is_empty(stream->s_buffer)) {
                pod_stream_add_token(stream, POD_TOKEN_STRING);
            }
            pod_stream_add_token(stream, POD_TOKEN_POD_SYNC);
            stream->s_state = POD_STATE_START;
            // Error: missing the '"' which ends the quoted string.
            warning = 1;
            break;
        default:
            pod_string_append_char(stream->s_buffer, c);
            break;
    }

    return warning;
}
