#include "scanner.h"
#include "pod_stream.h"



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
    pod_string *string;
    pod_map *map;

    switch (token) {
        case string:
            string = pod_string_dup_text(stream->buffer);
            pod_string_clear(stream->buffer);
            stream->have_concat = false;
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
