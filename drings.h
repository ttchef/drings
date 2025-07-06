
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
    DS_INVALID_LENGTH = -5,
} DS_RESULT;

typedef enum {
    DS_IS_HEAP = 0x1,
    DS_OWNS_MEM = 0x2,
    DS_READ_ONLY = 0x4,
    DS_STICKY_HEAP = 0x8,
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
ds_String*      ds_init_string(const char* string);
void            ds_free_string(ds_String* string);

// methods
void            ds_set(ds_String* string, const char* literal);
void            ds_clone(ds_String* string, ds_String* clone);
const char*     ds_to_c_str(ds_String* string);
void            ds_append(ds_String* string, const char* literal);
void            ds_append_dstring(ds_String* string, ds_String* append);

char            ds_pop(ds_String* string);
const char*     ds_pop_n(ds_String* string);
ds_String*      ds_pop_nds(ds_String* string);

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

static inline void ds_set_sticky_heap(ds_String* string) {
    string->flags |= DS_STICKY_HEAP;
}

static inline bool ds_has_valid_heap_data(const ds_String* string) {
    return (ds_is_heap(string) && string->heap_data);
}

static inline bool ds_has_sticky_heap(const ds_String* string) {
    return (string->flags & DS_STICKY_HEAP) != 0;
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

    ds_set_sticky_heap(string);

    return string;
}

void ds_free_string(ds_String* string)  {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return;
    }


    if (ds_is_heap(string)) {
        free(string->heap_data);
        string->heap_data = NULL;
    }
    free(string);
    string = NULL;
}

const char* ds_to_c_str(ds_String* string) {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return NULL;
    }
   
    if (ds_has_valid_heap_data(string)) {
        return string->heap_data;
    }
    else {
        return string->stack_data;
    }
}

void ds_append(ds_String *string, const char *literal) {
    if (!literal || !string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string in NULL");
        return;
    }

    size_t lit_length = strlen(literal);
    
    if (string->length + lit_length <= DS_SMALL_STRING_CAPACITY && ds_is_stack(string)) {
        memcpy(string->stack_data + string->length, literal, lit_length + 1);
        string->length += lit_length;
    }
    else if (ds_is_stack(string)) {
        char* heap_buffer = (char*)malloc(string->length + lit_length + 1);
        if (!heap_buffer) {
            DS_SET_ERROR(DS_ALLOC_FAIL, "Heap buffer allocation failed");
            return;
        }
        memcpy(heap_buffer, string->stack_data, string->length);
        memcpy(heap_buffer + string->length, literal, lit_length + 1);
        string->heap_data = heap_buffer;
        string->length += lit_length;
        string->capacity = string->length + 1;
        ds_set_is_heap(string);
    }
    else {
        while (string->capacity <= string->length + lit_length + 1) {
            string->capacity *= 2;
            char* heap_buffer = (char*)realloc(string->heap_data, string->capacity);
            if (!heap_buffer) {
                DS_SET_ERROR(DS_ALLOC_FAIL, "Failed heap buffer reallocation");
                return;
            }
            string->heap_data = heap_buffer;
        }
        memcpy(string->heap_data + string->length, literal, lit_length + 1);
        string->length += lit_length;
     }
}

void ds_append_dstring(ds_String *string, ds_String *append) {
    if (!string || !append) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string in NULL");
        return;
    }
    
    if (string->length + append->length <= DS_SMALL_STRING_CAPACITY && ds_is_stack(string) && ds_is_stack(append)) {
        memcpy(string->stack_data + string->length, append->stack_data, append->length + 1);
        string->length += append->length;
    }
    else if (ds_is_stack(string) && ds_is_stack(append)) {
        size_t new_length = string->length + append->length;
        char* heap_buffer = (char*)malloc(string->length + new_length + 1);
        if (!heap_buffer) {
            DS_SET_ERROR(DS_ALLOC_FAIL, "Failed heap buffer allocation");
            return;
        }
        memcpy(heap_buffer, string->stack_data, string->length);
        memcpy(heap_buffer + string->length, append->stack_data, append->length + 1);
        string->heap_data = heap_buffer;
        string->capacity = new_length + 1;
        string->length = new_length;
        ds_set_is_heap(string);
    }
    else if (ds_is_heap(string)) {
        size_t new_length = string->length + append->length;
        char* heap_buffer = NULL;
        while (string->capacity <= new_length + 1) {
            string->capacity *= 2;
            heap_buffer = (char*)realloc(string->heap_data, string->capacity);
            if (!heap_buffer) {
                DS_SET_ERROR(DS_ALLOC_FAIL, "Failed heap buffer reallocation");
                return;
            }
        }

        if (heap_buffer) {
            memcpy(heap_buffer, string->heap_data, string->length);
            string->heap_data = heap_buffer; 
        }

        if (ds_is_stack(append)) {
            memcpy(string->heap_data + string->length, append->stack_data, append->length + 1);
        }
        else {
            memcpy(string->heap_data + string->length, append->heap_data, append->length + 1);
        }
        string->length = new_length;
    }
}


char ds_pop(ds_String* string) {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return -1;
    }
    
    if (string->length == 0) {
        DS_SET_ERROR(DS_INVALID_LENGTH, "Length of the string going under 0");
        return -1;
    }
    
    char c = -1;

    if (ds_is_stack(string)) {
        c = string->stack_data[string->length - 1];
        string->stack_data[string->length - 1] = '\0';
        string->length--;
    }
    else if (ds_has_valid_heap_data(string) && ds_has_sticky_heap(string)) {
        c = string->heap_data[string->length - 1];
        string->heap_data[string->length - 1] = '\0';
        string->length--;
    }
    else if (ds_has_valid_heap_data(string)) {
        c = string->heap_data[string->length - 1];
        string->length--;

        if (string->length <= DS_SMALL_STRING_CAPACITY) {
            char* heap_buffer = string->heap_data;
            memcpy(string->stack_data, heap_buffer, string->length);
            string->stack_data[string->length + 1] = '\0';
        }
        else {
            string->heap_data[string->length + 1] = '\0';
        }
    }

    return c;

}

const char* ds_pop_n(ds_String* string) {

}

ds_String* ds_pop_nds(ds_String* string) {


}

#endif


