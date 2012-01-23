#include <stdio.h>
#include <stdlib.h>
#include "pod_char.h"



// These are the ASCII values of the characters Pod cares about.  They will be
// tested against the values given by the compiler to the appropriate chars.
// For example, '{' should produce OPEN_BRACE.

enum pod_chars {
    NEWLINE = 0x0a,
    RETURN = 0x0d,
    END_OF_POD = 0x17,
    QUOTE = 0x22,
    OPEN_ARROW = 0x3c,
    EQUALS = 0x3d,
    CLOSE_ARROW = 0x3e,
    OPEN_SQUARE = 0x5b,
    BACKSLASH = 0x5c,
    CLOSE_SQUARE = 0x5d,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};


 
// This program performs tests to make sure that whatever type pod_char_t is,
// this type can represent ASCII.  These tests really are basic.  So much so
// that this program serves more to test "test" than to test "pod_char.h".

int main(int argc, char *argv[])
{
    int error_count;
    int i;
    pod_char_t c;

    error_count = 0;
    printf("\n    testing pod_char_t\n");
    for (i = 0; i < 128; i++) {
        c = i;
        if (c != i) {
            error_count++;
            fprintf(stdout, "    Assignment test failed: %d\n", i);
        }
    }
    c = '\n';
    if (c != NEWLINE) {
        error_count++;
        fprintf(stdout, "    Newline (\\n) test failed\n");
    }
    c = '\r';
    if (c != RETURN) {
        error_count++;
        fprintf(stdout, "    Return (\\r) test failed\n");
    }
    c = '"';
    if (c != QUOTE) {
        error_count++;
        fprintf(stdout, "    Quote (\") test failed\n");
    }
    c = '<';
    if (c != OPEN_ARROW) {
        error_count++;
        fprintf(stdout, "    Open arrow (<) test failed\n");
    }
    c = '=';
    if (c != EQUALS) {
        error_count++;
        fprintf(stdout, "    Equals (=) test failed\n");
    }
    c = '>';
    if (c != CLOSE_ARROW) {
        error_count++;
        fprintf(stdout, "    Close arrow (>) test failed\n");
    }
    c = '[';
    if (c != OPEN_SQUARE) {
        error_count++;
        fprintf(stdout, "    Open square bracket ([) test failed\n");
    }
    c = '\\';
    if (c != BACKSLASH) {
        error_count++;
        fprintf(stdout, "    Backslash (\\) test failed\n");
    }
    c = ']';
    if (c != CLOSE_SQUARE) {
        error_count++;
        fprintf(stdout, "    Close square bracket (]) test failed\n");
    }
    c = '{';
    if (c != OPEN_BRACE) {
        error_count++;
        fprintf(stdout, "    Open brace ({) test failed\n");
    }
    c = '}';
    if (c != CLOSE_BRACE) {
        error_count++;
        fprintf(stdout, "    Close brace (}) test failed\n");
    }
    if (error_count == 1) {
        fprintf(stdout, "    1 error\n");
    } else if (error_count > 1) {
        fprintf(stdout, "    %d errors\n", error_count);
    }
    error_count += test_pod_char_copy();

    return error_count;
}



    // test_pod_char_copy
    //
    // Test pod_char_copy by copying an array of pod_char_t containing 0-127.
    // Since I don't guarantee anything beyond that, I'll leave it at that.

int test_pod_char_copy(void)
{
    int i;
    int error_count;
    pod_char_t *source;
    pod_char_t *target;

    error_count = 0;
    printf("    test_pod_char_copy\n");
    source = malloc(128 * sizeof(pod_char_t));
    target = malloc(128 * sizeof(pod_char_t));
    for (i = 0; i < 128; i++) {
        source[i] = i;
        target[i] = 0;
    }
    pod_char_copy(target, source, 128);
    for (i = 0; i < 128; i++) {
        if (target[i] != i) {
            error_count++;
        }
    }
    free(target);
    free(source);

    return error_count;
}
