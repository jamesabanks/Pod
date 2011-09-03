#include <scanner.h>



// At this point I have the leading backslash and one hex digit.
// Where do I store the digit?  Somewhere in the stream variable.
// How do I keep track of how many digits I have?  How many digits
// should I allow?  Put them in the stream variable, too.

int scan_escape_hex(pod_stream *stream, pod_char_t c)
{
    pod_char_t digit;
    int warning;

    switch (c) {
        case '\\': // End of escape
            // Put the char in the buffer.  Exit escape state.
            pod_string_append_char(stream->buffer, stream->escape_value);
            stream->state = stream->state & stream_state_mask;
            break;
        case '\n': // Unexpected end
        case '\r': // Unexpected end
        case '': // Unexpected end
            // The token ended.
            // The end was unexpected, so emit a warning.
            pod_string_append_char(stream->buffer, stream->escape_value);
            add_token(token_string);
            stream->state = stream_start;
            warning = 1;
            break;
        default:
            if (stream->escape_size >= stream->escape_max_size) {
                // There is a problem.  Too many digits.  Ignore the excess.
                // Emit a warning.
                warning = 1;
            } else {
                stream->escape_size += 4;
                switch (c) {
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
                        digit = (c - '0') & 0xf;
                        stream->escape_value *= 0x10;
                        stream->escape_value |= digit;
                        break;
                    case 'A':
                    case 'B':
                    case 'C':
                    case 'D':
                    case 'E':
                    case 'F':
                        digit = (10 + (c - 'A')) & 0xf
                        stream->escape_value *= 0x10;
                        stream->escape_value |= digit;
                        break;
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                        digit = (10 + (c - 'a')) & 0xf;
                        stream->escape_value *= 0x10;
                        stream->escape_value |= digit;
                        break;
                    default:
                        // Got a character that wasn't a hex digit.
                        // Emit a warning
                        warning = 1;
                        break;
                }
            }
    }

    return warning;
}
