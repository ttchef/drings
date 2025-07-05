
#ifndef DRINGS_H
#define DRINGS_H 

//#define DRINGS_IMPL // Temp only for development
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

// overall
ds_String* ds_init_string(const char* string);
void ds_free_string(ds_String* string);
const char* ds_get_string_ptr(ds_String* string);

// methods
void ds_append(ds_String* string, const char* literal);
void ds_set(ds_String* string, const char* literal);
ds_String* ds_clone(ds_String* string);

// helper functions
static inline uint32_t ds_length(ds_String* string) {
    return string->length;
}

static inline bool ds_is_heap(ds_String* string) {
    return string->is_heap;
}

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

void ds_set(ds_String* string, const char* literal) {
    size_t lit_length = strlen(literal);
    
    if (lit_length <= DS_SMALL_STRING_CAPACITY) {
        // free heap if there
        if (string->is_heap && string->heap_data) {
            string->is_heap = false;
            free(string->heap_data);
            string->heap_data = NULL;
        }
        memcpy(string->stack_data, literal, lit_length + 1);
        string->length = lit_length;
    }
    else {
        if (!string->is_heap) {
            string->heap_data = (char*)malloc(lit_length + 1);
            if (!string->heap_data) {
                fprintf(stderr, "[ERROR] Couldnt allocate string: %s to the heap with literal: %s from ds_set\n",
                        string->stack_data, literal);
                return;
            }
            string->is_heap = true;
        }
        else {
            char* heap_buffer = (char*)realloc(string->heap_data, lit_length + 1);
            if (!heap_buffer) {
                fprintf(stderr, "[ERROR] Couldnt reallocate string: %s on the heap while trying to set it to: %s in the ds_set call\n",
                        string->heap_data, literal);
                return;
            }
            string->heap_data = heap_buffer;
        }
        memcpy(string->heap_data, literal, lit_length + 1);
        string->length = lit_length;
    }
}

void ds_append(ds_String* string, const char* literal) {
    size_t lit_length = strlen(literal);
    size_t new_length = string->length + lit_length;

    if (string->is_heap && string->heap_data) {
        char* new_heap_data = (char*)realloc(string->heap_data, new_length + 1);
        if (!new_heap_data) {
            fprintf(stderr, "[ERROR] Couldnt reallocate string: %s when appending literal: %s\n", string->heap_data, literal);
            return;
        }
        string->heap_data = new_heap_data;
        memcpy(string->heap_data + string->length, literal, lit_length + 1);
        string->length = new_length;
    } 

    else {
        if (new_length <= DS_SMALL_STRING_CAPACITY) {
            memcpy(string->stack_data + string->length, literal, lit_length + 1);
            string->length = new_length;
        }
        else {
            string->is_heap = true;
            char* heap_buffer = (char*)malloc(new_length + 1);
            if (!heap_buffer) {
                fprintf(stderr, "[ERROR] Couldnt allocated heap data for string: %s while appending: %s\n", string->stack_data, literal);
                return;
            }
            memcpy(heap_buffer, string->stack_data, string->length);
            memcpy(heap_buffer + string->length, literal, lit_length + 1);
            string->heap_data = heap_buffer;
            string->length = new_length;
        }
    }
}

ds_String* ds_clone(ds_String* string) {
    ds_String* str = (ds_String*)malloc(sizeof(ds_String));
    if (!str) {
        fprintf(stderr, "[ERROR] Couldnt allocate cloning string in ds_clone function\n");
        return NULL;
    }

    if (string->is_heap && string->heap_data) {
        str->heap_data = (char*)malloc(string->length + 1);   
        if (!str->heap_data) {
            fprintf(stderr, "[ERROR] Couldnt allocate heap data for cloned string in ds_clone function\n");
            return NULL;
        }
        memcpy(str->heap_data, string->heap_data, string->length + 1);
        str->is_heap = true;
    }
    else {
        memcpy(str->stack_data, string->stack_data, string->length + 1);
        str->is_heap = false;
    }
    str->length = string->length;

    return str;
}

#endif


