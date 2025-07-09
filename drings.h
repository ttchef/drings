
#ifndef DRINGS_H
#define DRINGS_H 

//#define DRINGS_IMPL // Temp only for development
                    
#define DS_SMALL_STRING_CAPACITY 15
#define DS_STACK_CAPACITY 0

#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {

#ifdef DS_NAMESPACE
namespace ds { 
#endif

#endif

typedef enum {
    DS_UNDEFINIED = 1,
    DS_OK = 0,
    DS_ERROR = -1,
    DS_ALLOC_FAIL = -2,
    DS_OUT_OF_BOUNDS = -3,
    DS_INVALID_INPUT = -4,
    DS_INVALID_LENGTH = -5,
    DS_REACHED_UNINTENTIONAL_CONTROL_BLOCK = -6,
} DS_RESULT;

typedef enum {
    DS_IS_HEAP = 0x1,
    DS_OWNS_MEM = 0x2,
    DS_READ_ONLY = 0x4,
    DS_STICKY_HEAP = 0x8,
} DS_FLAG;

typedef enum {
    DS_FRONT = 0x1,
    DS_BACK = 0x2,
    DS_ALL = 0x4,
} DS_TRIM_FLAG;

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
const char*     ds_to_c_str(ds_String* string);
void            ds_append(ds_String* string, const char* literal);
void            ds_append_dstring(ds_String* string, ds_String* append);

char            ds_pop(ds_String* string);
size_t          ds_pop_n(ds_String* string, size_t n);

size_t          ds_reserve(ds_String* string, size_t n);
size_t          ds_clear(ds_String* string);
bool            ds_equal(ds_String* string0, ds_String* string1); // true if equal
size_t          ds_set(ds_String* string, const char* literal);
size_t          ds_clone(ds_String* string, ds_String* clone);
size_t          ds_trim_whitespace(ds_String* string);
size_t          ds_trim_whitespace_flags(ds_String* string, uint32_t flags);
ds_String*      ds_split(ds_String* string, char c); // returns split up string from first occurance


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

static inline size_t ds_move_dstring_to_heap(ds_String* string) {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input String is NULL");
        return -1;
    }

    string->capacity = string->length + 1;
    char* heap_buffer = (char*)malloc(string->capacity);
    if (!heap_buffer) {
        DS_SET_ERROR(DS_ALLOC_FAIL, "Couldnt allocate heap buffer");
        return -1;
    }

    memcpy(heap_buffer, string->stack_data, string->length + 1);
    string->heap_data = heap_buffer;
    ds_set_is_heap(string);

    return 0;
}

static inline size_t ds_move_dsstring_to_stack(ds_String* string) {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return -1;
    }

    if (string->length > DS_SMALL_STRING_CAPACITY) {
        DS_SET_ERROR(DS_INVALID_LENGTH, "Input string is to big for stack");
        return -1;
    }

    memcpy(string->stack_data, string->heap_data, string->length + 1);
    string->capacity = DS_STACK_CAPACITY;
    ds_set_is_stack(string);

    return 0;
}

// private



#ifdef __cplusplus

#ifdef DS_NAMESPACE
} 
#endif

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
        case DS_INVALID_LENGTH: return "Invalid Length";
        case DS_REACHED_UNINTENTIONAL_CONTROL_BLOCK: return "Unintentional control block reached";
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

        if (string->length <= DS_SMALL_STRING_CAPACITY) {
            char* heap_buffer = string->heap_data;
            memcpy(string->stack_data, heap_buffer, string->length);
            string->stack_data[string->length - 1] = '\0';
        }
        else {
            string->heap_data[string->length - 1] = '\0';
        }

        string->length--;
    }

    return c;

}

size_t ds_pop_n(ds_String* string, size_t n) {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input String is NULL");
        return -1;
    }

    if (((int32_t)string->length) - (int32_t)n < 0) {
        DS_SET_ERROR(DS_INVALID_LENGTH, "Length - n < 0");
        return -1;
    }

    // move to stack
    if ((ds_is_heap(string) && string->length <= DS_SMALL_STRING_CAPACITY && !ds_has_sticky_heap(string))
            || ds_is_stack(string)) {
        string->length -= n;
        
        if (ds_is_heap(string)) {
            string->heap_data[string->length] = '\0';
            ds_move_dsstring_to_stack(string);
        }
        else {
            string->stack_data[string->length] = '\0';
        }
    }
    else {
        string->length -= n;
        string->heap_data[string->length] = '\0';
    }
    
    return 0;
}

size_t ds_reserve(ds_String *string, size_t n) {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return -1;
    }
        
    if (ds_is_stack(string)) {
        ds_move_dstring_to_heap(string);
    }

    char* heap_buffer = (char*)realloc(string->heap_data, string->capacity + n);
    if (!heap_buffer) {
        DS_SET_ERROR(DS_ALLOC_FAIL, "Heap buffer reallocation failed");
        return -1;
    }

    memcpy(heap_buffer, string->heap_data, string->length + 1);
    string->heap_data = heap_buffer;
    string->capacity += n;

    return 0;
}

size_t ds_clear(ds_String* string) {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return -1;
    }

    if (ds_is_stack(string)) {
        string->stack_data[0] = '\0';
    }
    else {
        string->heap_data[0] = '\0';
    }

    string->length = 0;

    return 0;
}

