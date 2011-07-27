#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "../pod_string.h"



#define STRING_SIZE 100



volatile int tmp;
char *test_string = "Mary had a little lamb.";
char *test_string2 = "Baa, baa, black sheep.";
char *test_string3 = "Peter peter pumpkin eater.";
pod_char_t test_char = 0x6e; /* 'n' */



    // Test pod_string and its associated functions.

int main(int argc, char *argv)
{
    int error_count;

    error_count = 0;
    printf("\n    testing pod_string\n");
    if (POD_STRING_TYPE != 0x61) {
        error_count++;
        printf("    POD_STRING_TYPE is not the expected value.\n");
        printf("        (expected 0x61, got 0x%x.)\n", POD_STRING_TYPE);
    }
    error_count += test_create_and_destroy();
    error_count += test_copy_string();
    error_count += test_compare_string();
    error_count += test_dup();
    error_count += test_append();

    return error_count;
}



    // test_create_string_error
    //
    // Print an error message about a failure to create a pod_string, printing
    // out the label string first to let whoever know about where the error
    // actuall occurred.

int test_create_string_error(char *label)
{
    int error_count;
    char *estr;

    error_count = 1;
    estr = strerror(errno);
    printf("%sCouldn't create pod_string: %s (%d).\n", label, estr, errno);
    fflush(stdout);

    return error_count;
}



    // test_print_string
    //
    // Quick and dirty printing of the contents of a pod_string.  This function
    // assumes that each character is less than 256.

void test_print_string(FILE *out, struct pod_string *string)
{
    int i;

    for (i = 0; i < string->used; i++) {
        fputc(string->buffer[i], out);
    }
}



    // test_create_and_destroy
    //
    // Test the functions pod_create_string and pod_destroy_string.  Check to
    // see that struct members are initialized to correct values.

