#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include "pod_boolean.h"
#include "pod_char.h"
#include "pod_log.h"
#include "pod_marker.h"
#include "pod_stream.h"



    //  circular buffer:
    //
    //  put
    //      if head != (tail - 1) % size, put head, head = (head + 1) % size
    //      else overflow
    //  get
    //      if head != tail, get tail, tail = (tail + 1) % size
    //      else underflow



    // pod_stream_write_reset
    //
    // Reset the write stuff in the stream.  The buffer reset to zero/empty.
    // The write stack is emptied, the markers are destroyed.  Finally, the
    // pointer to the current object being written is reset to zero.

void pod_stream_write_reset(pod_stream *stream) 
{
    pod_object *object;

    stream->w_head = stream->w_tail = 0;
    object = pod_list_pop(stream->w_stack);
    while (object != NULL) {
        assert(object->type == POD_MARKER_TYPE);
        object->destroy(object);
        object = pod_list_pop(stream->w_stack);
    }
    stream->w_object = NULL;
}


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

int pod_stream_write_buffer(pod_stream *stream, int *os_err)
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

    warning = POD_OKAY;
    (*os_err = 0);
    finished = false;
    while (! finished) {
        if (stream->w_head > stream->w_tail) {
            // Write the case where the data doesn't circle back around.
            // For example: [...11111111..]
            bytes = stream->w_head - stream->w_tail;
            num_written = write(stream->fd,
                                &(stream->w_buffer[stream->w_tail]),
                                bytes);
            if (num_written > 0) {
                stream->w_tail += num_written;
            } else if (num_written < 0) {
                (*os_err) = errno;
                warning = POD_OS_ERROR;
                finished = true;
            }
        } else if (stream->w_head < stream->w_tail) {
            // Write the case where the data circles back around.  If there is
            // more to write, it will be caught the next time through the loop.
            // For example: [2222......111] (write 1s first, 2s second)
            bytes = stream->w_size - stream->w_tail;
            num_written = write(stream->fd,
                                &(stream->w_buffer[stream->w_tail]),
                                bytes);
            if (num_written > 0) {
                stream->w_tail = (stream->w_tail + num_written)
                                     && stream->w_mask;
            } else if (num_written < 0) {
                (*os_err) = errno;
                warning = POD_OS_ERROR;
                finished = true;
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

int pod_stream_write_char(pod_stream *stream, pod_char_t c, int *os_err)
{
    char byte;
    int warning;

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
        warning = pod_stream_write_buffer(stream, os_err);
        if (warning == POD_OS_ERROR) {
            return POD_OS_ERROR;
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
        warning = pod_stream_write_buffer(stream, os_err);
    }

    return warning;
}
    


int pod_stream_write_string(pod_stream *stream, pod_marker *marker, int *os_err)
{
    pod_char_t c;
    int digit;
    pod_string *string;
    pod_marker *temp;
    int warning;

    assert(stream != NULL);
    assert(marker != NULL);
    assert(os_err != NULL);

    string = (pod_string *) marker->object;
    switch (marker->state) {
        case POD_MARKER_BEGIN:
            warning = pod_stream_write_char(stream, POD_CHAR('"'), os_err);
            if (warning == POD_OKAY) {
                if (string->used > 0) {
                    marker->state = POD_MARKER_MIDDLE;
                    marker->index = 0;
                } else {
                    marker->state = POD_MARKER_END;
                }
            }
            break;
        case POD_MARKER_MIDDLE:
            c = string->buffer[marker->index];
            if ((! POD_CHAR_IS_PRINTING(c)) || c == POD_CHAR('"')) {
                warning = pod_stream_write_char(stream, POD_CHAR('\\'), os_err);
                if (warning == POD_OKAY) {
                    marker->escape = c;
                    if (c == POD_CHAR_NEWLINE || c == POD_CHAR_RETURN
                        || c == POD_CHAR_EOB || c == POD_CHAR('"')) {
                        marker->state = POD_MARKER_ESCAPE;
                    } else if (c > 0) {
                        marker->state = POD_MARKER_CHAR;
                    } else {
                        marker->state = POD_MARKER_NULL;
                    }
                }
            } else {
                warning = pod_stream_write_char(stream, c, os_err);
                if (warning == POD_OKAY) {
                    marker->index++;
                    if (marker->index == string->used) {
                        marker->state = POD_MARKER_END;
                    }
                }
            }
            break;
        case POD_MARKER_ESCAPE:
            warning = pod_stream_write_char(stream, marker->escape, os_err);
            if (warning == POD_OKAY) {
                marker->index++;
                if (marker->index == string->used) {
                    marker->state = POD_MARKER_END;
                } else {
                    marker->state = POD_MARKER_MIDDLE;
                }
            }
            break;
        case POD_MARKER_CHAR:
            digit = marker->escape & 0xf;
            if (digit < 10) {
                warning = pod_stream_write_char(stream,
                                                POD_CHAR('0' + digit),
                                                os_err);
            } else {
                digit -= 10;
                warning = pod_stream_write_char(stream,
                                                POD_CHAR('a' + digit),
                                                os_err);
            }
            if (warning == POD_OKAY) {
                marker->escape >>= 4;
                if (marker->escape == 0) {
                    marker->state = POD_MARKER_FINAL;
                }
            }
            break;
        case POD_MARKER_NULL:
            warning = pod_stream_write_char(stream, POD_CHAR('0'), os_err);
            if (warning == POD_OKAY) {
                marker->state = POD_MARKER_FINAL;
            }
            break;
        case POD_MARKER_FINAL:
            warning = pod_stream_write_char(stream, POD_CHAR('\\'), os_err);
            if (warning == POD_OKAY) {
                marker->index++;
                if (marker->index == string->used) {
                    marker->state = POD_MARKER_END;
                } else {
                    marker->state = POD_MARKER_MIDDLE;
                }
            }
            break;
        case POD_MARKER_END:
            warning = pod_stream_write_char(stream, POD_CHAR('"'), os_err);
            if (warning == POD_OKAY) {
                temp = (pod_marker *) pod_list_pop(stream->w_stack);
                assert(temp == marker);
                temp->object = NULL;
                assert(temp->next_child == NULL);
                pod_marker_destroy(temp);
            }
            break;
        default:
            // error
            assert(0);
            break;
    }

    return warning;
}



int pod_stream_write_list(pod_stream *stream, pod_marker *marker, int *os_err)
{
    pod_list *list;
    pod_marker *temp;
    int warning;

    assert(stream != NULL);
    assert(marker != NULL);
    assert(os_err != NULL);

    list = (pod_list *) marker->object;
    switch (marker->state) {
        case POD_MARKER_BEGIN:
            warning = pod_stream_write_char(stream, POD_CHAR('{'), os_err);
            if (warning == POD_OKAY) {
                marker->next_child = (pod_object *) list->header.next;
                if (marker->next_child != (pod_object *) &(list->header)) {
                    marker->state = POD_MARKER_MIDDLE;
                } else {
                    marker->state = POD_MARKER_END;
                }
            }
            break;
        case POD_MARKER_MIDDLE:
            temp = pod_marker_create();
            temp->object = (pod_object *) marker->next_child;
            temp->state = POD_MARKER_BEGIN;
            pod_list_push(stream->w_stack, (pod_object *) temp);
            marker->next_child = (pod_object *) marker->next_child->n.next;
            if (marker->next_child == (pod_object *) &(list->header)) {
                marker->state = POD_MARKER_END;
            }
        case POD_MARKER_END:
            warning = pod_stream_write_char(stream, POD_CHAR('}'), os_err);
            if (warning == POD_OKAY) {
                temp = (pod_marker *) pod_list_pop(stream->w_stack);
                assert(temp == marker);
                temp->object = NULL;
                temp->next_child = NULL;
                pod_marker_destroy(temp);
            }
            break;
        default:
            // error
            assert(0);
            break;
    }

    return warning;
}



int pod_stream_write_mapping(pod_stream *stream, pod_marker *marker,int *os_err)
{
    pod_mapping *mapping;
    pod_marker *temp;
    int warning;

    assert(stream != NULL);
    assert(marker != NULL);
    assert(os_err != NULL);

    warning = POD_OKAY;
    mapping = (pod_mapping *) marker->object;
    switch (marker->state) {
        case POD_MARKER_BEGIN:
            temp = pod_marker_create();
            temp->object = (pod_object *) mapping->key;
            temp->state = POD_MARKER_BEGIN;
            pod_list_push(stream->w_stack, (pod_object *) temp);
            marker->state = POD_MARKER_MIDDLE;
            break;
        case POD_MARKER_MIDDLE:
            warning = pod_stream_write_char(stream, POD_CHAR('='), os_err);
            if (warning == POD_OKAY) {
                marker->state = POD_MARKER_END;
            }
            break;
        case POD_MARKER_END:
            temp = pod_marker_create();
            temp->object = mapping->value;
            temp->state = POD_MARKER_BEGIN;
            pod_list_push(stream->w_stack, (pod_object *) temp);
            marker->state = POD_MARKER_COMPLETE;
            break;
        case POD_MARKER_COMPLETE:
            temp = (pod_marker *) pod_list_pop(stream->w_stack);
            assert(marker == temp);
            temp->object = NULL;
            assert(temp->next_child == NULL);
            pod_marker_destroy(temp);
            break;
        default:
            // error
            assert(0);
            break;
    }

    return warning;
}



    // pod_stream_write
    //
    // Write a pod to a stream.

int pod_stream_write(pod_stream *stream, pod_object *object, int *os_err)
{
    int finished;
    pod_marker *marker;
    int warning;

    // TODO pretty print version
    // TODO reset if starting a different pod
    if (object != stream->w_object) {
        pod_stream_write_reset(stream);
        stream->w_object = object;
    }

    assert(stream != NULL);
    assert(os_err != NULL);

    if (object == NULL) {
        return POD_OKAY;
    }

    if (pod_list_is_empty(stream->w_stack)) {
        marker = pod_marker_create();
        marker->object = object;
        marker->state = POD_MARKER_BEGIN;
        pod_list_push(stream->w_stack, (pod_object *) marker);
    }
    finished = false;
    warning = POD_OKAY;
    while (! finished) {
        marker = (pod_marker *) pod_list_peek(stream->w_stack);
        switch (object->type) {
            case POD_STRING_TYPE:
                warning = pod_stream_write_string(stream, marker, os_err);
                break;
            case POD_LIST_TYPE: 
                warning = pod_stream_write_list(stream, marker, os_err);
                break;
            case POD_MAPPING_TYPE:
                warning = pod_stream_write_mapping(stream, marker, os_err);
                break;
            default:
                // error
                assert(0);
                break;
        }
        if (pod_list_is_empty(stream->w_stack) || warning != POD_OKAY) {
            finished = true;
        }
    }

    return warning;
}
