#ifndef INCLUDE_POD_STREAM_TOKEN_H
#define INCLUDE_POD_STREAM_TOKEN_H



typedef enum pod_stream_token {
    pod_token_string,
    pod_token_begin_map,
    pod_token_equals,
    pod_token_end_map,
    pod_token_begin_blurb,
    pod_token_end_blurb,
    pod_token_begin_list,
    pod_token_end_list,
    pod_token_pod_sync
} pod_stream_token;




#endif /* INCLUDE_POD_STREAM_TOKEN_H */
