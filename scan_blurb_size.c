#include <scanner.h>



    // scan_blurb_pre_size
    //
    // At this point I have a left square bracket.  The next character should
    // be a whitespace or a hex digit.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int scan_blurb_pre_size(pod_stream *stream, pod_char_t c, pod_object *object)
{
    int warning;

    warning = 0;
    switch (c) {
        case '\t':
        case '\n':
        case '\r':
        case ' ':
            // whitespace/newline - haven't found anything yet.
            break;
        case '':
            // unexpected end
            add_token(stream, stream_pod_sync, object);
            stream->state = stream_start;
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
            stream->state &= stream_state_mask;
            stream->state |= stream_blurb_size;
            stream->blurb_size = (c - '0') & 0xf;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            stream->state &= stream_state_mask;
            stream->state |= stream_blurb_size;
            stream->blurb_size = (10 + (c - 'A')) & 0xf
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            stream->state &= stream_state_mask;
            stream->state |= stream_blurb_size;
            stream_blurb_size = (10 + (c - 'a')) & 0xf;
            break;
        default:
            // Syntax error - Got a character other than a whitespace or a hex
            //   digit (maybe a special warning if Pod got a ']').
            warning = 1;
    }

    return warning;
}



    // scan_blurb_size
    //
    // At this point Pod has the first digit of the blurb size.  Pod is now
    // looking for further hex digits, a space, or a right-bracket (']').
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int scan_blurb_size(pod_stream *stream, pod_char_t c, pod_object *object)
{
    pod_char_t digit;
    int warning;

    warning = 0;
    switch (c) {
        case '\t':
        case '\n':
        case '\r':
        case ' ':
            // whitespace/newline - size has ended
            stream->state &= stream_state_mask;
            stream->state |= stream_blurb_post_size;
            break;
        case '':
            // unexpected end
            add_token(stream, stream_pod_sync, object);
            stream->state = stream_start;
            break;
        case ']':
            // blurb ended
            // This should really set the stream into raw mode, where the
            //   blurb_size is decremented for every byte received, then return
            //   return to the start state.  But for now, just go back to the
            //   start state. (see also, scan_blurb_post_size)
            stream->state &= stream_state_mask;
            //stream->state |= stream_blurb_data;
            stream->state |= stream_start;
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
            digit = (c - '0') & 0xf;
            stream->blurb_size *= 0x10;
            stream->blurb_size |= digit;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            digit = (10 + (c - 'A')) & 0xf;
            stream->blurb_size *= 0x10;
            stream->blurb_size |= digit;
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            digit = (10 + (c - 'a')) & 0xf;
            stream->blurb_size *= 0x10;
            stream->blurb_size |= digit;
            break;
        default:
            // Syntax error - got a character other than a hex digit, a space/
            // newline, or a ']'.
            warning = 1;
            break;
    }

    return warning;
}



    // scan_blurb_post_size
    //
    // Pod has received a space, so the blurb size has ended.  Now Pod is
    // looking for the final character, a ']'.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int scan_blurb_post_size(pod_stream *stream, pod_char_t c, pod_object *object)
{
    int warning;

    warning = 0;
    switch (c) {
        case '\n':
        case '\r':
            // newline
            // ignore
            break;
        case '':
            // unexpected end
            add_token(stream, stream_pod_sync, object);
            stream->state = stream_start;
            break;
        case ' ':
        case '\t':
            // whitespace
            // ignore
            break;
        case ']':
            // blurb ended
            // This should really set the stream into raw mode, where the
            //   blurb_size is decremented for every byte received, then return
            //   return to the start state.  But for now, just go back to the
            //   start state (see also, scan_blurb_size).
            stream->state &= stream_state_mask;
            //stream->state |= stream_blurb_data;
            stream->state |= stream_start;
            break;
        default:
            // Syntax error: received something else while waiting for a ']'.
            warning = 1;
            break;
    }

    return warning;
}
