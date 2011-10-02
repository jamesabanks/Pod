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
    // Read a byte from a file descriptor and convert it to a pod_char_t.
    //
    // Returns:
    //      < 0         An I/O error, what the OS returns.
    //      POD_OKAY    Successfully got a character.
    //      POD_EOF     At end of file, no character

int pod_stream_read_char(pod_stream *stream, pod_char_t *c)
{
    ssize_t read_bytes;

    assert(stream->read_fd > -1);
    assert(stream->read_buffer != NULL);
    assert(stream->read_buffer_size > 0);

    if (stream->read_buffer_index >= stream->read_buffer_used) {
        read_bytes = read(stream->read_fd,
                          stream->read_buffer,
                          stream->read_buffer_size);
        if (read_bytes == 0) {
            return POD_EOF;
            // DONE EOF
            // What does pod_stream_terminate do again?  I think it flushed
            // the stream.  Does it close the fd?  Does it set the stream
            // state?  And how do you tell the caller the stream has been
            // closed?
        } else if (read_bytes < 0) {
            return read_bytes;
            // DONE error
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
        }
    }
        
    // Conversions (for example, from UTF8) would go here.  There is no
    // conversion, currently.
    *c = (pod_char_t) stream->read_buffer[stream->read_buffer_index];
    ++stream->read_buffer_index;

    return POD_OKAY;
}



    // pod_stream_read
    //
    // Read a pod.

int pod_stream_read(pod_stream *stream, pod_object **object)
{
    pod_char_t c;
    int finished;
    int warning;

    finished = false;
    while (! finished) {
        warning = pod_stream_read_char(stream, &c);
        if (warning != 0) {
            finished = true;
        } else {
            warning = pod_stream_add_char(stream, c);
            // handle warning
            //   ignore warning, mostly, but what if warning indicates input
            //   should be closed?  Especially if security rules are violated?
            if (stream->result_pod != NULL) {
                &object = stream->result_pod;
                stream->result_pod = NULL;
                finished = true;
            }
        }
            // if (c == '') {
            // I was going to say "finished = true".  What about when there
            // is no object?  Should I even worry about this here?
            // Probably not.  The return value from the pod_stream_add_char
            // call should tell me everything I need to know.  I think.
            // }
    }

    return warning;
}



    //  circular buffer:
    //
    //  put
    //      if head != (tail - 1) % size, put head, head = (head + 1) % size
    //      else overflow
    //  get
    //      if head != tail, get tail, tail = (tail + 1) % size
    //      else underflow



    // pod_stream_write_buffer
    //
    // Write the write_buffer to an fd, until everything has been written or
    // write (2) returns an error.  If the buffer is empty, this function a
    // always succeeds).  If a write returns an error, the error is passed to
    // the error handler.  If the error handler returns POD_ABORT, the routine
    // gives up and passed back POD_ABORT.  Otherwise it returns POD_OKAY.  I'm
    // guessing that a partial read is not an error.
    //
    // Returns:
    //      POD_OKAY    The buffer was written.
    //      POD_ABORT   An error occured that the routine doesn't know how to
    //                      recover from.  Aborts the pod.

