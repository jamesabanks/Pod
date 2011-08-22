#include <pod_stream.h>


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

Try again (getting lexer and parser confused):

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
        \{\ becomes {
        etc.

    if nl|cr causes the current string to be sent to the parser, then + must
    be a parser thing, not a lexer thing because concatenation must be able to
    combine strings on multiple lines.  That makes determining when to send a
    token much easier. (Optimizing too soon?)

    On the return to initial, submit token.  A change of state to initial
    submits a token.  Single character tokens (such as '{') can be thought of
    as a change to another state, then a change back, all in a single character.
    Ws never send tokens, although they terminate (outsize of "") others.
    Characters which change the state back to initial are terminal.

blurb = [length word-size endianess blurb]

But are blurbs text-based or binary?  Should there be both?

How does the lexer (and the parser, too) need to change to accomidate blurbs?
Have a special function to handle blurbs?

    Parameters:  state, character, finish-previous, send-this-token, next-state
    States:  initial, in_string, in_quoted
    Special situations:  escape_sequence, blurb

enum pod_lexer_state {
    state = 0xe,            /* binary 1110 */
    escape = 0x1,           /* binary 0001 */
    initial = 0,
    in_string = 0x2,        /* binary 0010 */
    in_string_escape = 0x3, /* binary 0011 */
    in_quoted = 0x4,        /* binary 0100 */
    in_quoted_escape = 0x5, /* binary 0101 */
    in_blurb = 0x6          /* binary 0110 */
}

if (state & pod_lexer_state.escape) {
    func_handle_escape(c, &replace_char, &replacement, &next_state);
    if (replace_char) {
        buffer->add_char(replacement);
    }
    if (state != next_state) {
        state = next_state & pod_lexer_state.state;
    }
} else if (state == in_blurb) {
    func_handle_blurb(c, &next_state);
    state = next_state;
} else {
    switch (state) {
        case initial:
            func_initial(c, &finish_string, &send_token, &next_state)
            break;
        case in_string:
            finish_string = false;
            func_in_string(c, &finish_string, &send_token, &next_state)
            break;
        case in_quoted:
            finish_string = false;
            func_in_quoted(c, &finish_string, &send_token, &next_state);
        default:
            error;
    }
    if (finish_string) {
        send_parse_token(string);
    }
    if (send_token) {
        send_parse_token(send_token);
    }
    state = next_state;
}

I don't know what I was thinking last night about strings and concantenation.  So I'm changing my mind again.  Initially, or when a string ends, a flag, called...say...have_concat, is set to false.  Set the flag to false and finish the string if any other token is processed.  If a concat token (currently the '+' character), set the flag to true.  If another string is encountered with the have_concat set to false then finish the previous string and start another.  If the have_concat flag is true, do NOT finish the string.  Just continue putting new characters in the buffer where you left off.  The '+' character is not a token, it's lexer notation.  That means treat it like a token but don't send anything to the parser.  Furthermore, this+word becomes thisword, but "this+word" becomes this+word.  So change is required in the initial and in_string states, but *not* the in_quoted.

func_handle_escape can return a warning.  So can func_handle_blurb.  Anything that submits a token can return a pod_object.




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
