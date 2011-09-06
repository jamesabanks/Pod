#include <scanner.h>



    // scan_escape_hex
    //
    // At this point Pod has the leading backslash and one hex digit.  Pod is
    // looking for another hex digit or a terminating backslash.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

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
            stream->state &= stream_state_mask;
            break;
        case '\n': // Unexpected end
        case '\r': // Unexpected end
        case '': // Unexpected end
            // The token ended.
            pod_string_append_char(stream->buffer, stream->escape_value);
            add_token(token_string);
            stream->state = stream_start;
            // Error: the end was unexpected, so emit a warning.
            warning = 1;
            break;
        default:
            if (stream->escape_size >= stream->escape_max_size) {
                // Error: Too many digits.  Ignore the escape.  Go to the end
                //   of the escape.
                warning = 1;
            } else {
                ++stream->escape_size;
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
                        // Error: got a character that wasn't a hex digit.
                        //   Ignore the escape.  Go to the end of the escape.
                        warning = 1;
                        break;
                }
            }
    }

    return warning;
}
