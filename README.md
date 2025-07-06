
# drings 

A simple dynamic string libary for c 


# Example 
```c

#include <stdio.h> 

#define DRINGS_IMPL
#include "drings.h"

int main() {

    ds_String* string1 = ds_init_string("Hello Guys with ");
    ds_append(string1, "llo");

    ds_String* string2 = ds_init_string("ich gehe zur sch");
    ds_append_dstring(string1, string2);
    char c = ds_pop(string2);
    printf("%c\n", c);


    printf("%s\n", ds_to_c_str(string2));

    ds_free_string(string1);

    return 0;
}


```
