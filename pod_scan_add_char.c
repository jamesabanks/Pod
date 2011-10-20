#include "pod_scan.h"
#include "pod_log.h"



    // pod_scan_add_char
    //
    // Got sufficient input to constitute a character and have coverted it to a
    // pod_char_t.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int pod_scan_add_char(pod_stream *stream, pod_char_t c)
{
    int warning;

    warning = 0;
    if (stream->total_warnings >= 0) {
        if (stream->total_warnings >= stream->max_warnings) {
            // warn that the stream is ignoring any further input
            warning = POD_TOO_MANY_WARNINGS;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
            return warning;
        }
    }
    ++stream->total_characters;
    ++stream->position;
    if (c == '\n' || c == '\r') {
        stream->position = 1;
        ++stream->line;
    }
    stream->c = c;
    switch (stream->s_state) {
        case POD_SCAN_START:
            warning = scan_start(stream, c);
            break;
        case POD_SIMPLE:
            warning = scan_simple(stream, c);
            break;
        case POD_SIMPLE_ESCAPE:
            warning = scan_escape(stream, c);
            break;
        case POD_SIMPLE_ESCAPE_HEX:
            warning = scan_escape_hex(stream, c);
            break;
        case POD_QUOTED:
            warning = scan_quoted(stream, c);
            break;
        case POD_QUOTED_ESCAPE:
            warning = scan_escape(stream, c);
            break;
        case POD_QUOTED_ESCAPE_HEX:
            warning = scan_escape_hex(stream, c);
            break;
        case pod_blurb_pre_size:
            warning = scan_blurb_pre_size(stream, c);
            break;
        case pod_blurb_size:
            warning = scan_blurb_size(stream, c);
            break;
        case pod_blurb_post_size:
            warning = scan_blurb_post_size(stream, c);
            break;
        case pod_end_escape:
            if (c == '\\') {
                stream->s_state = POD_SCAN_START;
            }
            break;
        case pod_end_line:
            if (c == '\n' || c == '\r') {
                stream->s_state = POD_SCAN_START;
            }
            break;
        case pod_end_pod:
            if (c == '.') {
                stream->s_state = POD_SCAN_START;
            }
            break;
        default:
            warning = POD_INVALID_STREAM_STATE;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
            stream->s_state = POD_SCAN_START;
            break;
    }
    if (warning != 0) {
        ++stream->total_warnings;
    }

    return warning;
}

