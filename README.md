
# drings 

A simple dynamic string libary for c 


# Example 
```c


#include <stdio.h> 

#define DRINGS_IMPL
#include "drings.h"

int main() {
    ds_String* string = ds_init_string("Hello World");
        
    printf("%s\n", ds_get_string_ptr(string));

    ds_free_string(string);
    return 0;
}

```
