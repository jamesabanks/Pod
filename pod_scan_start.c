#include "pod_boolean.h"
#include "pod_log.h"
#include "pod_scan.h"
#include "pod_scan_state.h"
#include "pod_scan_token.h"



    // pod_scan_start
    // 
    // At this point, Pod has nothing.  Well, it might have a string and/or a
    // a concat ("+").  But it might have nothing.
    //
    // Returns:
    //      int     The error id of any problem that occurred (see pod_log.h)

int pod_scan_start(pod_stream *stream, pod_char_t c)
{
    int have_string;
    int no_concat;
    int warning;

    have_string = ! pod_string_is_empty(stream->s_buffer);
    no_concat = ! stream->have_concat;
    warning = POD_OKAY;
    switch (c) {
        case POD_CHAR_TAB: // Do nothing.
        case POD_CHAR_NEWLINE:
        case POD_CHAR_RETURN:
        case POD_CHAR(' '):
            break;
        case POD_CHAR('"'):  // Start a quoted string
            if (have_string && no_concat) {
                // If there is already a string, but no concat, push it.  Pod
                // doesn't push the string until now because the next token
                // might be '+'
                pod_stream_add_token(stream, POD_TOKEN_STRING);
            }
            // Start the new string
            stream->s_state = POD_QUOTED;
            break;
        case POD_CHAR('+'):
            if (stream->have_concat) { // aka (! no_concat)
                // Multiple concats in a row is a syntax error, but an easily
                // ignorable one.
                warning = 1; // TODO
            } else {
                stream->have_concat = true;
            }
            break;
        case POD_CHAR('='):
            if (have_string) {
                pod_stream_add_token(stream, POD_TOKEN_STRING);
            }
            pod_stream_add_token(stream, POD_TOKEN_EQUALS);
            break;
//        case '[':
//            if (have_string) {
//                pod_stream_add_token(stream, pod_token_string);
//            }
//            pod_stream_add_token(stream, pod_token_begin_blurb);
//            break;
        case POD_CHAR('\\'):
            if (have_string && no_concat) {
                pod_stream_add_token(stream, POD_TOKEN_STRING);
            }
            stream->s_state = POD_SIMPLE_ESCAPE;
            break;
//        case ']':
//            if (have_string) {
//                pod_stream_add_token(stream, pod_token_string);
//            }
//            pod_stream_add_token(stream, pod_token_end_blurb);
//            // TODO Shouldn't get ']' in start state.
//            warning = 1;
//            break;
        case POD_CHAR('{'):
            if (have_string) {
                pod_stream_add_token(stream, POD_TOKEN_STRING);
            }
            pod_stream_add_token(stream, POD_TOKEN_BEGIN_LIST);
            break;
        case POD_CHAR('}'):
            if (have_string) {
                pod_stream_add_token(stream, POD_TOKEN_STRING);
            }
            pod_stream_add_token(stream, POD_TOKEN_END_LIST);
            break;
        case POD_CHAR_EOB:
            if (have_string) {
                pod_stream_add_token(stream, POD_TOKEN_STRING);
            }
            pod_stream_add_token(stream, POD_TOKEN_POD_SYNC);
            break;
        default:
            if (! POD_CHAR_IS_PRINTING(c)) {
                warning = 1; // TODO
            } else {
                if (have_string && no_concat) {
                    pod_stream_add_token(stream, POD_TOKEN_STRING);
                }
                pod_string_append_char(stream->s_buffer, c);
                stream->s_state = POD_SIMPLE;
            }
            break;
    }

    return warning;
}