bool ds_equal(ds_String* string0, ds_String* string1) {
    if (!string0 || !string1) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return false;
    }

    if (string0->length != string1->length) {
        return false;
    }

    if (ds_is_stack(string0) && ds_is_stack(string1)) {
        for (int i = 0; i < string0->length; i++) {
            if (string0->stack_data[i] != string1->stack_data[i]) return false;
        }
        return true;
    }

    else if (ds_is_stack(string0) && ds_is_heap(string1)) {
        for (int i = 0; i < string0->length; i++) {
            if (string0->stack_data[i] != string1->heap_data[i]) return false;
        }
        return true;
    }

    else if (ds_is_heap(string0) && ds_is_stack(string1)) {
        for (int i = 0; i < string0->length; i++) {
            if (string0->heap_data[i] != string1->stack_data[i]) return false;
        }
        return true;
    }

    else if (ds_is_heap(string0) && ds_is_heap(string1)) {
        for (int i = 0; i < string0->length; i++) {
            if (string0->heap_data[i] != string1->heap_data[i]) return false;
        }
        return true;
    }
    
    DS_SET_ERROR(DS_REACHED_UNINTENTIONAL_CONTROL_BLOCK, "Reached unententionally end of a function");
    return false;
}

size_t ds_set(ds_String* string, const char* literal) {
    if (!string || !literal) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return -1;
    }

    size_t lit_length = strlen(literal);

    if (lit_length <= DS_SMALL_STRING_CAPACITY) {
        // move to stack
        if (ds_is_heap(string) && !ds_has_sticky_heap(string)) {
            free(string->heap_data);
            string->heap_data = NULL;
            ds_set_is_stack(string);
            string->capacity = DS_STACK_CAPACITY;
        }
        memcpy(string->stack_data, literal, lit_length + 1);
    }
    else {
        if (ds_is_stack(string)) {
            ds_move_dstring_to_heap(string);
        }

        while (string->capacity <= lit_length + 1) {
            string->capacity *= 2;
            char* heap_buffer = (char*)realloc(string->heap_data, string->capacity);
            if (!heap_buffer) {
                DS_SET_ERROR(DS_ALLOC_FAIL, "Heap buffer reallocation failed");
                return -1;
            }
            string->heap_data = heap_buffer;
        }
        memcpy(string->heap_data, literal, lit_length + 1);
    }
    
    string->length = lit_length; 

    return 0;
}

/*  NOTE:
 *  this function can ignore flags of
 *  the string because it is cloning the 
 *  argument string with its flags
 */
size_t ds_clone(ds_String* string, ds_String* clone) {
    if (!string || !clone) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return -1;
    }

    if (ds_is_stack(string) && ds_is_stack(clone)) {
        memcpy(string->stack_data, clone->stack_data, clone->length + 1);
    }
    else if (ds_is_stack(string) && ds_is_heap(clone)) {
        ds_move_dstring_to_heap(string);
        if (string->capacity != clone->capacity) {
            char* heap_buffer = (char*)realloc(string->heap_data, clone->capacity);
            if (!heap_buffer) {
                DS_SET_ERROR(DS_ALLOC_FAIL, "Heap buffer reallocation failed");
                return -1;
            }
            string->heap_data = heap_buffer;
            string->capacity = clone->capacity;
        }
        memcpy(string->heap_data, clone->heap_data, clone->length + 1);
    }
    else if (ds_is_heap(string) && ds_is_stack(clone)) {
        ds_clear(string);
        ds_move_dsstring_to_stack(string);
        memcpy(string->stack_data, clone->stack_data, clone->length + 1);
    }
    else if (ds_is_heap(string) && ds_is_heap(clone)) {
        if (string->capacity != clone->capacity) {
            char* heap_buffer = (char*)realloc(string->heap_data, clone->capacity);
            if (!heap_buffer) {
                DS_SET_ERROR(DS_ALLOC_FAIL, "Heap buffer reallocation failed");
                return -1;
            }
            string->heap_data = heap_buffer;
            string->capacity = clone->capacity;
        }
        memcpy(string->heap_data, clone->heap_data, clone->length + 1);
    }

    string->length = clone->length;
    string->flags = clone->flags;

    return 0;
}

size_t ds_trim_whitespace(ds_String* string) {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return -1;
    }

    char* data = ds_is_heap(string) ? string->heap_data : string->stack_data;

    size_t write = 0;
    for (size_t read = 0; read < string->length; read++) {
        if (!isspace((unsigned char)data[read])) {
            data[write++] = data[read];
        }
    }

    data[write] = '\0';
    string->length = write;
    
    return 0;
}

size_t ds_trim_whitespace_flags(ds_String* string, uint32_t flags) {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return -1;
    }

    if (flags & DS_ALL) {
        return ds_trim_whitespace(string);
    }

    char* data = ds_is_heap(string) ? string->heap_data : string->stack_data;

    size_t start = 0, end = string->length;

    if (flags & DS_FRONT) {
        while (start < end && isspace((unsigned char)data[start])) {
            start++;
        }
    }

    if (flags & DS_BACK) {
        while (end > start && isspace((unsigned char)data[end - 1])) {
            end--;
        }
    }

    size_t delta_length = end - start;
    if (start > 0 && delta_length > 0) {
        memmove(data, data + start, delta_length);
    }
    data[delta_length] = '\0';
    string->length = delta_length;

    return 0;
}

ds_String* ds_split(ds_String* string, char seperator) {
    if (!string) {
        DS_SET_ERROR(DS_INVALID_INPUT, "Input string is NULL");
        return NULL;
    }

    char* data = ds_is_heap(string) ? string->heap_data : string->stack_data;

    size_t start;
    for (start = 0; start < string->length && data[start] != seperator; start++);
    if (start != string->length) {
        ds_String* cut = ds_init_string("");
        ds_clone(cut, string);
        char* c_data = ds_is_heap(cut) ? cut->heap_data : cut->stack_data;
        memcpy(c_data, data + start + 1, string->length - start + 1);
        data[start] = '\0';
        return cut;
    }

    return NULL;
}

#endif


