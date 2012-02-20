/******************************************************************** 
 *  test_pod_mapping.c
 *  Copyright (c) 2011, James A. Banks
 *  All rights reserved.
 *  See file LICENSE for details.
 ********************************************************************/

#include <stdio.h>
#include "pod_mapping.h"
#include "pod_string.h"



#define STRING_SIZE 100

char *test_chars[2] = { "Fee, fie, fo, fum.",
                        "I smell the blood of an Englishman"
                      };

pod_string *test_string[2];



    // Test pod_mapping and its associated functions.

int main(int argc, char *argv[])
{
    int error_count;
    int i;

    error_count = 0;
    printf("\n    testing pod_mapping\n");
    if (POD_MAPPING_TYPE != 0x63) {
        ++error_count;
        printf("        POD_MAPPING_TYPE is not the expected value.\n");
        printf("            (expected 0x63, got 0x%x)\n", POD_MAPPING_TYPE);
    }
    for (i = 0; i < 2; ++i) {
        test_string[i] = pod_string_create(STRING_SIZE, 0);
        pod_string_copy_from_cstring(test_string[i], test_chars[i]);
    }
    error_count += test_create_and_destroy_mapping();
    for (i = 0; i < 2; ++i) {
        test_string[i]->o.destroy(test_string[i]);
    }

    return error_count;
}



    // test_create_and_destroy_mapping
    //
    // Create a mapping.  Check its contents.  Destroy it.  Create a mapping
    // with a key and a value.  Check its contents.  Destroy it.
    //
    // Returns:
    //      int     The number of errors found.

int test_create_and_destroy_mapping(void)
{
    int error_count;
    pod_mapping *mapping;

    printf("    test_create_and_destroy_mapping\n");
    error_count = 0;
    mapping = pod_mapping_create();
    if (mapping == NULL) {
        printf("        pod_mapping_create returned NULL.\n");
        return 1;
    }
    fflush(stdout);
    if (mapping->o.n.previous != NULL) {
        ++error_count;
        printf("        (1) Mapping's o.n.previous is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", mapping->o.n.previous);
    }
    fflush(stdout);
    if (mapping->o.n.next != NULL) {
        ++error_count;
        printf("        (1) Mapping's o.n.next is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", mapping->o.n.next);
    }
    fflush(stdout);
    if (mapping->o.type != POD_MAPPING_TYPE) {
        ++error_count;
        printf("        (1) Mapping's type is not POD_MAPPING_TYPE.\n");
        printf("            (expected 0x63, got 0x%u)\n", mapping->o.type);
    }
    fflush(stdout);
    if (mapping->o.destroy != pod_mapping_destroy) {
        ++error_count;
        printf("        (1) Mapping's o.destroy is not set to pod_mapping_destroy.\n");
    }
    fflush(stdout);
    if (mapping->key != NULL) {
        ++error_count;
        printf("        (1) Mapping's key is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", mapping->key);
    }
    fflush(stdout);
    if (mapping->value != NULL) {
        ++error_count;
        printf("        (1) Mapping's value is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", mapping->value);
    }
    fflush(stdout);
    mapping->o.destroy(mapping);
    mapping = pod_mapping_create_with(test_string[0], (pod_object *) test_string[1]);
    if (mapping == NULL) {
        printf("        pod_mapping_create_with returned NULL.\n");
        return 1;
    }
    if (mapping->o.n.previous != NULL) {
        ++error_count;
        printf("        (2) Mapping's o.n.previous is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", mapping->o.n.previous);
    }
    if (mapping->o.n.next != NULL) {
        ++error_count;
        printf("        (2) Mapping's o.n.next is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", mapping->o.n.next);
    }
    if (mapping->o.type != POD_MAPPING_TYPE) {
        ++error_count;
        printf("        (2) Mapping's type is not POD_MAPPING_TYPE.\n");
        printf("            (expected 0x63, got 0x%u)\n", mapping->o.type);
    }
    if (mapping->o.destroy != pod_mapping_destroy) {
        ++error_count;
        printf("        (2) Mapping's o.destroy is not set to pod_mapping_destroy.\n");
    }
    if (mapping->key != test_string[0]) {
        ++error_count;
        printf("        (2) Mapping's key is not what it should be.\n");
        printf("            (expected 0x%p, got 0x%p)\n", test_string[0], mapping->key);
    }
    if (mapping->value != (pod_object *) test_string[1]) {
        ++error_count;
        printf("        (2) Mapping's value is not what it should be.\n");
        printf("            (expected 0x%p, got 0x%p)\n", test_string[1], mapping->value);
    }
    /* NULL out key and value to avoid destroying them and getting a segfault */
    mapping->key = NULL;
    mapping->value = NULL;
    mapping->o.destroy(mapping);

    return error_count;
}
