#include "scan.h"
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



    // scan_simple
    //
    // At this point, Pod has the first character of a simple string.
    // 
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int scan_simple(pod_stream *stream, pod_char_t c)
{
    int warning;

    warning = 0;
    switch (c) {
        case '\t': // Ends a simple string
        case '\n':
        case '\r':
        case ' ':
            stream->state = pod_start;
            break;
        case '"':  // A quote ends the simple string and starts a quoted string.
            pod_stream_add_token(stream, pod_token_string);
            stream->state = pod_quoted;
            break;
        case '+': // A concat mark (currently '+') ends the simple string
            stream->have_concat = true;
            stream->state = pod_start;
            break;
        case '<':
            pod_stream_add_token(stream, pod_token_string);
            pod_stream_add_token(stream, pod_token_begin_map);
            stream->state = pod_start;
            break;
        case '=':
            pod_stream_add_token(stream, pod_token_string);
            pod_stream_add_token(stream, pod_token_equals);
            stream->state = pod_start;
            break;
        case '>':
            pod_stream_add_token(stream, pod_token_string);
            pod_stream_add_token(stream, pod_token_end_map);
            stream->state = pod_start;
            break;
        case '[':
            pod_stream_add_token(stream, pod_token_string);
            stream->state = pod_blurb_pre_size;
            break;
        case '\\':
            stream->state = pod_simple_escape;
            break;
        case ']':
            pod_stream_add_token(stream, pod_token_string);
            stream->state = pod_start;
            // Syntax error: extraneous close bracket.
            warning = 1;
            break;
        case '{':
            pod_stream_add_token(stream, pod_token_string);
            pod_stream_add_token(stream, pod_token_begin_list);
            stream->state = pod_start;
            break;
        case '}':
            pod_stream_add_token(stream, pod_token_string);
            pod_stream_add_token(stream, pod_token_end_list);
            stream->state = pod_start;
            break;
        case '':
            pod_stream_add_token(stream, pod_token_string);
            pod_stream_add_token(stream, pod_token_pod_sync);
            stream->state = pod_start;
            break;
        default:
            if (c < 32) {
                // TODO warn, illegal char (?).  What about non-printing
                // characters that are above 31?
                warning = 1;
            } else {
                pod_string_append_char(stream->buffer, c);
            }
            break;
    }

    return warning;
}
