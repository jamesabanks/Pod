#ifndef INCLUDE_POD_LOG_H
#define INCLUDE_POD_LOG_H

/******************************************************************** 
 *  pod_log.h
 *  Copyright (c) 2011-2012, James A. Banks
 *  All rights reserved.
 *  See file LICENSE for details.
 ********************************************************************/




typedef enum {
    POD_OKAY = 0,
    POD_OS_ERROR,
    POD_ABORT,
    POD_EOF,
    POD_BUFFER_IS_FULL,

    // pod_stream_add_char
    POD_TOO_MANY_WARNINGS = 10,
    POD_INVALID_STREAM_STATE,

    // pod_stream_add_token
    POD_EQUALS_NOT_PRECEDED_BY_KEY = 20,
    POD_UNMATCHED_END_MAP,
    POD_UNMATCHED_END_LIST,
    POD_UNKNOWN_TOKEN,

    // pod_stream_merge_down
    POD_EQUALS_AT_END = 30,
    POD_TWO_MAPPINGS,
    POD_INVALID_INSERT_INTO_MAP,
    POD_INVALID_INSERT_INTO_STRING,
} pod_warnings;



#endif /* INCLUDE_POD_LOG_H */
