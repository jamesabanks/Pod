#include "pod_stream.h"
#include "scanner.h"



    // pod_stream_add_char
    //
    // Got sufficient input to constitute a character and have coverted it to a
    // pod_char_t.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int pod_stream_add_char(pod_stream *stream, pod_char_t c, pod_object **object)
{
    int warning;

    warning = 0;
    if (stream->total_warnings >= 0) {
        if (stream->total_warnings >= stream_max_warnings) {
            // warn that the stream is ignoring any further input
            warning = POD_TOO_MANY_WARNINGS;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
            return warning;
        }
    }
    ++stream->total_characters;
    ++stream->position;
    if (c == '\n' || c == '\r') {
        stream->position = 1;
        ++stream->line;
    }
    stream->c = c;
    switch (stream->state) {
        case stream_start:
            warning = scan_start(stream, c, object);
            break;
        case stream_simple:
            warning = scan_simple(stream, c, object);
            break;
        case stream_simple_escape:
            warning = scan_escape(stream, c, object);
            break;
        case stream_simple_escape_hex:
            warning = scan_escape_hex(stream, c, object);
            break;
        case stream_quoted:
            warning = scan_quoted(stream, c, object);
            break;
        case stream_quoted_escape:
            warning = scan_escape(stream, c, object);
            break;
        case stream_quoted_escape_hex:
            warning = scan_escape_hex(stream, c, object);
            break;
        case stream_blurb:
        case stream_blurb_pre_size:
            warning = scan_blurb_pre_size(stream, c, object);
            break;
        case stream_blurb_size:
            warning = scan_blurb_size(stream, c, object);
            break;
        case stream_blurb_post_size:
            warning = scan_blurb_post_size(stream, c, object);
            break;
        case stream_end_escape:
            if (c == '\\') {
                stream->state = stream_start;
            }
            break;
        case stream_end_line:
            if (c == '\n' || c == '\r') {
                stream->state = stream_start;
            }
            break;
        case stream_end_pod:
            if (c == '') {
                stream->state = stream_start;
            }
            break;
        default:
            warning = POD_INVALID_STREAM_STATE;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
            stream->state = stream_start;
            break;
    }
    if (warning != 0) {
        ++stream->total_warnings;
    }

    return warning;
}



int add_token(pod_stream *stream, pod_stream_token token, pod_object **object)
{
    pod_string *previous;
    pod_string *current;
    pod_map *map;
    pod_object *top;
    pod_object *new;
    pod_object *null;

    switch (token) {
        case string:
            current = pod_string_dup_text(stream->buffer);
            stream->buffer->used = 0;
            stream->have_concat = false;
            if (stream->previous_string != NULL) {
                top = pod_list_peek(stream->stack);
                previous = stream->previous_string;
                if (top == NULL) {
                    // The stack is empty, so this is the whole pod.
                    (*object) = previous;

                } else if (top->type == POD_LIST_TYPE) {
                    // The top of the stack is a list, so add this.
                    pod_list_append((pod_list *) top, (pod_object *) previous);

                } else if (top->type == POD_MAP_TYPE) {
                    // Maps can contain only named objects so, for example,
                    // < "foo" > becomes < "foo" = "" >
                    null = (pod_object *) pod_string_create_null();
                    pod_map_define((pod_map *) top, previous, null);

                } else if (top->type == POD_MAPPING_TYPE) {
                    // The top of the stack is a mapping, so this is the value.
                    ((pod_mapping *) top)->value = previous;
                    top = pod_list_pop(stream->stack);
                    new = pod_list_peek(stream->stack);
                    if (new == NULL) {
                        // This is the whole pod.
                        (*object) = top;
                    } else if (new->type == POD_LIST_TYPE) {
                        // The top is a list, so add this.
                        pod_list_append((pod_list *) new, (pod_object *) top);
                    } else if (new->type == POD_MAP_TYPE) {
                        // The top is a map, so add this.
                        pod_map_define((pod_map *) new, (pod_mapping *) top);
                    } else if (new->type == POD_MAPPING_TYPE) {
                        // I don't think this can happen, but...
                        // syntax error
                        // eg: "ask" = "about" = "pod" is illegal
                        // err...whatever
                    } else {
                        // Error: invalid type (stack is for containers)
                    }

                } else {
                    // Error: invalid type (stack is for containers)
                }
                previous = NULL;
            }
            stream->previous_string = current;
            break;
        case begin_map:
            if already have a string:
                Append it to current pod_object or if there is no current
                pod_object, assign it to object.
            map = pod_create_map();
            push map on current pod_object stack
            break;
        case equals:
            if (stream->previous_string != NULL) {
                top = pod_list_peek(stream->stack);
                previous = stream->previous_string;
                if (top->type == POD_MAPPING_TYPE) {
                    // The top of the stack is also a mapping, so finish it.
                    null = (pod_object *) pod_string_create_null();
                    ((pod_mapping *) top)->value = null;
                    top = pod_list_pop(stream->stack);
                    new = pod_list_peek(stream->stack);
                    if (new == NULL) {
                        // This is the whole pod.
                        (*object) = top;
                    } else if (new->type == POD_LIST_TYPE) {
                        // The top is a list, so add this.
                        pod_list_append((pod_list *) new, (pod_object *) top);
                    } else if (new->type == POD_MAP_TYPE) {
                        // The top is a map, so add this.
                        pod_map_define((pod_map *) new, (pod_mapping *) top);
                    } else if (new->type == POD_MAPPING_TYPE) {
                        // I don't think this can happen, but...
                        // syntax error
                        // eg: "ask" = "about" = "pod" is illegal
                        // err...whatever
                    } else {
                        // Error: invalid type (stack is for containers)
                    }
                }
                mapping = pod_create_mapping();
                mapping->key = stream->previous_string;
                pod_list_push(stream->stack, (pod_object *) mapping);
            } else {
                // syntax error, eg: < = "foo" >
            }
  

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
            break;
}



void pod_stream_log(pod_stream stream, int message, char *file_name, int line)
{
    // Example:
    //
    // Pod message 127: Got illegal character ')'
    //   stream: std_out
    //   at character xxxx (line xxx, column xx)
    //   message from pod_stream.c, line xxx
}
