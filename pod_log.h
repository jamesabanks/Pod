#ifndef INCLUDE_POD_LOG_H
#define INCLUDE_POD_LOG_H



typedef enum pod_warnings {
    POD_OKAY = 0,

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