int test_create_and_destroy(void)
{
    int error_count;
    int i;
    struct pod_string *string;

    printf("    test_create_and_destroy\n");
    error_count = 0;
        // STRING_SIZE elements, no flags
    string = pod_string_create(STRING_SIZE, 0);
    if (string == NULL) {
        return test_create_string_error("        ");
    }

    if (string->o.type != POD_STRING_TYPE) {
        error_count++;
        printf("        String's type is not POD_STRING_TYPE.\n");
    }
    if (string->o.n.previous != &string->o.n) {
        error_count++;
        printf("        String's o.n.previous value is not the node's address.\n");
    }
    if (string->o.n.next != &string->o.n) {
        error_count++;
        printf("        String's o.n.next value is not the node's address.\n");
    }
    if (string->o.destroy != pod_string_destroy) {
        error_count++;
        printf("        String's o.destroy is not set to pod_destroy_string.\n");
    }
    if (string->size != STRING_SIZE) {
        error_count++;
        printf("        String's size is not STRING_SIZE (%d).\n", STRING_SIZE);
    }
    if (string->used != 0) {
        error_count++;
        printf("        String's used count is not 0, it is %u.\n", string->used);
    }
    if (string->flags != 0) {
        error_count++;
        printf("        String's flags is not 0, it's %d.\n", string->flags);
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
    int error_count;

    printf("    test_copy_string\n");
    error_count = 0;
    size = strlen(test_string);
    for (i = 0; i < 100; i++) {
        buffer[i] = 0xa5;  // a5 is 10100101
    }
    a_string = pod_string_create(size, 0);
    if (a_string == NULL) {
        return test_create_string_error("        a_string: ");
    }

    pod_string_copy_from_cstring(a_string, test_string);
    pod_string_copy_to_cstring(buffer, a_string);
    buffer[99] = '\0';
    if (strncmp(test_string, buffer, size) != 0) {
        error_count++;
        printf("        (1) test_string (%s) and buffer (%s) differ.\n", test_string, buffer);
    }
    for (i = 0; i < 100; i++) {
        buffer[i] = 0xa5;  // a5 is 10100101
    }
    b_string = pod_string_create(size, 0);
    if (b_string == NULL) {
        a_string->o.destroy(a_string);
        return test_create_string_error("        b_string: ");
    }

    pod_string_copy(b_string, a_string);
    pod_string_copy_to_cstring(buffer, b_string);
    buffer[99] = '\0';
    if (strncmp(test_string, buffer, size) != 0) {
        error_count++;
        printf("        (2) test_string (%s) and buffer (%s) differ.\n", test_string, buffer);
    }
    b_string->o.destroy(b_string);
    a_string->o.destroy(a_string);

    fflush(stdout);
    return error_count;
}



int test_compare_string(void)
{
    int i;
    int error_count;
    int result;
    size_t size1;
    size_t size2;
    size_t size3;
    struct pod_string *a_string;
    struct pod_string *b_string;
    struct pod_string *c_string;
    struct pod_string *d_string;

    printf("    test_compare_string\n");
    error_count = 0;
    size1 = strlen(test_string);
    size2 = strlen(test_string2);
    size3 = strlen(test_string3); 
    a_string = pod_string_create(size1, 0);
    if (a_string == NULL) {
        return test_create_string_error("        a_string: ");
    }
    b_string = pod_string_create(size2, 0);
    if (b_string == NULL) {
        a_string->o.destroy(a_string);
        return test_create_string_error("        b_string: ");
    }
    c_string = pod_string_create(size3, 0);
    if (c_string == NULL) {
        a_string->o.destroy(a_string);
        b_string->o.destroy(a_string);
        return test_create_string_error("        c_string: ");
    }
    d_string = pod_string_create(size1, 0);
    if (d_string == NULL) {
        a_string->o.destroy(a_string);
        b_string->o.destroy(a_string);
        c_string->o.destroy(a_string);
        return test_create_string_error("        d_string: ");
    }
    pod_string_copy_from_cstring(a_string, test_string);
    pod_string_copy_from_cstring(b_string, test_string2);
    pod_string_copy_from_cstring(c_string, test_string3);
    pod_string_copy_from_cstring(d_string, test_string);
    result = pod_string_compare(b_string, a_string);
    if (result >= 0) {
        error_count++;
        printf("        Got %d comparing '", result);
        test_print_string(stdout, b_string);
        printf("' to '");
        test_print_string(stdout, a_string);
        printf("'.\n");
        printf("    Expected < 0.\n");
    }
    result = pod_string_compare(d_string, a_string);
    if (result != 0) {
        error_count++;
        printf("        Got %d comparing '", result);
        test_print_string(stdout, d_string);
        printf("' to '");
        test_print_string(stdout, a_string);
        printf("'.\n");
        printf("    Expected 0.\n");
    }
    result = pod_string_compare(c_string, a_string);
    if (result <= 0) {
        error_count++;
        printf("        Got %d comparing '", result);
        test_print_string(stdout, c_string);
        printf("' to '");
        test_print_string(stdout, a_string);
        printf("'.\n");
        printf("    Expected > 0.\n");
    }
    result = pod_string_compare_to_cstring(a_string, test_string3);
    if (result >= 0) {
        error_count++;
        printf("        Got %d comparing '", result);
        test_print_string(stdout, a_string);
        printf("' to cstring '%s'.\n", test_string3);
        printf("    Expected < 0.\n");
    }
    result = pod_string_compare_to_cstring(a_string, test_string);
    if (result != 0) {
        error_count++;
        printf("        Got %d comparing '", result);
        test_print_string(stdout, a_string);
        printf("' to cstring '%s'.\n", test_string);
        printf("    Expected 0.\n");
    }
    result = pod_string_compare_to_cstring(a_string, test_string2);
    if (result <= 0) {
        error_count++;
        printf("        Got %d comparing '", result);
        test_print_string(stdout, a_string);
        printf("' to cstring '%s'.\n", test_string2);
        printf("    Expected > 0.\n");
    }

    return error_count;
}



    // test_dup
    //
    // Test the functions pod_string_dup and pod_string_dup_text.

int test_dup(void)
{
    pod_string *dup;
    pod_string *dup_text;
    int error_count;
    int result;
    pod_string *string;

    printf("    test_dup\n");
    error_count = 0;
    string = pod_string_create(STRING_SIZE, 0);
    if (string == NULL) {
        return test_create_string_error("        string:");
    }
    pod_string_copy_from_cstring(string, test_string);
    dup = pod_string_dup(string);
    if (dup == NULL) {
        return test_create_string_error("        dup:");
    }
    dup_text = pod_string_dup_text(string);
    if (dup_text == NULL) {
        return test_create_string_error("        dup_text:");
    }
    if (string->size == 0) {
        error_count++;
        printf("        string->size is 0.  It should have been initialized to something else.\n");
    }
    if (string->size != dup->size) {
        error_count++;
        printf("        string->size (%u) and dup->size (%u) differ.\n", string->size, dup->size);
    }
    if (string->used != dup->used) {
        error_count++;
        printf("        string->used (%u) and dup->used (%u) differ.\n", string->used, dup->used);
    }
    result = pod_string_compare(string, dup);
    if (result != 0) {
        error_count++;
        printf("        string and dup don't have the same text.\n");
    }
    if (string->size == dup_text->size) {
        error_count++;
        printf("        string->size (%u) and dup_text->size (%u) are the same.\n", string->size, dup_text->size);
    }
    if (string->used != dup_text->used) {
        error_count++;
        printf("        string->used (%u) and dup_text->used (%u) differ.\n", string->used, dup_text->used);
    }
    result = pod_string_compare(string, dup_text);
    if (result != 0) {
        error_count++;
        printf("        string and dup_text don't have the same text.\n");
    }

    return error_count;
}



    // test_append
    //
    // Test the pod_string_append... functions.

int test_append(void)
{
    char chars[STRING_SIZE + 1];
    int error_count;
    size_t new_used;
    pod_string *string;
    size_t used;

    printf("    test_append\n");
    chars[STRING_SIZE] = '\0';
    error_count = 0;
    string = pod_string_create(STRING_SIZE, 0);
    if (string == NULL) {
        return test_create_string_error("        string:");
    }
    if (string->used != 0) {
        ++error_count;
        printf("        string->used is NOT 0, and it was just created.\n");
    }
    pod_string_append_char(string, test_char);
    if (string->used != 1) {
        ++error_count;
        printf("        string->used is NOT 1.\n");
        pod_string_copy_to_cstring(chars, string);
        printf("            (String: %s)\n", chars);
    }
    pod_string_copy_from_cstring(string, test_string);
    used = string->used;
    pod_string_append_char(string, test_char);
    if (string->used - used != 1) {
        ++error_count;
        printf("        Appending a pod_char_t didn't change a string by 1.\n");
        pod_string_copy_to_cstring(chars, string);
        printf("            (String: %s)\n", chars);
    }
 
    return error_count;
}
