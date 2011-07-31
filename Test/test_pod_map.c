#include <stdio.h>
#include "../pod_map.h"
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
    error_count += test_map_define();
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



    // test_map_define
    //
    // Returns:
    //      int     The number of errors found.

int test_map_define(void)
{
    int error_count;
    int i;
    pod_map *map;
    pod_object *object;
    size_t size;

    printf("    test_map_define\n");
    error_count = 0;
    map = pod_map_create();
    for (i = 0; i < 3; ++i) {
        pod_map_assign(map, key[i], (pod_object *) value[i]);
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

    return error_count;
}
