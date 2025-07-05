
# drings 

A simple dynamic string libary for c 


# Example 
```c

#include <stdio.h> 

#define DRINGS_IMPL
#include "drings.h"

int main() {
    ds_String* string1 = ds_init_string("Hello ");
    ds_append(string1, "World");

    printf("%s\n", ds_get_string_ptr(string1));

    ds_set(string1, "Hello World Guys love you");
    printf("%s\n", ds_get_string_ptr(string1));

    ds_String* string2 = ds_clone(string1);
    printf("%s\n", ds_get_string_ptr(string2));

    ds_free_string(string1);
    ds_free_string(string2);
    return 0;
}
```
