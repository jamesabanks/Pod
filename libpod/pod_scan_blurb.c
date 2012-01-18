#include "pod_char.h"
#include "pod_log.h"
#include "pod_scan.h"



    // scan_blurb_pre_size
    //
    // At this point, Pod has a left square bracket.  The next character should
    // be a whitespace or a hex digit.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int pod_scan_blurb_pre_size(pod_stream *stream, pod_char_t c)
{
    int warning;

    warning = POD_OKAY;
    switch (c) {
        case POD_CHAR_TAB:
        case POD_CHAR_NEWLINE:
        case POD_CHAR_RETURN:
        case POD_CHAR(' '):
            // whitespace/newline - haven't found anything yet.
            break;
        case POD_CHAR_EOB:
            // unexpected end
            pod_stream_add_token(stream, POD_TOKEN_POD_SYNC);
            stream->s_state = POD_STATE_START;
            break;
        case POD_CHAR('0'):
        case POD_CHAR('1'):
        case POD_CHAR('2'):
        case POD_CHAR('3'):
        case POD_CHAR('4'):
        case POD_CHAR('5'):
        case POD_CHAR('6'):
        case POD_CHAR('7'):
        case POD_CHAR('8'):
        case POD_CHAR('9'):
            stream->s_state &= POD_STATE_MASK;
            stream->s_state |= pod_blurb_size;
            stream->blurb_size = (c - '0') & 0xf;
            break;
        case POD_CHAR('A'):
        case POD_CHAR('B'):
        case POD_CHAR('C'):
        case POD_CHAR('D'):
        case POD_CHAR('E'):
        case POD_CHAR('F'):
            stream->s_state &= POD_STATE_MASK;
            stream->s_state |= pod_blurb_size;
            stream->blurb_size = (10 + (c - 'A')) & 0xf;
            break;
        case POD_CHAR('a'):
        case POD_CHAR('b'):
        case POD_CHAR('c'):
        case POD_CHAR('d'):
        case POD_CHAR('e'):
        case POD_CHAR('f'):
            stream->s_state &= POD_STATE_MASK;
            stream->s_state |= pod_blurb_size;
            stream->blurb_size = (10 + (c - 'a')) & 0xf;
            break;
        default:
            // Syntax error - Got a character other than a whitespace or a hex
            //   digit (maybe a special warning if Pod got a ']').
            warning = 1; // TODO
    }

    return warning;
}



    // scan_blurb_size
    //
    // At this point, Pod has the first digit of the blurb size.  Pod is now
    // looking for further hex digits, a space, or a right-bracket (']').
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int pod_scan_blurb_size(pod_stream *stream, pod_char_t c)
{
    pod_char_t digit;
    int warning;

    warning = POD_OKAY;
    switch (c) {
        case POD_CHAR_TAB:
        case POD_CHAR_NEWLINE:
        case POD_CHAR_RETURN:
        case POD_CHAR(' '):
            // whitespace/newline - size has ended
            stream->s_state &= POD_STATE_MASK;
            stream->s_state |= pod_blurb_post_size;
            break;
        case POD_CHAR_EOB:
            // unexpected end
            pod_stream_add_token(stream, POD_TOKEN_POD_SYNC);
            stream->s_state = POD_STATE_START;
            break;
        case POD_CHAR(']'):
            // blurb ended
            // This should really set the stream into raw mode, where the
            //   blurb_size is decremented for every byte received, then return
            //   return to the start state.  But for now, just go back to the
            //   start state. (see also, scan_blurb_post_size)
            stream->s_state &= POD_STATE_MASK;
            //stream->s_state |= stream_blurb_data;
            stream->s_state |= POD_STATE_START;
            break;
        case POD_CHAR('0'):
        case POD_CHAR('1'):
        case POD_CHAR('2'):
        case POD_CHAR('3'):
        case POD_CHAR('4'):
        case POD_CHAR('5'):
        case POD_CHAR('6'):
        case POD_CHAR('7'):
        case POD_CHAR('8'):
        case POD_CHAR('9'):
            digit = (c - '0') & 0xf;
            stream->blurb_size *= 0x10;
            stream->blurb_size |= digit;
            break;
        case POD_CHAR('A'):
        case POD_CHAR('B'):
        case POD_CHAR('C'):
        case POD_CHAR('D'):
        case POD_CHAR('E'):
        case POD_CHAR('F'):
            digit = (10 + (c - 'A')) & 0xf;
            stream->blurb_size *= 0x10;
            stream->blurb_size |= digit;
            break;
        case POD_CHAR('a'):
        case POD_CHAR('b'):
        case POD_CHAR('c'):
        case POD_CHAR('d'):
        case POD_CHAR('e'):
        case POD_CHAR('f'):
            digit = (10 + (c - 'a')) & 0xf;
            stream->blurb_size *= 0x10;
            stream->blurb_size |= digit;
            break;
        default:
            // Syntax error - got a character other than a hex digit, a space/
            // newline, or a ']'.
            warning = 1; // TODO
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

int pod_scan_blurb_post_size(pod_stream *stream, pod_char_t c)
{
    int warning;

    warning = POD_OKAY;
    switch (c) {
        case POD_CHAR_NEWLINE:
        case POD_CHAR_RETURN:
            // newline
            // ignore
            break;
        case POD_CHAR_EOB:
            // unexpected end
            pod_stream_add_token(stream, POD_TOKEN_POD_SYNC);
            stream->s_state = POD_STATE_START;
            break;
        case POD_CHAR_TAB:
        case POD_CHAR(' '):
            // whitespace
            // ignore
            break;
        case POD_CHAR(']'):
            // blurb ended
            // This should really set the stream into raw mode, where the
            //   blurb_size is decremented for every byte received, then return
            //   return to the start state.  But for now, just go back to the
            //   start state (see also, scan_blurb_size).
            stream->s_state &= POD_STATE_MASK;
            //stream->s_state |= stream_blurb_data;
            stream->s_state = POD_STATE_START;
            break;
        default:
            // Syntax error: received something else while waiting for a ']'.
            warning = 1; // TODO
            break;
    }

    return warning;
}
