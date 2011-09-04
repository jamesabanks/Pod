#include <scanner.h>



// At this point I have a left square bracket.  I am getting the first
// character of the blurb, which may be space or a hex digit.

int scan_blurb_presize(pod_stream *stream, pod_char_t c)
{
    int warning;

    warning = 0;
    switch (c) {
        case '\n':
        case '\r':
            // new line.
            // ignore
            break;
        case '':
        case ']':
            // unexpected end
            break;
        case '\t':
        case ' ':
            // whitespace
            // ignore
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
            // Syntax error
            warning = 1;
    }

    return warning;
}



// At this point I have the first digit of the blurb size.  I am looking for
// further digits, a space, or a right-bracket (']').
int scan_blurb_size(pod_stream *stream, pod_char_t c)
{
    pod_char_t digit;
    int warning;

    warning = 0;
    switch (c) {
        case '\n':
        case '\r':
            // newline
            break;
        case '':
            // unexpected end
            break;
        case ' ':
        case '\t':
            stream->state &= stream_state_mask;
            stream->state |= stream_blurb_post_size;
            // whitespace - size has ended.
            break;
        case ']':
            // blurb ended
            stream->state &= stream_state_mask;
            stream->state |= stream_blurb_data;
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
            // Syntax error
            warning = 1;
    }

    return warning;
}



int scan_blurb_postsize(pod_stream *stream, pod_char_t c)
{
    pod_char_t digit;
    int warning;

    warning = 0;
    switch (c) {
        case '\n':
        case '\r':
        case '':
            // unexpected end
            break;
        case ' ':
        case '\t':
            state = (stream->state & stream_state_mask) | stream_blurb_size;
            // whitespace
            break;
        case ']':
            // blurb ended
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
            // Syntax error
            warning = 1;
    }

    return warning;
}
