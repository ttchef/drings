
#include <stdio.h> 

#define DRINGS_IMPL
#include "drings.h"

int main() {

    ds_String* string1 = ds_init_string("Hello World Guys");
    printf("%s\n", ds_to_c_str(string1));

    ds_free_string(string1);

    /*
    ds_String* string1 = ds_init_string("Hello ");
    ds_append(string1, "World");

    printf("%s\n", ds_get_string_ptr(string1));

    ds_set(string1, "Hello World Guys love you");
    printf("%s\n", ds_get_string_ptr(string1));

    ds_String* string2 = ds_clone(string1);
    printf("%s\n", ds_get_string_ptr(string2));

    ds_String* string3 = ds_init_string("I am on the hea");
    char c = ds_pop(string3);
    printf("Char: %c String: %s\n", c, ds_get_string_ptr(string3));
    c = ds_pop(string3);
    printf("Char: %c String: %s\n", c, ds_get_string_ptr(string3));

    // reserve
    ds_reserve(string1, 100);
    for (int i = 0; i < 100; i++) {
        ds_append(string1, "Abc");
    }
    printf("%s\n", ds_get_string_ptr(string1));

    ds_free_string(string1);
    ds_free_string(string2);
    ds_free_string(string3);

    */
    return 0;
}

