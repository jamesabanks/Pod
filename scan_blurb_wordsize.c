#include <scanner.h>

(remember, state is blurb)
for now,
when ']' is encountered,
    new state = lexer_initial
    return

scan_blurb_presize
scan_blurb_size
scan_blurb_endian
scan_blurb_prewordsize
scan_blurb_wordsize
scan_blurb_data

[sp* h* sp+ [l|b] sp+ h* sp oct* sp*]



// At this point I have a space, meaning the space/tab after the endian
// designator.  I need to start looking for the word size.  The word size only
// needs to be two digits, as the word size is in bytes (octets), so a word
// size of 4 is 32 bits.  A word size of 10 => 0x10 (16 decimal) is 256 bits.
// Word size must always be in powers of two.  

int scan_blurb_prewordsize(pod_stream *stream, pod_char_t c)
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
            // more whitespace; do nothing
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
            state = (stream->state & stream_state_mask) | stream_blurb_wordsize;
            stream->blurb_wordsize = (c - '0') & 0xf;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            state = (stream->state & stream_state_mask) | stream_blurb_wordsize;
            stream->blurb_wordsize = (10 + (c - 'A')) & 0xf
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            state = (stream->state & stream_state_mask) | stream_blurb_wordsize;
            stream_blurb_wordsize = (10 + (c - 'a')) & 0xf;
            break;
        default:
            // Syntax error
            warning = 1;
    }

    return warning;
}



int scan_blurb_wordsize(pod_stream *stream, pod_char_t c)
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
            // single whitespace separating the word size from the data
            break;
        case '\t':
            // either a syntax error or same as ' '
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
            stream->blurb_word_size *= 0x10;
            stream->blurb_word_size |= digit;
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
