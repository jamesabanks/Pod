
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

int scan_simple(pod_stream *stream, pod_char_t c, int *next_state)
{
    int state;
    int warning;

    state = stream_simple;
    warning = 0;
    switch (c) {
        case '\t': // Ends a simple string
        case '\n':
        case '\r':
        case ' ':
            state = stream_start
            break;
        case '"':  // A quote ends the simple string and starts a quoted string.
            add_token(string);
            state = stream_quoted;
            break;
        case '+': // A concat mark (currently '+') ends the simple string
            have_concat = true;
            state = stream_start;
            break;
        case '<':
            add_token(string);
            add_token(begin_map);
            state = stream_start;
            break;
        case '=':
            add_token(string);
            add_token(equals);
            state = stream_start;
            break;
        case '>':
            add_token(string);
            add_token(end_map);
            state = stream_start;
            break;
        case '[':
            add_token(string);
            add_token(begin_blurb);
            state = stream_start;
            break;
        case '\\':
            state = stream_simple_escape;
            break;
        case ']':
            add_token(string);
            add_token(end_blurb);
            // TODO Shouldn't get ']' in simple string state.
            break;
        case '{':
            add_token(string);
            add_token(begin_list);
            break;
        case '}':
            add_token(string);
            add_token(end_list);
            break;
        default:
            if (c < 32) {
                // TODO warn, illegal char (?).  What about non-printing
                // characters that are above 31?
            } else {
                pod_string_add_char(stream->buffer, c);
            }
            break;
    }
    *next_state = state;

    return warning;
}
