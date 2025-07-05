
#include <stdio.h> 

#define DINGS_IMPL
#include "dings.h"

int main() {
    ds_String* string = ds_init_string("Hello World");
        
    printf("%s\n", ds_get_string_ptr(string));

    ds_free_string(string);
    return 0;
}