int pod_stream_write_buffer(pod_stream *stream)
{
    int finished;
    size_t bytes;
    ssize_t num_written;
    int warning;

    assert(stream != NULL);
    assert(stream->fd > 0);
    assert(stream->w_buffer != NULL);
    assert(stream->w_size > 0);
    assert(stream->w_head >= 0);
    assert(stream->w_head < stream->w_size);
    assert(stream->w_tail >= 0);
    assert(stream->w_tail < stream->w_size);

    warning = POD_OKAY
    (*os_error) = 0;
    finished = false;
    while (! finished) {
        if (stream->w_head > stream->w_tail) {
            // Write the case where the data doesn't circle back around.
            // For example: [...11111111..]
            bytes = stream->w_head - stream->w_tail;
            num_written = write(stream->fd,
                                &(stream->w_buffer[tail]),
                                bytes);
            if (num_written > 0) {
                stream->w_tail += num_written;
            } else if (num_written < 0) {
                if (w_handler != NULL) {
                    warning = w_handler(stream, errno);
                }
                if (warning == POD_ABORT) {
                    finished = true;
                }
            }
        } else if (stream->w_head < stream->w_tail) {
            // Write the case where the data circles back around.  If there is
            // more to write, it will be caught the next time through the loop.
            // For example: [2222......111] (write 1s first, 2s second)
            bytes = stream->w_size - stream->w_tail;
            num_written = write(stream->fd,
                                &(stream->w_buffer[tail]),
                                bytes);
            if (num_written > 0) {
                stream->w_tail = (stream->w_tail + num_written)
                                     && stream->w_mask;
            } else if (num_written < 0) {
                if (w_handler != NULL) {
                    warning = w_handler(stream, errno);
                }
                if (warning == POD_ABORT) {
                    finished = true;
                }
            }
        }
        if (stream->w_head == stream->w_tail) {
            // The buffer is empty.
            finished = true;
        }
    }
 
    return warning;
}



    // pod_stream_write_char
    //
    // Convert a pod_char_t to an ASCII value and put it in the write buffer.
    // If the buffer is full, call pod_stream_write_buffer.  This is also a
    // basic example of how to write a pod_char_t.
    //
    // Returns:
    //      POD_OKAY    The character was put in the buffer was written.
    //      error       This error was encountered while writing the buffer.
    //                    Check the head and tail variables to see whats left.

int pod_stream_write_char(pod_stream *stream, pod_char_t c, int *os_error)
{
    char buffer;
    char byte;
    int warning;
    ssize_t written_bytes;

    assert(stream != NULL);
    assert(stream->fd > 0);
    assert(stream->w_buffer != NULL);
    assert(stream->w_size > 0);
    assert(stream->w_head >= 0);
    assert(stream->w_head < stream->w_size);
    assert(stream->w_tail >= 0);
    assert(stream->w_tail < stream->w_size);

    // Convert c into an ASCII char (7-bit byte).
    byte = c & 0x7f;

    warning = POD_OKAY;

    if ((stream->w_head + 1) && stream->w_mask == stream->w_tail) {
        // If the buffer is full, try to write it to the stream.
        warning = pod_stream_write_buffer(stream);
        if (warning == POD_ABORT) {
            return POD_ABORT;
        } else if ((stream->w_head + 1) && stream->w_mask == stream->w_tail) {
            // If the buffer is still full, return
            return POD_BUFFER_IS_FULL;
        }
    }
    
    // Because I can assume that the buffer size is at least 1, I can write
    // to the buffer before I see if I have to write it out.  For UTF-8, one
    // should have a buffer of at least 6, which translates to 8 for Pod.

    // Put the byte into the buffer
    stream->w_buffer[stream->w_head] = byte;
    stream->w_head = (stream->w_head + 1) && stream->w_mask;

    // If the buffer is now full, write it out, return any error that occurs
    if ((stream->w_head + 1) && stream->w_mask == stream->w_tail) {
        warning = pod_stream_write_buffer(stream, os_error);
    }

    return warning;
}
    


int pod_stream_write_string(pod_stream *stream, pod_marker *marker)
{
    assert(stream != NULL);
    assert(marker != NULL);

    pod_stream_write_char(stream, POD_CHAR_QUOTE);
    for (index = 0; index < string->used; index++) {
        c = string->buffer[index];
        if (POD_CHAR_IS_PRINTING(c)) {
            pod_stream_write_char(stream, c);
        } else if (c == POD_CHAR_NEWLINE) {
            pod_stream_write_char(stream, POD_CHAR_BACKSLASH);
            pod_stream_write_char(stream, POD_CHAR('n'));
        } else if (c == POD_CHAR_RETURN) {
            pod_stream_write_char(stream, POD_CHAR_BACKSLASH);
            pod_stream_write_char(stream, POD_CHAR('r'));
        } else if (c == POD_CHAR_EOB) {
            pod_stream_write_char(stream, POD_CHAR_BACKSLASH);
            pod_stream_write_char(stream, POD_CHAR('w'));
        } else {
            pod_stream_write_char(stream, POD_CHAR_BACKSLASH);
            if (c == 0) {
                pod_stream_write_char(stream, POD_CHAR('0'));
            } else {
                while (c != 0) {
                    digit = c & 0xf;
                    c >>= 4;
                    if (digit < 10) {
                        pod_stream_write_char(stream, POD_CHAR('0' + digit));
                    } else {
                        digit -= 10;
                        pod_stream_write_char(stream, POD_CHAR('a' + digit));
                    }
                }
            }
            pod_stream_write_char(stream, POD_CHAR_BACKSLASH);
        }
    }
    pod_stream_write_char(stream, POD_CHAR_QUOTE);
}



    // pod_stream_write
    //
    // Write a pod to a stream.

