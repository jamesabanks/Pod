#include "pod_char.h"
#include "pod_log.h"
#include "pod_scan.h"



    // pod_scan_escape_hex
    //
    // At this point Pod has the leading backslash and one hex digit.  Pod is
    // looking for another hex digit or a terminating backslash.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int pod_scan_escape_hex(pod_stream *stream, pod_char_t c)
{
    pod_char_t digit;
    int warning;

    warning = POD_OKAY;
    switch (c) {
        case POD_CHAR('\\'): // End of escape
            // Put the char in the buffer.  Exit escape state.
            pod_string_append_char(stream->s_buffer, stream->escape_value);
            stream->s_state &= POD_STATE_MASK;
            break;
        case POD_CHAR_NEWLINE:  // Unexpected end
        case POD_CHAR_RETURN:
        case POD_CHAR_EOB:
            // The token ended.
            pod_string_append_char(stream->s_buffer, stream->escape_value);
            pod_stream_add_token(stream, POD_TOKEN_STRING);
            stream->s_state = POD_STATE_START;
            // Error: the end was unexpected, so emit a warning.
            warning = 1; // TODO
            break;
        default:
            if (stream->escape_size >= stream->escape_max_size) {
                // Error: Too many digits.  Ignore the escape.  Go to the end
                //   of the escape.  Actually, this is a terminal error.
                //   Otherwise one could have an infinitely long escape.
                warning = 1;  // TODO Terminal
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
                        // If POD_CHAR_BITS is not a multiple of four, then
                        //   excess bits are shifted (or mul'd) into oblivion.
                        stream->escape_value *= 0x10;
                        stream->escape_value |= digit;
                        break;
                    case 'A':
                    case 'B':
                    case 'C':
                    case 'D':
                    case 'E':
                    case 'F':
                        digit = (10 + (c - 'A')) & 0xf;
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
                        warning = 1; // TODO
                        break;
                }
            }
    }

    return warning;
}
