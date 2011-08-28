#include <pod_stream.h>



void pod_stream_add_char(
    pod_stream *stream,
    pod_char_t c,
    pod_object **object,
    int *error
)
{
    if (stream->state & lexer_escape_mask) {
        (*error) = lexer_handle_escape(c, &replace, &replacement, &finished);
        if (replace) {
            buffer->add_char(replacement);
        }
        if (finished) {
            state = next_state & lexer_state_mask;
        }
    } else {
        switch (stream->state) {
            case lexer_initial:
                lexer_handle_initial(stream, c, &next_state);
                break;
            case lexer_string:
                lexer_handle_string(stream, c, &finish_string, &send_token, &next_state);
                break;
            case lexer_quoted:
                lexer_handle_quoted(stream, c, &finish_string, &send_token, &next_state);
            case lexer_blurb:
                lexer_handle_blurb(stream, c, &next_state);
                state = next_state;
            default:
                error;
        }
    }
    if (finish_string) {
        send_parse_token(string);
    }
    if (send_token) {
        send_parse_token(send_token);
    }
    state = next_state;
}



int add_token(pod_stream *stream, int token, pod_object **object)
{
    pod_string *string;
    pod_map *map;

    switch (token) {
        case string:
            string = pod_string_dup_text(stream->buffer);
            pod_string_clear(stream->buffer);
            stream->have_concate = false;
            if already have a string:
                Append it to current pod_object or if there is no current
                pod_object, assign it to object.
            put string in current_string position
            break;
        case begin_map:
            if already have a string:
                Append it to current pod_object or if there is no current
                pod_object, assign it to object.
            map = pod_create_map();
            push map on current pod_object stack
            break;
        case equals:
            if NOT already have a string:
                warn
                create null string
            pod_create_mapping();
            mapping->key = old_string (or null string);
            push mapping on current pod_object stack or if there is nothing on
                the current stack, assign it to object an return it.
            break;
        case end_map:
            if already have a string
                append it to current top of stack.  if the stack is empty,
                assign it to object to return it.
            if the top of the current object stack is not a map
                warn
                ignore
            else
                remove from top of stack
                append to new (and previous :) top of stack
                if there is no new top of stack (ie, the stack is empty),
                    assign it to object and return it
            break;

        case begin_blurb:
        case end_blurb:
            break;

        case begin_list:
            if already have a string
                append it to current top of stack.  if the stack is empty,
                assign it to object to return it.
            list = pod_list_create()
            push list on current stack
            break;
        case end_list:
            if already have a string
                append it to current top of stack.  if the stack is empty,
                assign it to object to return it.
            if the top of stack is not a list
                warn
                ignore
            else
                remove from top of stack
                append to new top of stack
                if there is no new top of stack (ie, the stack is empty),
                    assign it to object and return it.
            break;
        case pod_sync:
            if already have a string
                append it to current top of stack.  if the stack is empty,
                assign it to object to return it.
            if stack is not empty
                pod the top of stack
                append to new top of stack
                repeat until empty
                assign to object and return it.
            break;
        default:
