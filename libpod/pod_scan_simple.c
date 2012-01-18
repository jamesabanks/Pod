#include "pod_boolean.h"
#include "pod_log.h"
#include "pod_scan.h"

/*
    {   '\t',   0,              stream_start }
    {   '\n',   0,              stream_start }
    {   '\r',   0,              stream_start }
    {   ' ',    0,              stream_start }
    {   '"',    0,              stream_quoted }
    {   '+',    0,              stream_start }
    {   '<',    begin_map,      stream_start }
    {   '=',    equals,         stream_start }
    {   '>',    end_map,        stream_start }
    {   '[',    begin_blurb,    stream_blurb }
    {   '\\',   0,              stream_simple_escape }
    {   ']',    end_blurb,      stream_start, error }
    {   '{',    begin_list,     stream_start }
    {   '}',    end_list,       stream_start }
    {   other,  0,              stream_simple }
    { other<32, 0,              stream_simple, warn }

(remember, state is simple)
find character in list
if char is '+', set have_concat to true
else if char sends token
    set have_concat to false
    send(string)
    send(token)
if char is '"'
    set have_concat to false
    send(string)
if char is other
    add to string
state = new_state
*/



    // pod_scan_simple
    //
    // At this point, Pod has the first character of a simple string.
    // 
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int pod_scan_simple(pod_stream *stream, pod_char_t c)
{
    int warning;

    warning = POD_OKAY;
    switch (c) {
        case POD_CHAR_TAB:      // Ends a simple string
        case POD_CHAR_NEWLINE:
        case POD_CHAR_RETURN:
        case POD_CHAR(' '):
            stream->s_state = POD_STATE_START;
            break;
        case POD_CHAR('"'):
            // A quote ends the simple string and starts a quoted string.
            pod_stream_add_token(stream, POD_TOKEN_STRING);
            stream->s_state = POD_STATE_QUOTED;
            break;
        case POD_CHAR('+'):
            // A concat mark (currently '+') ends the simple string
            stream->have_concat = true;
            stream->s_state = POD_STATE_START;
            break;
        case POD_CHAR('='):
            pod_stream_add_token(stream, POD_TOKEN_STRING);
            pod_stream_add_token(stream, POD_TOKEN_EQUALS);
            stream->s_state = POD_STATE_START;
            break;
//        case '[':
//            pod_stream_add_token(stream, pod_token_string);
//            stream->state = pod_blurb_pre_size;
//            break;
        case POD_CHAR('\\'):
            stream->s_state = POD_STATE_SIMPLE_ESCAPE;
            break;
//        case ']':
//            pod_stream_add_token(stream, pod_token_string);
//            stream->state = pod_start;
//            // Syntax error: extraneous close bracket.
//            warning = 1;
//            break;
        case POD_CHAR('{'):
            pod_stream_add_token(stream, POD_TOKEN_STRING);
            pod_stream_add_token(stream, POD_TOKEN_BEGIN_LIST);
            stream->s_state = POD_STATE_START;
            break;
        case POD_CHAR('}'):
            pod_stream_add_token(stream, POD_TOKEN_STRING);
            pod_stream_add_token(stream, POD_TOKEN_END_LIST);
            stream->s_state = POD_STATE_START;
            break;
        case POD_CHAR_EOB:
            pod_stream_add_token(stream, POD_TOKEN_STRING);
            pod_stream_add_token(stream, POD_TOKEN_POD_SYNC);
            stream->s_state = POD_STATE_START;
            break;
        default:
            if (! POD_CHAR_IS_PRINTING(c)) {
                warning = 1; // TODO
            } else {
                warning = pod_scan_append_to_buffer(stream->s_buffer, c);
            }
            break;
    }

    return warning;
}
