#include <stdio.h>
#include "../pod_list.h"
#include "../pod_string.h"



#define STRING_SIZE 100

char *test_chars[4] = { "Baa, baa, black sheep,",
                         "have you any wool?",
                         "Yes sir, yes sir,",
                         "three bags full."
                      };

pod_string *test_string[4];



    // Test pod_list and its associated functions.

int main(int argc, char *argv[])
{
    int error_count;
    int i;

    error_count = 0;
    printf("\n    testing pod_list\n");
    if (POD_LIST_TYPE != 0x62) {
        error_count++;
        printf("    POD_LIST_TYPE is not the expected value.\n");
        printf("        (expected 0x62, got 0x%x)\n", POD_LIST_TYPE);
    }
    for (i = 0; i < 4; i++) {
        test_string[i] = pod_string_create(STRING_SIZE, 0);
        pod_string_copy_from_cstring(test_string[i], test_chars[i]);
    }
    error_count += test_create_and_destroy_list();
    error_count += test_list_append_peek_pop_push();
    for (i = 0; i < 4; i++) {
        test_string[i]->o.destroy(test_string[i]);
    }

    return error_count;
}



    // test_create_and_destroy_list
    //
    // Create a list, make sure everything is correct, and destroy it.
    //
    // Returns:
    //      int     The number of errors found.

int test_create_and_destroy_list(void)
{
    int error_count;
    pod_list *list;

    printf("    test_create_and_destroy_list\n");
    error_count = 0;
    list = pod_list_create();
    if (list == NULL) {
    }
    if (list->o.n.previous != NULL) {
        ++error_count;
        printf("        List's o.n.previous is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", list->o.n.previous);
    }
    if (list->o.n.next != NULL) {
        ++error_count;
        printf("        List's o.n.next is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", list->o.n.next);
    }
    if (list->o.type != POD_LIST_TYPE) {
        ++error_count;
        printf("        List's type is not POD_LIST_TYPE.\n");
        printf("            (expected 0x62, got 0x%u)\n", list->o.type);
    }
    if (list->o.destroy != pod_list_destroy) {
        ++error_count;
        printf("        List's o.destroy is not set to pod_list_destroy.\n");
    }
    if (list->header.previous != &list->header) {
        ++error_count;
        printf("        List's header.previous is not the header's address.\n");
        printf("            (expected 0x%p, got 0x%p)\n", &list->header, list->header.previous);
    }
    if (list->header.next != &list->header) {
        ++error_count;
        printf("        List's header.next is not the header's address.\n");
        printf("            (expected 0x%p, got 0x%p)\n", &list->header, list->header.next);
    }
    list->o.destroy(list);

    return error_count;
}



int test_list_append_peek_pop_push(void)
{
    pod_object *a;
    pod_object *b;
    int error_count;
    int i;
    pod_list *list;
    size_t size;

    printf("    test_list_append_peek_pop_push\n");
    error_count = 0;
    list = pod_list_create();
    for (i = 0; i < 4; i++) {
        pod_list_append(list, (pod_object *) test_string[i]);   
        size = pod_list_size(list);
        if (size != (size_t) i + 1) {
            ++error_count;
            printf("        Post-append size is not what is expected.\n");
            printf("            (expected %d, got %zu)\n", i + 1, size);
        }
    }
    for (i = 3; i >= 0; i--) {
        a = pod_list_peek(list);
        b = pod_list_pop(list);
        if (a != b) {
            ++error_count;
            printf("        (1) Peek does not equal pop. (i = %d)\n", i);
            printf("            (Peek=0x%p, Pop = 0x%p)\n", a, b);
        }
        size = pod_list_size(list);
        if (size != (size_t) i) {
            ++error_count;
            printf("        (1) Post-pop size is not what is expected.\n");
            printf("            (expected %d, got %zu)\n", i, size);
        }
    }
    for (i = 0; i < 4; i++) {
        pod_list_push(list, (pod_object *) test_string[i]);   
        a = pod_list_peek(list);
        if (a != (pod_object *) test_string[i]) {
            ++error_count;
            printf("        Peek does not equal what was pushed. (i = %d)\n", i);
            printf("            (Peek=0x%p, Pushed = 0x%p)\n", a, test_string[i]);
        }
        size = pod_list_size(list);
        if (size != (size_t) i + 1) {
            ++error_count;
            printf("        Post-push size is not what is expected.\n");
            printf("            (expected %d, got %zu)\n", i + 1, size);
        }
    }
    for (i = 3; i >= 0; i--) {
        a = pod_list_peek(list);
        b = pod_list_pop(list);
        if (a != b) {
            ++error_count;
            printf("        (2) Peek does not equal pop. (i = %d)\n", i);
            printf("            (Peek=0x%p, Pop = 0x%p)\n", a, b);
        }
        size = pod_list_size(list);
        if (size != (size_t) i) {
            ++error_count;
            printf("        (2) List's size is not what is expected.\n");
            printf("            (expected %d, got %zu)\n", i, size);
        }
    }
}
