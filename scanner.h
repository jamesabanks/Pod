#ifndef INCLUDE_POD_SCANNER_H
#define INCLUDE_POD_SCANNER_H



enum boolean {
    false = 0,
    true = 1
};



enum stream_state {
    stream_state_mask = 0xe,        /* binary 1110 */
    stream_escape_mask = 0x1,       /* binary 0001 */
    stream_start = 0,
    stream_simple = 0x2,            /* binary 0010 */
    stream_simple_escape = 0x3,     /* binary 0011 */
    stream_quoted = 0x4,            /* binary 0100 */
    stream_quoted_escape = 0x5,     /* binary 0101 */
    stream_blurb = 0x6              /* binary 0110 */
};



enum stream_token {
    token_string,
    token_begin_map,
    token_equals,
    token_end_map,
    token_begin_blurb,
    token_end_blurb,
    token_begin_list,
    token_end_list,
    token_pod_sync
}



extern int scan_escape(
    pod_stream *stream,
    pod_char_t c,
    int *replace,
    pod_char_t *r,
    int *finished
);
extern int scan_start(
    pod_stream *stream,
    pod_char_t c,
    pod_object **object,
    int *next_state
);
extern int scan_string(
    pod_stream *stream,
    pod_char_t c,
    pod_object **object,
    int *next_state
);
extern int scan_quoted(
    pod_stream *stream,
    pod_char_t c,
    pod_object **object,
    int *next_state
);
extern int lexer_handle_blurb(
    pod_stream *stream,
    pod_char_t c,
    int *next_state
);


extern int add_token(pod_stream *stream, int token, pod_object **object);


#endif /* INCLUDE_POD_SCANNER_H */
