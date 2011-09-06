#include <scanner.h>
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

int scan_simple(pod_stream *stream, pod_char_t c, pod_object **object)
{
    int warning;

    warning = 0;
    switch (c) {
        case '\t': // Ends a simple string
        case '\n':
        case '\r':
        case ' ':
            stream->state = stream_start
            break;
        case '"':  // A quote ends the simple string and starts a quoted string.
            add_token(stream, stream_string, object);
            stream->state = stream_quoted;
            break;
        case '+': // A concat mark (currently '+') ends the simple string
            have_concat = true;
            stream->state = stream_start;
            break;
        case '<':
            add_token(stream, stream_string, object);
            add_token(stream, stream_begin_map, object);
            stream->state = stream_start;
            break;
        case '=':
            add_token(stream, stream_string, object);
            add_token(stream, stream_equals, object);
            stream->state = stream_start;
            break;
        case '>':
            add_token(stream, stream_string, object);
            add_token(stream, stream_end_map, object);
            stream->state = stream_start;
            break;
        case '[':
            add_token(stream, stream_string, object);
            stream->state = stream_blurb_pre_size;
            break;
        case '\\':
            stream->state = stream_simple_escape;
            break;
        case ']':
            add_token(stream, stream_string, object);
            stream->state = stream_start;
            // Syntax error: extraneous close bracket.
            warning = 1;
            break;
        case '{':
            add_token(stream, stream_string, object);
            add_token(stream, stream_begin_list, object);
            break;
        case '}':
            add_token(stream, stream_string, object);
            add_token(stream, stream_end_list, object);
            break;
        case '':
            add_token(stream, stream_string, object);
            add_token(stream, stream_pod_sync, object);
            break;
        default:
            if (c < 32) {
                // TODO warn, illegal char (?).  What about non-printing
                // characters that are above 31?
                warning = 1;
            } else {
                pod_string_add_char(stream->buffer, c);
            }
            break;
    }

    return warning;
}
