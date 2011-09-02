#include <scanner.h>



int scan_blurb_endian(pod_stream *stream, pod_char_t c)
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
        case 'B':
        case 'b':
            // big endian
            break;
        case 'L':
        case 'l':
            // little endian
            break;
        default:
            // syntax error
            warning = 1;
    }

    return warning;
}
