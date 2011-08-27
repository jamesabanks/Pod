#ifndef INCLUDE_POD_LEXER_H
#define INCLUDE_POD_LEXER_H



enum stream_state {
    stream_state_mask = 0xe,        /* binary 1110 */
    stream_escape_mask = 0x1,       /* binary 0001 */
    stream_start = 0,
    stream_simple = 0x2,            /* binary 0010 */
    stream_simple_escape = 0x3,     /* binary 0011 */
    stream_quoted = 0x4,            /* binary 0100 */
    stream_quoted_escape = 0x5,     /* binary 0101 */
    stream_blurb = 0x6              /* binary 0110 */
}


extern int lexer_handle_escape(
    pod_char_t c,
    int *replace,
    pod_char_t *r,
    int *finished
);
extern int scan_start(pod_stream *stream, pod_char_t c, int *next_state);
extern int scan_string(pod_stream *stream, pod_char_t c, int *next_state);
extern int lexer_handle_quoted(
    pod_stream *stream,
    pod_char_t c,
    int *next_state
);
extern int lexer_handle_blurb(
    pod_stream *stream,
    pod_char_t c,
    int *next_state
);



#endif /* INCLUDE_POD_LEXER_H */
