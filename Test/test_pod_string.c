#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "../pod_string.h"



#define STRING_SIZE 56



volatile int tmp;
char *test_string = "Mary had a little lamb.";



    // Test pod_string and its associated functions.

int main(int argc, char *argv)
{
    int error_count;

    error_count = 0;
    if (POD_STRING_TYPE != 0xb0001) {
        error_count++;
        printf("    POD_STRING_TYPE is not the expected value.\n");
        printf("        (expected 0xb0001, got 0x%x.)\n", POD_STRING_TYPE);
    }
    error_count += test_create_and_destroy();
    error_count += test_copy_string();

    return error_count;
}



    // test_create_string_error
    //
    // Print an error message about a failure to create a pod_string, printing
    // out the label string first to let whoever know about where the error
    // actuall occurred.

int test_create_string_error(char *label, struct pod_string *string)
{
    int error_count;
    char *estr;

    error_count = 1;
    estr = strerror(errno);
    printf("%sCouldn't create pod_string: %s (%d).\n", label, estr, errno);
    if (string != NULL) {
        error_count++;
        printf("%s    and pod_create_string didn't return NULL.\n", label);
    }

    return error_count;
}



    // test_create_and_destroy
    //
    // Test the functions pod_create_string and pod_destroy_string.  Check to
    // see that struct members are initialized to correct values.

int test_create_and_destroy(void)
{
    int error;
    int error_count;
    char *estr;
    int i;
    struct pod_string *string;

    printf("    test_create_and_destroy\n");
    error_count = 0;
        // STRING_SIZE elements, no flags
    string = pod_create_string(&error, STRING_SIZE, 0);
    if (error != 0) {
        return test_create_string_error("        ", string);
    }

    if (string->o.type != POD_STRING_TYPE) {
        error_count++;
        printf("    String's type is not POD_STRING_TYPE.\n");
    }
    if (string->o.next != NULL) {
        error_count++;
        printf("    String's o.next value is not NULL.\n");
    }
    if (string->o.previous != NULL) {
        error_count++;
        printf("    String's o.previous value is not NULL.\n");
    }
    if (string->o.destroy != pod_destroy_string) {
        error_count++;
        printf("    String's o.destroy is not set to pod_destroy_string.\n");
    }
    if (string->size != STRING_SIZE) {
        error_count++;
        printf("    String's size is not STRING_SIZE (%d).\n", STRING_SIZE);
    }
    if (string->used != 0) {
        error_count++;
        printf("    String's used count is not 0, it is %lu.\n", string->used);
    }
    if (string->flags != 0) {
        error_count++;
        printf("    String's flags is not 0, it's %d.\n", string->flags);
    }
    if (error_count != 0) {
        return error_count;
    }

    fflush(stdout);
    for (i = 0; i < STRING_SIZE; i++) {
        // Some sort of malloc tester needs to be used to see if reading or
        //   writing doesn't work.  Is this actually happening or is this being
        //   "optimized" out?
        tmp = string->buffer[i];
        string->buffer[i] = 0;
        string->buffer[i] = 10;
    }
    string->o.destroy(string);

    return error_count;
}



    // test_copy_string
    //
    // Test the copy functions in pod_string.h

int test_copy_string(void)
{
    char buffer[100];
    int i;
    size_t size;
    struct pod_string *a_string;
    struct pod_string *b_string;
    int error;
    int error_count;

    printf("    test_copy_string\n");
    error_count = 0;
    size = strlen(test_string);
    for (i = 0; i < 100; i++) {
        buffer[i] = 0xa5;  // a5 is 10100101
    }
    a_string = pod_create_string(&error, size, 0);
    if (error != 0) {
        return test_create_string_error("        (1) ", a_string);
    }

    fflush(stdout);
    pod_copy_string_from_cstring(a_string, test_string);
    pod_copy_string_to_cstring(buffer, a_string);
    buffer[99] = '\0';
    if (strncmp(test_string, buffer, size) != 0) {
        error_count++;
        printf("        (1) test_string (%s) and buffer (%s) differ.\n", test_string, buffer);
    }
    for (i = 0; i < 100; i++) {
        buffer[i] = 0xa5;  // a5 is 10100101
    }
    b_string = pod_create_string(&error, size, 0);
    if (error != 0) {
        return test_create_string_error("        (2) ", b_string);
    }

    fflush(stdout);
    pod_copy_string(b_string, a_string);
    pod_copy_string_to_cstring(buffer, b_string);
    buffer[99] = '\0';
    if (strncmp(test_string, buffer, size) != 0) {
        error_count++;
        printf("        (2) test_string (%s) and buffer (%s) differ.\n", test_string, buffer);
    }
    b_string->o.destroy(b_string);
    a_string->o.destroy(a_string);

    return error_count;
}
