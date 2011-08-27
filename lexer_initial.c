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

int scan_start(pod_stream *stream, pod_char_t c, int *next_state)
{
    int state;
    int warning;

    state = stream_start;
    warning = 0;
    switch (c) {
        case '\t': // Nothing to do.  Skip.
        case '\n':
        case '\r':
        case ' ':
            break;
        case '"':  // Start a quoted string
            if ((have_concat == false) && (have_string)) { add_token(string); }
            state = stream_quoted;
            break;
        case '+':
            have_concat = true;
            break;
        case '<':
            have_concat = false;
            if (have_string) { add_token(string); }
            add_token(begin_map);
            break;
        case '=':
            have_concat = false;
            if (have_string) { add_token(string); }
            add_token(equals);
            break;
        case '>':
            have_concat = false;
            if (have_string) { add_token(string); }
            add_token(end_map);
            break;
        case '[':
            have_concat = false;
            if (have_string) { add_token(string); }
            add_token(begin_blurb);
            break;
        case '\\':
            if ((have_concat == false) && (have_string)) { add_token(string); }
            state = stream_string_escape;
            break;
        case ']':
            have_concat = false;
            if (have_string) { add_token(string); }
            add_token(end_blurb);
            // TODO Shouldn't get ']' in start state.
            break;
        case '{':
            have_concat = false;
            if (have_string) { add_token(string); }
            add_token(begin_list);
            break;
        case '}':
            have_concat = false;
            if (have_string) { add_token(string); }
            add_token(end_list);
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
