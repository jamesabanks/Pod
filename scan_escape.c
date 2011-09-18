#include "scan.h"



    // scan_escape
    //
    // At this point I have the leading backslash.  I am getting the escaped
    // character, or the first character of the escaped hex number.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int scan_escape(pod_stream *stream, pod_char_t c)
{
    int warning;

    warning = 0;
    switch (c) {
        case '\n': // Unexpected end
        case '\r': // Unexpected end
        case '': // Unexpected end
            // There is no escape character, and the token ends.
            if (!pod_string_is_empty(stream->buffer)) {
                pod_stream_add_token(stream, pod_token_string);
            }
            if (c == '') {
                pod_stream_add_token(stream, pod_token_pod_sync);
            }
            stream->state = pod_start;
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
            stream->state &= pod_state_mask;
            stream->state |= pod_escape_hex;
            stream->escape_value = (c - '0') & 0xf;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            stream->state &= pod_state_mask;
            stream->state |= pod_escape_hex;
            stream->escape_value = (10 + (c - 'A')) & 0xf;
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            stream->state &= pod_state_mask;
            stream->state |= pod_escape_hex;
            stream->escape_value = (10 + (c - 'a')) & 0xf;
            break;
        case 't':
            pod_string_append_char(stream->buffer, '\t');
            stream->state &= pod_state_mask;
            break;
        case 'n':
            pod_string_append_char(stream->buffer, '\n');
            stream->state &= pod_state_mask;
            break;
        case 'r':
            pod_string_append_char(stream->buffer, '\r');
            stream->state &= pod_state_mask;
            break;
        default:
            if (c < 32) {
                // TODO warn, illegal char (?).  What about non-printing
                // characters that are above 31?
                // Syntax error: got control (non-printing, < 32) character.
                warning = 1;
            } else {
                pod_string_append_char(stream->buffer, c);
                stream->state &= pod_state_mask;
            }
            break;
    }

    return warning;
}
