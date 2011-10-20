#include "pod_stream.h"
#include "pod_scan.h"
#include "pod_boolean.h"
#include "pod_log.h"



void pod_stream_log(pod_stream *stream, int message, char *file_name, int line)
{
    // Example:
    //
    // Pod message 127: Got illegal character ')'
    //   stream: std_out
    //   at character xxxx (line xxx, column xx)
    //   message from pod_stream.c, line xxx
}
