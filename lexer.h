#ifndef INCLUDE_POD_LEXER_H
#define INCLUDE_POD_LEXER_H



enum lexer_state {
    lexer_state_mask = 0xe,         /* binary 1110 */
    lexer_escape_mask = 0x1,        /* binary 0001 */
    lexer_initial = 0,
    lexer_string = 0x2,             /* binary 0010 */
    lexer_string_escape = 0x3,      /* binary 0011 */
    lexer_quoted = 0x4,             /* binary 0100 */
    lexer_quoted_escape = 0x5,      /* binary 0101 */
    lexer_blurb = 0x6               /* binary 0110 */
}


extern int lexer_handle_escape(
    pod_char_t c,
    int *replace,
    pod_char_t *r,
    int *finished
);
extern int lexer_handle_initial(
    pod_stream *stream,
    pod_char_t c,
    int *next_state
);
extern int lexer_handle_string(
    pod_stream *stream,
    pod_char_t c,
    int *next_state
);
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
