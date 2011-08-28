/*
        'c'       token         next state
    {   '\t',     0,            stream_start } // tab
    {   '\n',     0,            stream_start } // new line
    {   '\r',     0,            stream_start } // carriage return
    {   ' ',      0,            stream_start } // space

    {   '"',      0,            stream_quoted }
    {   '+',      0,            stream_start }
    {   '<',      begin_map,    stream_start }
    {   '=',      equals,       stream_start }
    {   '>',      end_map,      stream_start }
    {   '[',      begin_blurb,  stream_blurb }
    {   '\\',     0,            stream_string_escape }
    {   ']',      end_blurb,    stream_start, warn }
    {   '{',      begin_list,   stream_start }
    {   '}',      end_list,     stream_start }

    { other<32, 0,              stream_start, warn }
    { other,    0,              stream_string }

if char is '+', set have_concat to true
else if char sends token
    set have_concat to false
    if there is a string send(string)
    send(token)
if char is '"', '\\', or other
    if have_concat is false and there is a string
        send(string)
    if char is other, add to string
state = new_state
*/

int scan_start(
    pod_stream *stream,
    pod_char_t c,
    pod_object **object,
    int *next_state
)
{
    int have_string;
    int state;
    int warning;

    have_string = ! pod_stream_is_empty(stream->buffer);
    state = stream_start;
    warning = 0;
    switch (c) {
        case '\t': // Do nothing.
        case '\n':
        case '\r':
        case ' ':
            break;
        case '"':  // Start a quoted string
            if ((!stream->have_concat) && (have_string)) {
                add_token(stream, stream_string, object);
            }
            state = stream_quoted;
            break;
        case '+':
            stream->have_concat = true;
            break;
        case '<':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_begin_map, object);
            break;
        case '=':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_equals, object);
            break;
        case '>':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_end_map, object);
            break;
        case '[':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_begin_blurb, object);
            break;
        case '\\':
            if ((!stream->have_concat) && (have_string)) {
                add_token(stream, stream_string, object);
            }
            state = stream_string_escape;
            break;
        case ']':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_end_blurb);
            // TODO Shouldn't get ']' in start state.
            break;
        case '{':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_begin_list, object);
            break;
        case '}':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_end_list, object);
            break;
        case '':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_pod_sync, object);
            break;
        default:
            if (c < 32) {
                // TODO warn, illegal char (?).  What about non-printing
                // characters that are above 31?
            } else {
                pod_string_add_char(stream->buffer, c);
                state = stream_simple;
            }
            break;
    }
    *next_state = state;

    return warning;
}
