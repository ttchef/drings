
#ifndef DRINGS_H
#define DRINGS_H 

#define DRINGS_IMPL // Temp only for development
                    //
#define DS_SMALL_STRING_CAPACITY 15
#define DS_STACK_CAPACITY 0

#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DS_UNDEFINIED = 1,
    DS_OK = 0,
    DS_ERROR = -1,
    DS_ALLOC_FAIL = -2,
    DS_OUT_OF_BOUNDS = -3,
    DS_INVALID_INPUT = -4,
} DS_RESULT;

typedef enum {
    DS_IS_HEAP = 0x1,
    DS_OWNS_MEM = 0x2,
    DS_READ_ONLY = 0x4,
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

typedef struct {
    DS_RESULT error_code;
    const char* function_name;
    const char* file_name;
    int line_number;
    char message[256];
} ds_ErrorInfo;

// construct
ds_String* ds_init_string(const char* string);
void ds_free_string(ds_String* string);

// methods
const char* ds_to_c_str(ds_String* string);
void ds_append_literal(ds_String* string, const char* literal);
void ds_append_dstring(ds_String* string, ds_String* string1);
void ds_append_char(ds_String* string, char c);

char ds_pop(ds_String* string);

// helper
static inline bool ds_is_heap(const ds_String* string) {
    return (string->flags & DS_IS_HEAP) != 0;
}

static inline bool ds_is_stack(const ds_String* string) {
    return (string->flags & DS_IS_HEAP) == 0;
}

static inline bool ds_is_empty(const ds_String* string) {
    return string->length == 0;
}

static inline char* ds_data(ds_String* string) {
    return ds_is_heap(string) ? string->heap_data : string->stack_data;
}

static inline void ds_set_is_heap(ds_String* string) {
    string->flags |= DS_IS_HEAP;
}

static inline void ds_set_is_stack(ds_String* string) {
    string->flags &= ~DS_IS_HEAP;
}

static inline bool ds_has_valid_heap_data(const ds_String* string) {
    return (ds_is_heap(string) && string->heap_data);
}


// Erroc management
static ds_ErrorInfo ds_last_error = {0};
static bool ds_error_login_enabled = true;

void ds_default_error_callback(const ds_ErrorInfo* error);

typedef void (*ds_ErrorCallback)(const ds_ErrorInfo* error);
static ds_ErrorCallback ds_error_callback = ds_default_error_callback;

#define DS_SET_ERROR(code, msg, ...) \
    do { \
        if (ds_error_login_enabled) { \
            ds_last_error.error_code = code; \
            ds_last_error.function_name = __func__; \
            ds_last_error.file_name = __FILE__; \
            ds_last_error.line_number = __LINE__; \
            snprintf(ds_last_error.message, sizeof(ds_last_error.message), msg, ##__VA_ARGS__); \
            if (ds_error_callback) ds_error_callback(&ds_last_error); \
        } \
    } while(0)

void ds_set_error_callback(ds_ErrorCallback callback);
void ds_enable_error_loggin(bool enabled);
const ds_ErrorInfo* ds_get_last_error();
void ds_clear_last_error();
const char* ds_error_string(DS_RESULT result);

// private



#ifdef __cplusplus
}
#endif

#endif // DRINGS_H

#ifdef DRINGS_IMPL

void ds_set_error_callback(ds_ErrorCallback callback) {
    if (callback) ds_error_callback = callback;
}

void ds_enable_error_loggin(bool enabled) {
    ds_error_login_enabled = enabled;
}

const ds_ErrorInfo* ds_get_last_error() {
    return &ds_last_error;
}

void ds_clear_last_error() {
    ds_last_error.error_code = DS_UNDEFINIED;
    ds_last_error.file_name = 0;
    ds_last_error.function_name = 0;
    ds_last_error.line_number = 0;
}

const char* ds_error_string(DS_RESULT result) {
    switch (result) {
        case DS_UNDEFINIED:     return "Undefinied";
        case DS_OK:             return "Success";
        case DS_ERROR:          return "General Error";
        case DS_ALLOC_FAIL:     return "Memory allocation failed";
        case DS_OUT_OF_BOUNDS:  return "Index out of bounds";
        case DS_INVALID_INPUT:  return "Invalid function input";
        default:                return "Unkown Error";
    }
}

void ds_default_error_callback(const ds_ErrorInfo *error) {
    printf("Error in %s:%d (%s): %s - %s\n",
            error->file_name,
            error->line_number,
            error->function_name,
            ds_error_string(error->error_code),
            error->message
            );
}

ds_String* ds_init_string(const char* literal) {
    if (!literal) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input String in NULL");
        return NULL;
    }

    ds_String* string = (ds_String*)malloc(sizeof(ds_String));
    if (!string) {
        DS_SET_ERROR(DS_ALLOC_FAIL, "Allocation from string failed");
        return NULL;
    }
    
    size_t lit_length = strlen(literal);
    
    // small enoguh for stack?
    if (lit_length <= DS_SMALL_STRING_CAPACITY) {
        memcpy(string->stack_data, literal, lit_length + 1);
        string->length = lit_length;
        string->capacity = DS_STACK_CAPACITY;
        ds_set_is_stack(string);
    }
    // on the heap
    else {
        string->heap_data = (char*)malloc(lit_length + 1);
        memcpy(string->heap_data, literal, lit_length + 1);
        string->length = lit_length;
        string->capacity = lit_length + 1;
        ds_set_is_heap(string);
    }

    return string;
}

void ds_free_string(ds_String* string)  {
    if (ds_is_heap(string)) {
        free(string->heap_data);
        string->heap_data = NULL;
    }
    free(string);
    string = NULL;
}

const char* ds_to_c_str(ds_String* string) {
   
    if (ds_has_valid_heap_data(string)) {
        return string->heap_data;
    }
    else {
        return string->stack_data;
    }
}

void ds_append_literal(ds_String *string, const char *literal) {

}

void ds_append_dstring(ds_String *string, ds_String *string1) {

}

void ds_append_char(ds_String *string, char c) {

}

char ds_pop(ds_String* string) {

}

#endif


