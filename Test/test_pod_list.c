#include <stdio.h>
#include "../pod_list.h"
#include "../pod_string.h"



#define STRING_SIZE 100

char *test_chars[5] = { "Baa, baa, black sheep,",
                         "have you any wool?",
                         "Yes sir, yes sir,",
                         "three bags full.",
                         "Nursery Rhymes"
                      };

pod_string *test_string[5];



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
    for (i = 0; i < 5; i++) {
        test_string[i] = pod_string_create(STRING_SIZE, 0);
        pod_string_copy_from_cstring(test_string[i], test_chars[i]);
    }
    error_count += test_create_and_destroy_list();
    error_count += test_list_append_peek_pop_push_size();
    error_count += test_list_find_insert_remove();
    error_count += test_list_replace();
    error_count += test_list_apply_all();
    for (i = 0; i < 5; i++) {
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



int test_list_append_peek_pop_push_size(void)
{
    pod_object *a;
    pod_object *b;
    int error_count;
    int i;
    pod_list *list;
    size_t size;

    printf("    test_list_append_peek_pop_push_size\n");
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
    a = pod_list_peek(list);
    if (a != NULL) {
        ++error_count;
        printf("        List should be empty, but peek returned not-NULL.\n");
        printf("            (expected NULL, got %p)\n", a);
    }
    a = pod_list_pop(list);
    if (a != NULL) {
        ++error_count;
        printf("        List should be empty, but pop returned not-NULL.\n");
        printf("            (expected NULL, got %p)\n", a);
    }
    for (i = 0; i < 4; i++) {
        if (test_string[i]->o.n.previous != NULL) {
            ++error_count;
            printf("        String test_string[%d]->o.n.previous != NULL.\n", i);
        }
        if (test_string[i]->o.n.next != NULL) {
            ++error_count;
            printf("        String test_string[%d]->o.n.next != NULL.\n", i);
        }
    }
    list->o.destroy(list);

    return error_count;
}



int test_list_find_insert_remove(void)
{
    int error_count;
    int i;
    pod_list *list;
    pod_object *object;
    size_t size;

    /* Test Ouline:
    size is 0

    insert 0 at 0 (0)         size is now 1
    insert 1 at 1 (0 1)       size is now 2
    insert 2 at 1 (0 2 1)     size is now 3
    insert 3 at 0 (3 0 2 1)   size is now 4

    insert 5 (should equal NULL)
    
    find 0  test_string = 3
    find 1  test_string = 0
    find 2  test_string = 2
    find 3  test_string = 1

    find 5 (should equal NULL)

    remove 5 (should equal NULL)

    remove 1 from (3 0 2 1) got 0  now (3 2 1) size is now 3
    remove 1 from (3 2 1)   got 2  now (3 1)   size is now 2
    remove 0 from (3 1)     got 3  now (1)     size is now 1
    remove 0 from (1)       got 1  now ()      size is now 0

    remove 0 (should equal NULL)
    */

    printf("    test_list_find_insert_remove\n");
    error_count = 0;
    list = pod_list_create();
    object = pod_list_insert(list, 0, (pod_object *) test_string[0]);
    // Test pod_list_insert
    if (object != (pod_object *) test_string[0]) {
        ++error_count;
        printf("        Got wrong object back from first insert.\n");
        printf("            (expected 0x%p, got 0x%p)\n", test_string[0], object);
    }
    size = pod_list_size(list);
    if (size != 1) {
        ++error_count;
        printf("        Size after first insert is wrong.\n");
        printf("            (expected 1, got %zu)", size);
    }
    object = pod_list_insert(list, 1, (pod_object *) test_string[1]);
    if (object != (pod_object *) test_string[1]) {
        ++error_count;
        printf("        Got wrong object back from second insert.\n");
        printf("            (expected 0x%p, got 0x%p)\n", test_string[1], object);
    }
    size = pod_list_size(list);
    if (size != 2) {
        ++error_count;
        printf("        Size after second insert is wrong.\n");
        printf("            (expected 2, got %zu)", size);
    }
    object = pod_list_insert(list, 1, (pod_object *) test_string[2]);
    if (object != (pod_object *) test_string[2]) {
        ++error_count;
        printf("        Got wrong object back from third insert.\n");
        printf("            (expected 0x%p, got 0x%p)\n", test_string[2], object);
    }
    size = pod_list_size(list);
    if (size != 3) {
        ++error_count;
        printf("        Size after third insert is wrong.\n");
        printf("            (expected 1, got %zu)", size);
    }
    object = pod_list_insert(list, 0, (pod_object *) test_string[3]);
    if (object != (pod_object *) test_string[3]) {
        ++error_count;
        printf("        Got wrong object back from fourth insert.\n");
        printf("            (expected 0x%p, got 0x%p)\n", test_string[3], object);
    }
    size = pod_list_size(list);
    if (size != 4) {
        ++error_count;
        printf("        Size after first insert is wrong.\n");
        printf("            (expected 4, got %zu)", size);
    }
    object = pod_list_insert(list, 5, (pod_object *) test_string[4]);
    if (object != NULL) {
        ++error_count;
        printf("        Didn't get NULL after invalid insert.\n");
        printf("            (expected NULL, got 0x%p)\n", object);
    }
    // Test pod_list_find
    object = pod_list_find(list, 0);
    if (object != (pod_object *) test_string[3]) {
        ++error_count;
        printf("        Didn't find the third string object.\n");
        printf("            (expected 3 or 0x%p, got 0x%p)\n", test_string[3], object);
    }
    object = pod_list_find(list, 1);
    if (object != (pod_object *) test_string[0]) {
        ++error_count;
        printf("        Didn't find the zero-th string object.\n");
        printf("            (expected 0 or 0x%p, got 0x%p)\n", test_string[0], object);
    }
    object = pod_list_find(list, 2);
    if (object != (pod_object *) test_string[2]) {
        ++error_count;
        printf("        Didn't find the second string object.\n");
        printf("            (expected 3 or 0x%p, got 0x%p)\n", test_string[2], object);
    }
    object = pod_list_find(list, 3);
    if (object != (pod_object *) test_string[1]) {
        ++error_count;
        printf("        Didn't find the first string object.\n");
        printf("            (expected 3 or 0x%p, got 0x%p)\n", test_string[1], object);
    }
    object = pod_list_find(list, 5);
    if (object != NULL) {
        ++error_count;
        printf("        Didn't get NULL after invalid find.\n");
        printf("            (expected NULL, got 0x%p)\n", object);
    }
    // Test pod_list_remove
    object = pod_list_remove(list, 5);
    if (object != NULL) {
        ++error_count;
        printf("        Didn't get NULL after invalid remove.\n");
        printf("            (expected NULL, got 0x%p)\n", object);
    }

    object = pod_list_remove(list, 1);
    if (object != (pod_object *) test_string[0]) {
        ++error_count;
        printf("        Got wrong object back from first remove.\n");
        printf("            (expected 0x%p, got 0x%p)\n", test_string[0], object);
    }
    size = pod_list_size(list);
    if (size != 3) {
        ++error_count;
        printf("        Size after first remove is wrong.\n");
        printf("            (expected 3, got %zu)", size);
    }

    object = pod_list_remove(list, 1);
    if (object != (pod_object *) test_string[2]) {
        ++error_count;
        printf("        Got wrong object back from second remove.\n");
        printf("            (expected 0x%p, got 0x%p)\n", test_string[2], object);
    }
    size = pod_list_size(list);
    if (size != 2) {
        ++error_count;
        printf("        Size after second remove is wrong.\n");
        printf("            (expected 2, got %zu)", size);
    }

    object = pod_list_remove(list, 0);
    if (object != (pod_object *) test_string[3]) {
        ++error_count;
        printf("        Got wrong object back from third remove.\n");
        printf("            (expected 0x%p, got 0x%p)\n", test_string[3], object);
    }
    size = pod_list_size(list);
    if (size != 1) {
        ++error_count;
        printf("        Size after third remove is wrong.\n");
        printf("            (expected 1, got %zu)", size);
    }

    object = pod_list_remove(list, 0);
    if (object != (pod_object *) test_string[1]) {
        ++error_count;
        printf("        Got wrong object back from fourth remove.\n");
        printf("            (expected 0x%p, got 0x%p)\n", test_string[1], object);
    }
    size = pod_list_size(list);
    if (size != 0) {
        ++error_count;
        printf("        Size after fourth remove is wrong.\n");
        printf("            (expected 0, got %zu)", size);
    }

    object = pod_list_remove(list, 0);
    if (object != NULL) {
        ++error_count;
        printf("        Didn't get NULL after remove on empty list.\n");
        printf("            (expected NULL, got 0x%p)\n", object);
    }
    for (i = 0; i < 5; i++) {
        if (test_string[i]->o.n.previous != NULL) {
            ++error_count;
            printf("        String test_string[%d]->o.n.previous != NULL.\n", i);
        }
        if (test_string[i]->o.n.next != NULL) {
            ++error_count;
            printf("        String test_string[%d]->o.n.next != NULL.\n", i);
        }
    }

    list->o.destroy(list);

    return error_count;
}



int test_list_replace(void) {
    int error_count;
    pod_list *list;
    pod_object *object;
    pod_string *string;

    printf("    test_list_replace\n");
    error_count = 0;
    list = pod_list_create();
    pod_list_push(list, (pod_object *) test_string[0]);
    object = pod_list_replace(list, 0, (pod_object *) test_string[1]);
    if (object != (pod_object *) test_string[0]) {
        ++error_count;
        printf("        Didn't return test_string[0] after replace.\n");
    }
    string = (pod_string *) pod_list_pop(list);
    if (string != test_string[1]) {
        ++error_count;
        printf("        Didn't get first test string back after replace.\n");
        printf("            (expected %p, got 0x%p)\n", test_string[1], string);
    }
    object = pod_list_replace(list, 0, (pod_object *) test_string[2]);
    if (object != NULL) {
        ++error_count;
        printf("        Didn't return NULL from replace on empty list.\n");
    }
    list->o.destroy(list);

    return error_count;
}


int test_list_apply_all(void)
{
    int error_count;
    pod_list *list;

    printf("    test_list_apply_all\n");
    error_count = 0;
    list = pod_list_create();
    printf("        Not implemented yet.\n");
    list->o.destroy(list);
}
