
#include <stdio.h> 

#define DRINGS_IMPL
#include "drings.h"

int main() {
    ds_String* string = ds_init_string("Hello ");
        
    ds_append("you are a nice persoon", string);
    printf("%s\n", ds_get_string_ptr(string));

    ds_free_string(string);
    return 0;
}

