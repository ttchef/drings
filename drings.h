
#ifndef DRINGS_H
#define DRINGS_H 

#define DRINGS_IMPL // Temp only for development
#define DS_SMALL_STRING_CAPACITY 15

#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t length;
    bool is_heap;
    union {
        char stack_data[DS_SMALL_STRING_CAPACITY + 1]; 
        char* heap_data;
    };
} ds_String;

ds_String* ds_init_string(const char* string);
void ds_free_string(ds_String* string);
const char* ds_get_string_ptr(ds_String* string);

#ifdef __cplusplus
}
#endif

#endif // DRINGS_H

#ifdef DRINGS_IMPL

ds_String* ds_init_string(const char *string) {
    size_t length = strlen(string);
    ds_String* str = (ds_String*)malloc(sizeof(ds_String));
    if (!str) {
        fprintf(stderr, "[ERROR] Couldnt allocate enough memory for string: %s\n", string);
        return NULL;
    }

    str->length = length;

    if (length <= DS_SMALL_STRING_CAPACITY) {
        str->is_heap = false;
        memcpy(str->stack_data, string, length + 1);
    }
    else {
        str->is_heap = true;
        str->heap_data = (char*)malloc(length + 1);
        if (!str->heap_data) {
            fprintf(stderr, "[ERROR] Couldnt allocate heap data for string: %s\n", string);
            free(str);
            return NULL;
        }
        memcpy(str->heap_data, string, length + 1);
    }
    

    return str;
}

void ds_free_string(ds_String *string) {
    if (string->is_heap && string->heap_data) {
        free(string->heap_data);
    }
    free(string);
}

const char* ds_get_string_ptr(ds_String* string) {
    if (string->is_heap && string->heap_data) {
        return string->heap_data;
    }
    return string->stack_data;
}

#endif


