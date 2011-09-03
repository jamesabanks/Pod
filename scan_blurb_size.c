#include <scanner.h>



// At this point I have an open square bracket.  I am getting the first
// character of the blurb, which may be space or a hex digit.

int scan_blurb_presize(pod_stream *stream, pod_char_t c)
{
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
            // whitespace
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
            state = (stream->state & stream_state_mask) | stream_blurb_size;
            stream->blurb_size = (c - '0') & 0xf;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            state = (stream->state & stream_state_mask) | stream_blurb_size;
            stream->blurb_size = (10 + (c - 'A')) & 0xf
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            state = (stream->state & stream_state_mask) | stream_blurb_size;
            stream_blurb_size = (10 + (c - 'a')) & 0xf;
            break;
        default:
            // Syntax error
            warning = 1;
    }

    return warning;
}



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
