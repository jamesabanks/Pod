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
    int warning;

    warning = 0;
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
                warning = POD_EQUALS_NOT_PRECEDED_BY_KEY;
                pod_stream_log(stream, warning, __FILE__, __LINE__);
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
                warning = POD_UNMATCHED_END_MAP;
                pod_stream_log(stream, warning, __FILE__, __LINE__);
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
                warning = POD_UNMATCHED_END_LIST;
                pod_stream_log(stream, warning, __FILE__, __LINE__);
            }
            break;
        case pod_token_pod_sync:
            while (! pod_list_is_empty(stream->stack)) {
                pod_stream_merge_down(stream);
            }
            break;
        default:
            // Error: unknown token
            warning = POD_UNKNOWN_TOKEN;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
            break;
    }

    return warning;
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
    pod_mapping *mapping;
    pod_object *object;
    pod_object *popped;
    pod_object *top;
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
            warning = POD_EQUALS_AT_END;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
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
            pod_stream_log(stream, warning, __FILE__, __LINE__);
        } else {
            // Error: a mapping cannot be the value of another mapping
            // I don't think this can happen
            warning = POD_TWO_MAPPINGS;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
            popped->destroy(popped);
        }
    } else if (top->type == POD_MAP_TYPE) {
        if (popped->type == POD_MAPPING_TYPE) {
            object = pod_map_define_mapping((pod_map *) top,
                                            (pod_mapping *) popped);
            if (object != NULL) {
                object->destroy(object);
            }
        } else {
            // Error: tried to insert unnamed object into map
            // ie, < foo >
            warning = POD_INVALID_INSERT_INTO_MAP;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
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



    // pod_stream_read_char
    //
    // Read a pod from a file descriptor.

int pod_stream_read_char(pod_stream *stream, pod_char_t *c)
{
    int finished;
    ssize_t read_bytes;
    int warning;

    assert(stream->read_fd > -1);
    assert(stream->read_buffer != NULL);
    assert(stream->read_buffer_size > 0);

    finished = false;
    while (!finished) {
        if (stream->read_buffer_index >= stream->read_buffer_used) {
            read_bytes = read(stream->read_fd,
                              stream->read_buffer,
                              stream->read_buffer_size);
            if (read_bytes == 0) {
                ? pod_stream_terminate(stream, object);
                return POD_EOF;
                // TODO EOF
                // What does pod_stream_terminate do again?  I think it flushed
                // the stream.  Does it close the fd?  Does it set the stream
                // state?  And how do you tell the caller the stream has been
                // closed?
            } else if (read_bytes < 0) {
                return read_bytes;
                // TODO error
                // EAGAIN, EWOULDBLOCK  return error to caller.
                // EBADF bad fd, return to caller
                // EFAULT return to caller
                // EINTR fine, just do it again, or do we tell the caller?
                // EINVALID return error to caller
                // EIO return to caller
                // EISDIR return to caller
                // anything else, return to caller
            } else {
                stream->read_buffer_used = read_bytes;
                stream->read_buffer_index = 0;
                finished = true;
            }
        }
    }
        
    // Conversions (for example, from UTF8) go here.  There is no
    // conversion, currently.
    *c = (pod_char_t) stream->read_buffer[stream->read_buffer_index];
    ++stream->read_buffer_index;

    return warning;
}



    // pod_stream_read
    //
    // Read a pod from a file descriptor.


int pod_stream_read(pod_stream *stream, pod_object **object)
{
        warning = pod_stream_add_char(stream, c)
        // handle warning
        //   what if warning indicates input should be closed?
        if (stream->result_pod != NULL) {
            &object = stream->result_pod;
            finished = true;
        }
        if (c == '') {
            // I was going to say "finished = true".  What about when there
            // is no object?  Should I even worry about this here?
            // Probably not.  The return value from the pod_stream_add_char
            // call should tell me everything I need to know.  I think.
        }
    }

    return warning;
}



int pod_stream_write_char(stream, pod_char_t c)
{
    int finished;
    ssize_t num_written;
    int out;
    int warning;

    warning = POD_OKAY;
    out = (int) c;
    finished = false;
    while (!finished) {
        num_written = write(fd, &out, 1);
        if (num_written > 0) {
            // This should only ever be 1.
            finished = true;
        } else if (num_written < 0) {
            // TODO error
            // EAGAIN, EWOULDBLOCK  return error to caller
            // EBADF bad fd
            // EFAULT shouldn't happen (the buffer is the "out" variable
            // EFBIG exceeding file size limit
            // EINTR fine, just do it again.
            // EINVAL insuitable for writing
            // EIO return to caller
            // ENOSPC no room on device
            // EPIPE reading end of pipe is closed (preceded by SIGPIPE)
            // anything else, return to caller
        }
    }

    return warning;
}
    


    // pod_stream_write
    //
    // Write a pod to a file descriptor

int pod_stream_write(pod_stream *stream, pod_object *object)
{
    pod_char_t c;
    size_t index;
    pod_string *string;

    // pretty print version?
    if (object == NULL) // don't pass NULL!  This should be ASSERT only (?)

    switch (object->type) {
        case POD_STRING_TYPE:
            string = (pod_string *) object;
            pod_stream_write_char(stream, '"');
            for (index = 0; index < string->used; index++) {
                c = string->buffer[index];
                if (c is printable) {
                    pod_stream_write_char(stream, c);
                } else if (c is linefeed) {
                    pod_stream_write_char(stream, 10);
                } else if (c is carriage_return) {
                    pod_stream_write_char(stream, 13);
                } else if (c is tab) {
                    pod_stream_write_char(stream, 9);
                } else {
                    pod_stream_write_char(stream, '\\');
                    if (c == 0) {
                        pod_stream_write_char(stream, '0');
                    } else {
                        while (c != 0) {
                            digit = c & 0xf;
                            c << 4;
                            if (digit < 10) {
                                pod_stream_write_char(stream, '0' + digit);
                            } else {
                                digit -= 10;
                                pod_stream_write_char(stream, 'a' + digit);
                        }
                    }
                    pod_stream_write_char(stream, '\\');
                }
            }
            pod_stream_write_char(stream, '"');
            break;
        case POD_LIST_TYPE:
            list = (pod_list *) object;
            pod_stream_write_char(stream, '{');
            next_object = first_object_in_list;
            while (more objects(haven't reached list node yet)) {
                sub_object = next object;
                pod_stream_write(stream, object)
            }
            pod_stream_write_char(stream, '}');
            break;
        case POD_MAPPING_TYPE:
            mapping = (pod_mapping *) object;
            pod_stream_write(stream, (pod_object *) mapping->key);
            pod_stream_write_char(stream, '=');
            pod_stream_write(stream, mapping->value);
            break;
        default:
            // this is an error, do nothing
            break;
    }
    
}



    // pod_stream_default_process_pod
    //
    // This is the default was a pod object is processed.  It is put into the
    // stream object.  From there the programmer can get it and set the
    // stream->result_pod back to NULL.

void pod_stream_default_process_pod(pod_stream *stream, pod_object *object)
{
    // if (stream->process_pod == NULL) {
    //     stream->results = pod;
    // } else {
    //     stream->process_pod(stream, pod);
    // }
    stream->result_pod = object;
}
