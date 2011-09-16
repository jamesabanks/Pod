#include "pod_stream.h"
#include "scan.h"
#include "pod_log.h"



int pod_stream_merge_down(pod_stream *stream);



    // pod_stream_add_char
    //
    // Got sufficient input to constitute a character and have coverted it to a
    // pod_char_t.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int pod_stream_add_char(pod_stream *stream, pod_char_t c)
{
    int warning;

    warning = 0;
    if (stream->total_warnings >= 0) {
        if (stream->total_warnings >= stream->max_warnings) {
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
        case pod_start:
            warning = scan_start(stream, c);
            break;
        case pod_simple:
            warning = scan_simple(stream, c);
            break;
        case pod_simple_escape:
            warning = scan_escape(stream, c);
            break;
        case pod_simple_escape_hex:
            warning = scan_escape_hex(stream, c);
            break;
        case pod_quoted:
            warning = scan_quoted(stream, c);
            break;
        case pod_quoted_escape:
            warning = scan_escape(stream, c);
            break;
        case pod_quoted_escape_hex:
            warning = scan_escape_hex(stream, c);
            break;
        case pod_blurb_pre_size:
            warning = scan_blurb_pre_size(stream, c);
            break;
        case pod_blurb_size:
            warning = scan_blurb_size(stream, c);
            break;
        case pod_blurb_post_size:
            warning = scan_blurb_post_size(stream, c);
            break;
        case pod_end_escape:
            if (c == '\\') {
                stream->state = pod_start;
            }
            break;
        case pod_end_line:
            if (c == '\n' || c == '\r') {
                stream->state = pod_start;
            }
            break;
        case pod_end_pod:
            if (c == '.') {
                stream->state = pod_start;
            }
            break;
        default:
            warning = POD_INVALID_STREAM_STATE;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
            stream->state = pod_start;
            break;
    }
    if (warning != 0) {
        ++stream->total_warnings;
    }

    return warning;
}



int pod_stream_add_token(pod_stream *stream, pod_stream_token token)
{
    pod_string *current;
    pod_list *list;
    pod_map *map;
    pod_mapping *mapping;
    pod_object *top;

    top = pod_list_peek(stream->stack);
    switch (token) {
        case pod_token_string:
            if (top->type == POD_STRING_TYPE) {
                pod_stream_merge_down(stream);
            }
            current = pod_string_dup_text(stream->buffer);
            stream->buffer->used = 0;
            stream->have_concat = false;
            pod_list_push(stream->stack, (pod_object *) current);
            break;
        case pod_token_begin_map:
            if (top->type == POD_STRING_TYPE) {
                pod_stream_merge_down(stream);
            }
            map = pod_map_create();
            pod_list_push(stream->stack, (pod_object *) map);
            break;
        case pod_token_equals:
            if (top->type == POD_STRING_TYPE) {
                // make a new mapping with the string as the key
                // push it on the stack
                mapping = pod_mapping_create();
                mapping->key = (pod_string *) pod_list_pop(stream->stack);
                pod_list_push(stream->stack, (pod_object *) mapping);
            } else {
                // Error: the token prior to "=" needs to be a string.
            }
            break;
        case pod_token_end_map:
            if (top->type == POD_STRING_TYPE || top->type == POD_MAPPING_TYPE) {
                pod_stream_merge_down(stream);
            } 
            top = pod_list_peek(stream->stack);
            if (top->type == POD_MAP_TYPE) {
                pod_stream_merge_down(stream);
            } else {
                // Error: unmatched ">"
            }
            break;

        case pod_token_begin_blurb:
        case pod_token_end_blurb:
            break;

        case pod_token_begin_list:
            if (top->type == POD_STRING_TYPE) {
                pod_stream_merge_down(stream);
            }
            list = pod_list_create();
            pod_list_push(stream->stack, (pod_object *) list);
            break;
        case pod_token_end_list:
            if (top->type == POD_STRING_TYPE || top->type == POD_MAPPING_TYPE) {
                pod_stream_merge_down(stream);
            }
            top = pod_list_peek(stream->stack);
            if (top->type == POD_LIST_TYPE) {
                pod_stream_merge_down(stream);
            } else {
                // Error, encountered unmatched "}"
            }
            break;
        case pod_token_pod_sync:
            while (! pod_list_is_empty(stream->stack)) {
                pod_stream_merge_down(stream);
            }
            break;
        default:
            // Error: unknown token
            break;
    }
}



    // pod_stream_merge_down
    //
    // This routine takes whatever is on top of the stack and adds it into to
    // the next element down.  This only works if this second element is a
    // structure (that is, a list or a map).  Furthermore, if it's a map, then
    // only a mapping can be added.  If this routine is called with the top
    // element as a string with the POD_IS_KEY flag set, the routine will clear
    // the POD_IS_KEY flag.  So this routine shouldn't be called by the "="
    // token.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int pod_stream_merge_down(pod_stream *stream)
{
    int is_mapping;
    pod_mapping *mapping;
    pod_object *top;
    pod_object *popped;
    pod_string *string;
    int warning;

    warning = 0;

    top = pod_list_peek(stream->stack);

    if (top == NULL) {
        // The stack is empty, nothing to do
        return 0;
    } else if (top->type == POD_MAPPING_TYPE) {
        mapping = (pod_mapping *) top;
        if (mapping->value == NULL) {
            // Error: trailing "="
            // ie, foo = . -> foo= . -> foo(error)
            mapping = (pod_mapping *) pod_list_pop(stream->stack);
            pod_list_push(stream->stack, (pod_object *) mapping->key);
            mapping->key = NULL;
            mapping->o.destroy(mapping);
        }
    }

    popped = pod_list_pop(stream->stack);
    top = pod_list_peek(stream->stack);

    if (top == NULL) {
        // The stack is now empty, so popped is the whole pod.
        stream->process_pod(popped);
    } else if (top->type == POD_LIST_TYPE) {
        // The top of the stack is a list, so add popped.
        pod_list_append((pod_list *) top, popped);
    } else if (top->type == POD_MAPPING_TYPE) {
        if (popped->type != POD_MAPPING_TYPE) {
            ((pod_mapping *) top)->value = popped;
            warning = pod_stream_merge_down(stream);
        } else {
            // Error: a mapping cannot be the value of another mapping
            // I don't think this can happen
            popped->destroy(popped);
        }
    } else if (top->type == POD_MAP_TYPE) {
        if (popped->type == POD_MAPPING_TYPE) {
            pod_map_define_mapping((pod_map *) top, popped);
        } else {
            // Error: tried to insert unnamed object into map
            // ie, < foo >
            popped->destroy(popped);
        }
    } else {
        // Error: tried to insert element into string
        // I don't think this can happen
        popped->destroy(popped);
    }

    return warning;
}



void pod_stream_log(pod_stream *stream, int message, char *file_name, int line)
{
    // Example:
    //
    // Pod message 127: Got illegal character ')'
    //   stream: std_out
    //   at character xxxx (line xxx, column xx)
    //   message from pod_stream.c, line xxx
}
