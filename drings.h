
#ifndef DRINGS_H
#define DRINGS_H 

#define DRINGS_IMPL // Temp only for development
                    //
#define DS_SMALL_STRING_CAPACITY 15

#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DS_OK = 0,
    DS_ERROR = -1,
    DS_ALLOC_FAIL = -2,
    DS_OUT_OF_BOUNDS = -3,
} DS_RESULT;

typedef enum {
    DS_IS_HEAP = 0x1,
    DS_IS_STACK = 0x2,
    DS_OWNS_MEM = 0x4,
    DS_READ_ONLY = 0x8,
} DS_FLAG;

typedef struct {
    uint32_t length;
    uint32_t capacity;
    uint32_t flags;
    union {
        char stack_data[DS_SMALL_STRING_CAPACITY + 1]; 
        char* heap_data;
    };
} ds_String;

typedef struct {
    const char* data;
    uint32_t length;
} ds_StringView;

typedef struct {
    char* data;
    uint32_t length;
} ds_MutableStringView;

// construct
ds_String* ds_init_string(const char* string);
void ds_free_string(ds_String* string);

// methods

// helper
static inline bool ds_is_heap(const ds_String* string) {
    return (string->flags & DS_IS_HEAP) != 0;
}

static inline bool ds_is_stack(const ds_String* string) {
    return (string->flags & DS_IS_STACK) != 0;
}

static inline bool ds_is_empty(const ds_String* string) {
    return string->length == 0;
}

static inline char* ds_data(ds_String* string) {
    return ds_is_heap(string) ? string->heap_data : string->stack_data;
}

// private



#ifdef __cplusplus
}
#endif

#endif // DRINGS_H

#ifdef DRINGS_IMPL



#endif


