
# drings 

A simple dynamic string libary for c 

# Notes
The string_view functions havent been tested yet!
This libary is under developement right now so some functions may not work or havent been implemented

# Example 
```c

#include <stdio.h> 

#define DRINGS_IMPL
#include "drings.h"

int main() {

    ds_String* string1 = ds_init_string("He");
    ds_append(string1, "llo");

    ds_String* string2 = ds_init_string("ich gehe zur sch");
    //ds_append_dstring(string1, string2);
        
    printf("%s\n", ds_to_c_str(string2));

    ds_String* string3 = ds_init_string("");
    ds_reserve(string3, 100);
    for (int i = 0; i < 100; i++) {
        ds_append(string3, "A");
    }
    printf("%s\n", ds_to_c_str(string3));

    ds_clear(string3);
    printf("%s\n", ds_to_c_str(string3));

    ds_String* s1 = ds_init_string("Hello");
    ds_String* s2 = ds_init_string("Hello guys wtf is going");

    printf("%b\n", ds_equal(s1, s2));

    ds_set(s1, "I am");
    printf("%s\n", ds_to_c_str(s1));

    ds_clone(string1, s2);
    printf("%s\n", ds_to_c_str(string1));

    ds_String* s3 = ds_init_string("     Hello,A    B   C  ");
    ds_trim_whitespace_flags(s3, DS_FRONT | DS_BACK);
    ds_String* s4 = ds_split(s3, ',');
    printf("%s\n", ds_to_c_str(s3));
    printf("%s\n", ds_to_c_str(s4));

    ds_free_string(string1);
    ds_free_string(string2);
    ds_free_string(string3);
    ds_free_string(s1);
    ds_free_string(s2);
    ds_free_string(s3);
    ds_free_string(s4);


    return 0;
}
```
