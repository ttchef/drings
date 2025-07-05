
#include <stdio.h> 

#define DRINGS_IMPL
#include "drings.h"

int main() {
    ds_String* string = ds_init_string("Hello ");
        
    ds_append(string, "World");
    printf("%s\n", ds_get_string_ptr(string));

    ds_set(string, "Hello World Guys love you");
    printf("%s\n", ds_get_string_ptr(string));

    ds_free_string(string);
    return 0;
}

