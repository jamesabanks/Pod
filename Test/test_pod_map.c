#include <stdio.h>
#include <string.h>
#include "../pod_map.h"
#include "../pod_node.h"
#include "../pod_string.h"



#define STRING_SIZE 100

char *test_keys[5] = { "one",
                       "two",
                       "three",
                       "four",
                       "five"
                     };

char *test_values[5] = { "Baa, baa, black sheep,",
                         "have you any wool?",
                         "Yes sir, yes sir,",
                         "three bags full.",
                         "Nursery Rhymes"
                      };

int test_remove[4] = { 1, 0, 3, 2 };

pod_string *key[5];
pod_string *value[5];



    // Test pod_map and its associated functions.

int main(int argc, char *argv)
{
    int error_count;
    int i;

    printf("\n    testing pod_map\n");
    error_count = 0;
    if (POD_MAP_TYPE != 0x64) {
        error_count++;
        printf("    POD_MAP_TYPE is not the expected value.\n");
        printf("        (expected 0x64, got 0x%x)\n", POD_MAP_TYPE);
    }
    for (i = 0; i < 5; ++i) {
        key[i] = pod_string_create(STRING_SIZE, 0);
        pod_string_copy_from_cstring(key[i], test_keys[i]);
        value[i] = pod_string_create(STRING_SIZE, 0);
        pod_string_copy_from_cstring(value[i], test_values[i]);
    }
    error_count += test_create_and_destroy_map();
    error_count += test_map_link_integrity();
    error_count += test_map_functions();
    for (i = 0; i < 5; ++i) {
        key[i]->o.destroy(key[i]);
        value[i]->o.destroy(value[i]);
    }

    return error_count;
}



    // test_create_and_destroy_map
    //
    // Create a map, make sure everything is correct, and destroy it.
    //
    // Returns:
    //      int     The number of errors found.

int test_create_and_destroy_map(void)
{
    int error_count;
    pod_map *map;

    printf("    test_create_and_destroy_map\n");
    error_count = 0;
    map = pod_map_create();
    if (map == NULL) {
        printf("        pod_map_create returned NULL.\n");
        return 1;
    }
    if (map->o.n.previous != NULL) {
        ++error_count;
        printf("        Map's o.n.previous is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", map->o.n.previous);
    }
    if (map->o.n.next != NULL) {
        ++error_count;
        printf("        Map's o.n.next is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", map->o.n.next);
    }
    if (map->o.type != POD_MAP_TYPE) {
        ++error_count;
        printf("        Map's type is not POD_LIST_TYPE.\n");
        printf("            (expected 0x62, got 0x%u)\n", map->o.type);
    }
    if (map->o.destroy != pod_map_destroy) {
        ++error_count;
        printf("        Map's o.destroy is not set to pod_list_destroy.\n");
    }
    if (map->header.previous != &map->header) {
        ++error_count;
        printf("        Map's header.previous is not the header's address.\n");
        printf("            (expected 0x%p, got 0x%p)\n", &map->header, map->header.previous);
    }
    if (map->header.next != &map->header) {
        ++error_count;
        printf("        Map's header.next is not the header's address.\n");
        printf("            (expected 0x%p, got 0x%p)\n", &map->header, map->header.next);
    }
    if (map->current != NULL) {
        ++error_count;
        printf("        Map's current is not NULL.\n");
        printf("            (expected NULL, got 0x%p)\n", map->current);
    }
    map->o.destroy(map);

    return error_count;
}



    // test_map_link_integrity
    //
    // Check to see if the links are consitent.
    //
    // Returns:
    //      int     The number of errors found.

int test_map_link_integrity(void)
{
    int error_count;
    int i;
    pod_map *map;
    pod_mapping *mapping;
    pod_node *node;

    printf("    test_map_link_integrity\n");
    error_count = 0;
    map = pod_map_create();
    for (i = 0; i < 5; i++) {
        pod_map_define(map, key[i], (pod_object *) value[i]);
    }
    node = map->header.next;
    i = 0;
    while (node != &map->header) {
        if (node != node->previous->next) {
            ++error_count;
            printf("        mapping node != node->previous->next (i = %d)\n",i);
        }
        if (node != node->next->previous) {
            ++error_count;
            printf("        mapping node != node->next->previous (i = %d)\n",i);
        }
        node = node->next;
        i++;
    }
    for (i = 0; i < 5; i++) {
        pod_map_remove(map, key[i]);
    }
    map->o.destroy(map);

    return error_count;
}



    // test_map_functions
    //
    // Returns:
    //      int     The number of errors found.

int test_map_functions(void)
{
    int error_count;
    int i;
    int j;
    pod_map *map;
    pod_mapping *mapping;
    pod_object *object;
    size_t size;
    char text[101];
    char text2[101];

    printf("    test_map_functions\n");
    error_count = 0;
    map = pod_map_create();
    text[100] = '\0';
    text2[100] = '\0';
    for (i = 0; i < 4; ++i) {
        pod_map_define(map, key[i], (pod_object *) value[i]);
        object = pod_map_lookup(map, key[i]);
        if (object != (pod_object *) value[i]) {
            ++error_count;
            printf("        Returned object not equalling key[%d].\n", i);
            printf("            (expected 0x%p, got 0x%p)\n", value[i], object);
        }
        size = pod_map_size(map);
        if (size != i + 1) {
            ++error_count;
            printf("        Size is not correct after defining %d object(s).\n", i);
            printf("            (expected %d, got %zu)\n", i + 1, size);
        }
    }
    i = 0;
    size = pod_map_size(map);
    pod_map_iterate(map);
    mapping = pod_map_next(map);
    while (mapping != NULL) {
        ++i;
        pod_string_copy_to_cstring(text, (pod_string *) mapping->key);
        pod_string_copy_to_cstring(text2, (pod_string *) mapping->value);
        //printf("        ['%s' has '%s']\n", text, text2);
        mapping = pod_map_next(map);
    }
    if (i != size) {
        ++error_count;
        printf("        Did not iterate over 'size' number of mappings.\n");
        printf("            (expected %zu, iterated over %d.", size, i);
    }
    /* The state of the map:
        The map should look like this:
            ( 3 2 1 0 ) or ("four" "three" "two" "one")
        The most coverage is acheived by removing two one four three.
            (or something equivalent, first, middle, last, alone)
    */
    size = pod_map_size(map);
    if (size != 4) {
        ++error_count;
        printf("        Pre-remove size wasn't four.\n");
        printf("            (expected 4, got %zu\n", size);
    }
    for (i = 0; i < 4; i++) {
        j = test_remove[i];
        mapping = pod_map_lookup_mapping(map, key[j]);
        object = pod_map_remove(map, key[j]);
        if (object != (pod_object *) value[j]) {
            ++error_count;
            if (object != NULL) {
                pod_string_copy_to_cstring(text, (pod_string *) object);
            } else {
                strcpy(text, "NULL");
            }
            printf("        Remove didn't get the expected value for %d (i = %d).\n", j, i);
            printf("            ('%s' got '%s')\n", test_keys[j], text);
            printf("            (expected '%s')\n", test_values[j]);
        }
    }
    size = pod_map_size(map);
    if (size != 0) {
        ++error_count;
        printf("        Final size wasn't zero.\n");
        printf("            (expected 0, got %zu\n", size);
    }

    return error_count;
}
