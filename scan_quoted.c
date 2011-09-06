#include <scanner.h>



    // scan_quoted
    //
    // Pod has the opening quote.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)
    
int scan_quoted(pod_stream *stream, pod_char_t c, pod_object **object)
{
    int warning;

    warning = 0;
    switch (c) {
        case '\n': // Ends a quoted string
        case '\r':
            // Error: missing the '"' which ends the quoted string.
            warning = 1;
            stream->state = stream_start;
            break;
        case '"':  // A quote ends the quoted string.
            stream->state = stream_start;
            break;
        case '\\':
            state = stream_quoted_escape;
            break;
        case '':
            if (!pod_string_is_empty(stream->buffer) {
                add_token(stream, stream_string, object);
            }
            add_token(stream, stream_pod_sync, object);
            stream->state = stream_start;
            // Error: missing the '"' which ends the quoted string.
            warning = 1;
            break;
        default:
            pod_string_add_char(stream->buffer, c);
            break;
    }

    return warning;
}