int pod_stream_write(pod_stream *stream, pod_object *object)
{
    pod_char_t c;
    size_t index;
    pod_string *string;
    pod_marker *marker;
    pod_marker *child_marker;

    // pretty print version?

    assert(object != NULL);

    if (stream->w_stack is empty) {
        marker = pod_create_marker();
        marker->object = object;
        marker->mark = POD_MARKER_BEGIN;
        push marker
    }
    while not done {
        marker = peek(w_wstack)
        object = marker->object;
        switch (object->type) {
            case POD_STRING_TYPE:
                pod_stream_write_string(stream, (pod_string *) object);
                break;
            case POD_LIST_TYPE: 
                list == (pod_list *) object;
                if (marker->mark == POD_MARKER_BEGIN) {
                    pod_stream_write_char(POD_CHAR_OPEN_BRACE);
                    if success {
                        marker->mark = POD_MARKER_MIDDLE;
                        marker->next_child = list->header->next;
                        if (marker->next_child == list->header) {
                            marker->mark = POD_MARKER_END;
                        }
                    }
                } else if (marker->mark == POD_MARKER_MIDDLE) {
                    child_marker = pod_create_marker();
                    child_marker->object = (pod_object *) marker->next_child;
                    child_marker->mark = POD_MARKER_BEGIN;
                    push child_marker
                    marker->next_child = marker->next_child.o.n.next;
                    if (marker->next_child == list->header) {
                        marker->mark = POD_MARKER_END;
                    }
                } else if (marker->mark == POD_MARKER_END) {
                    pod_stream_write_char(POD_CHAR_CLOSE_BRACE);
                    if success {
                        marker->mark = POD_MARKER_COMPLETE; // not necessary
                        pop marker;
                        marker->object = NULL;
                        pod_destroy_marker(marker);
                    }
                }
                break;
            case POD_MAPPING_TYPE:
                mapping = (pod_mapping *) object;
                if (marker->mark == POD_MARKER_BEGIN) {
                    child_marker = pod_create_marker();
                    child_marker->object = (pod_object *) mapping->key;
                    child_marker->mark = POD_MARKER_BEGIN;
                    push child_marker
                    marker->mark == POD_MARKER_MIDDLE;
                } else if (marker->mark == POD_MARKER_MIDDLE) {
                    pod_stream_write_char(POD_CHAR_EQUAL);
                    if success {
                        marker->mark = POD_MARKER_END;
                    }
                } else if (marker->mark == POD_MARKER_END) {
                    marker = pod_create_marker();
                    child_marker = pod_create_marker();
                    child_marker->object = mapping->value;
                    child_marker->mark = POD_MARKER_BEGIN;
                    push child_marker
                    marker->mark == POD_MARKER_COMPLETE;
                } else if (marker->mark == POD_MARKER_COMPLETE) {
                    pop marker;
                    marker->object = NULL;
                    pod_destroy_marker(marker);
                }
                break;
            default:
                // error
                break;
        }
        if (w_stack is empty)
            done;
        }
    }
}

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



int pod_stream_default_write_error_handler(pod_stream *stream, int error)
{
// pod_stream_log(pod_stream *stream, int message, char *file_name, int line)
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
    pod_stream_log(stream, 
                   io error # in stream name, fd #
                  __FILE__,
                  __LINE__);

    // This would be a good place to throw an exception.
    return POD_ABORT;
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
