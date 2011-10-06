#include <assert.h>
#include <unistd.h>
#include "pod_boolean.h"
#include "pod_log.h"
#include "pod_stream.h"



    // pod_stream_read_char
    //
    // Read a byte from a file descriptor and convert it to a pod_char_t.
    //
    // Returns:
    //      POD_OKAY        Successfully got a character
    //      POD_OS_ERROR    An I/O error, stored in os_err
    //      POD_EOF         At end of file, no character

int pod_stream_read_char(pod_stream *stream, pod_char_t *c, int *os_err)
{
    ssize_t read_bytes;

    assert(stream->fd > -1);
    assert(stream->r_buffer != NULL);
    assert(stream->r_size > 0);
    assert(stream->r_head >= stream->r_tail);

    if (stream->r_tail == stream->r_head) {
        // If the buffer is empty, do a read to put something in it
        read_bytes = read(stream->fd,
                          stream->r_buffer,
                          stream->r_size);
        if (read_bytes == 0) {
            return POD_EOF;
            // DONE EOF
            // What does pod_stream_terminate do again?  I think it flushed
            // the stream.  Does it close the fd?  Does it set the stream
            // state?  And how do you tell the caller the stream has been
            // closed?
        } else if (read_bytes < 0) {
            (*os_err) = read_bytes;
            return POD_OS_ERROR;
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
            stream->r_head = read_bytes;
            stream->r_tail = 0;
        }
    }
        
    // Conversions (for example, from UTF8) would go here.  There is no
    // conversion, currently.
    (*c) = (pod_char_t) stream->r_buffer[stream->r_tail];
    ++stream->r_tail;
    if (stream->r_tail == stream->r_head) {
        stream->r_tail = stream->r_head = 0;
    }

    return POD_OKAY;
}



    // pod_stream_read
    //
    // Read a pod.

int pod_stream_read(pod_stream *stream, pod_object **object, int *os_err)
{
    pod_char_t c;
    int finished;
    int warning;

    assert(stream != NULL);
    assert(object != NULL);
    assert(os_err != NULL);

    finished = false;
    while (! finished) {
        warning = pod_stream_read_char(stream, &c, os_err);
        if (warning != 0) {
            finished = true;
        } else {
            warning = pod_stream_add_char(stream, c);
            // TODO handle warning
            //   ignore warning, mostly?, but what if warning indicates input
            //   should be closed?  Especially if security rules are violated?
            if (stream->result_pod != NULL) {
                (*object) = stream->result_pod;
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

