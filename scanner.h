#ifndef INCLUDE_POD_SCANNER_H
#define INCLUDE_POD_SCANNER_H



enum boolean {
    false = 0,
    true = 1
};



enum stream_state {
    stream_state_mask = 0xf0,           /* binary  1111 0000 */
    stream_escape_mask = 0x0f,          /* binary  0000 1111 */
    stream_escape_hex = 0x0c,
    stream_start = 0,
    stream_simple = 0x10,               /* binary  0001 0000 */
    stream_simple_escape = 0x18,        /* binary  0001 1000 */
    stream_simple_escape_hex = 0x1c,    /* binary  0001 1100 */
    stream_quoted = 0x20,               /* binary  0010 0000 */
    stream_quoted_escape = 0x28,        /* binary  0010 1000 */
    stream_quoted_escape_hex = 0x2c,    /* binary  0010 1100 */
    stream_blurb = 0x30                 /* binary  0011 0000 */
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
