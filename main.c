
#include <stdio.h> 

#define DRINGS_IMPL
#include "drings.h"

int main() {

    ds_String* string1 = ds_init_string("Hello Guys with ");
    ds_append(string1, "llo");

    ds_String* string2 = ds_init_string("ich gehe zur sch");
    ds_append_dstring(string1, string2);
        
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

    ds_free_string(string1);
    ds_free_string(string2);
    ds_free_string(string3);
    ds_free_string(s1);
    ds_free_string(s2);

    return 0;
}

