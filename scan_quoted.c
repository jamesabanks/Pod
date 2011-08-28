#include <scanner.h>

/*
    {   '\n',   0,              scan_start }
    {   '\r',   0               scan_start }
    {   '"',    0,              scan_start }
    {   '\\',   0,              scan_quoted_escape }
    {   other,  0,              scan_quoted }
    { other<32, 0,              scan_quoted, warn }

(remember, state is quoted)
find character in list
if char is other
    add to string
state = new_state
*/

int scan_quoted(
    pod_stream *stream,
    pod_char_t c,
    pod_object **object,
    int *next_state
)
{
    int state;
    int warning;

    state = stream_quoted;
    warning = 0;
    switch (c) {
        case '\n': // Ends a quoted string
        case '\r':
            state = stream_start;
            break;
        case '"':  // A quote ends the quoted string.
            state = stream_start;
            break;
        case '\\':
            state = stream_quoted_escape;
            break;
        case '':
            add_token(stream, stream_string, object);
            add_token(stream, stream_pod_sync, object);
            state = stream_start;
            break;
        default:
            if (c < 32) {
                // TODO warn, illegal char (?).  What about non-printing
                // characters that are above 31?
            } else {
                pod_string_add_char(stream->buffer, c);
            }
            break;
    }
    *next_state = state;

    return warning;
}
