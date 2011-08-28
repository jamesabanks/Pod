#include <scanner.h>

/*
    {   '\n',   0,              scan_start }
    {   '\r',   0               scan_start }
    {   '"',    0,              scan_start }
    {   '\\',   0,              scan_quoted_escape }
    {   other,  0,              scan_quoted }
    { other<32, 0,              scan_quoted, warn }

(remember, state is blah_escaped)
find character in list
if char is other
    add to string
state = new_state
*/

int scan_escape(
    pod_stream *stream,
    int *next_state
)
{
    int state;
    int warning;

    warning = 0;
    switch (c) {
        case '\t':
        case '\n':
        case '\r':
            state = stream_start;
            break;
        case ' ':
            break;
        case '\\':
            state &= stream_escape_mask;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            break;
        case 't':
        case 'n':
        case 'r':
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
