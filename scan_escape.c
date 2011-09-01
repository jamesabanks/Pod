#include <scanner.h>



// At this point I have the leading backslash.  I am getting the escaped
// character, or the first character of the escaped hex number.

int scan_escape(pod_stream *stream, pod_char_t c)
{
    pod_char_t digit;
    int warning;

    warning = 0;
    switch (c) {
        case '\n': // Unexpected end
        case '\r': // Unexpected end
        case '': // Unexpected end
            // There is no escape character, and the token ends.
            if (!pod_string_is_empty(stream->buffer) {
                add_token(stream, stream_string, object);
            }
            if (c == '') {
                add_token(stream, stream_pod_sync, object);
            }
            state = stream_start;
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
            state = (stream->state & stream_state_mask) | stream_escape_hex;
            digit = (c - '0') & 0xf;
            number *= 0x10;
            number |= digit;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            state = (stream->state & stream_state_mask) | stream_escape_hex;
            digit = (10 + (c - 'A')) & 0xf
            number *= 0x10;
            number |= digit;
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            state = (stream->state & stream_state_mask) | stream_escape_hex;
            digit = (10 + (c - 'a')) & 0xf;
            number *= 0x10;
            number |= digit;
            break;
        case 't':
            pod_string_append_char(stream->buffer, '\t');
            stream->state &= stream_state_mask;
            break;
        case 'n':
            pod_string_append_char(stream->buffer, '\n');
            stream->state &= stream_state_mask;
            break;
        case 'r':
            pod_string_append_char(stream->buffer, '\r');
            stream->state &= stream_state_mask;
            break;
        default:
            if (c < 32) {
                // TODO warn, illegal char (?).  What about non-printing
                // characters that are above 31?
                warning = 1;
            } else {
                pod_string_add_char(stream->buffer, c);
                stream->state &= stream->state & stream_state_mask;
            }
            break;
    }

    return warning;
}
