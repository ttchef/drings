
# drings 

A simple dynamic string libary for c 


# Example 
```c


#include <stdio.h> 

#define DRINGS_IMPL
#include "drings.h"

int main() {

    ds_String* string1 = ds_init_string("He");
    ds_append(string1, "llo");

    printf("%s\n", ds_to_c_str(string1));

    ds_free_string(string1);

    return 0;
}

```
