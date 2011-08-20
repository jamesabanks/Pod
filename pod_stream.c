#include <pod_stream.h>


{ state,    char,   pre-terminal,   post-terminal,  next_state }

  initial   '{'     yes             yes             initial
                    if have string, send token(string)
                    send token(begin_list)
                    return to initial state
  initial   '+'     no              no              initial
  initial   ' '|\t  no              no              initial
  initial   nl|cr   yes             no              initial
  initial   '\'     depends         no              in_string
  initial   else    depends         no              in_string

    make escape sequences \blah\
        \\ becomes \
        \126\ becomes ~
        \x7e\ becomes ~
        \n\ becomes nl
        etc.

    if nl|cr causes the current string to be sent to the parser, then + must
    be a parser thing, not a lexer thing because concatenation must be able to
    combine strings on multiple lines.  That makes determining when to send a
    token much easier.
  
{ state,    char,   token,          final state }

{ empty,    '{',    begin_list,     in_list }
{ empty,    '}',    end_list,       empty (warn) }
{ empty,    '<',    begin_map,      in_map }
{ empty,    '>',    end_map,        empty }
{ empty,    '[',    begin_blurb,    in_blurb (size) }
{ empty,    ']',    end_blurb,      empty }
{ empty,    '"',    begin_quoted,   in_quoted }
{ empty,    '=',    equals,         empty (warn) }
{ empty,    ws,     none,           empty }
{ empty,    nl|cr,  none,           empty }
{ empty,    '+',    got_concat,     got_concat }
{ empty,    '\',    got_escape,     got_escape }
{ empty,    else,   begin_string,   in_string }

void pod_stream_add_char(
    pod_stream *stream,
    pod_char_t c,
    pod_object **object,
    int error
)
{
    what do we do given c and the current state?
    what is the next state based on c?
    pass on a token?
    state = new_state?

    if ((stream->state == stream_in_string || (stream->state == stream_in_quoted)) {
        (put a name on the stream for better error tracking?)
        if we're over max_string_size, return error
        if we're over warn_string_size, emit warning
            
    }
    switch (stream->state) {
        case stream_empty:
            switch (c) {
                case '{':
                    // if we have a string in the buffer, call add_token for it.
                    // don't add to buffer, buffer should be empty
                    // add_token(start_ordered)
                    // new_state = stream_in_ordered
                    break;
                case '}':
                    // if we have a string in the buffer, call add_token for it.
                    // don't add to buffer, buffer should be empty  
                    // add_token(end_ordered), should get warning
                    // new_state = stream_empty (no change)
                    break;
                case '"':
                    new_state = stream_in_quoted;
                case '<':
                case '>':
                (?) case ',':
                        
            }
        case stream_in_ordered:
        default:
    }
}
