
#ifndef DRINGS_H
#define DRINGS_H 

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
    ds_StringView* views;
    uint32_t count;
    uint32_t capacity;
} ds_StringViewArray;

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

// string view
ds_StringView   ds_string_view_from_cstr(const char* str);
ds_StringView   ds_string_view_from_string(const ds_String* string);
ds_StringView   ds_string_view_from_buffer(const char* data, uint32_t length);
ds_StringView   ds_string_view_from_substr(const ds_StringView* view, uint32_t start, uint32_t length);
ds_StringView   ds_string_view_from_substr_to_end(const ds_StringView* view, uint32_t start);
ds_StringView   ds_string_view_from_string_substr(ds_String* string, uint32_t start, uint32_t length);
ds_StringView   ds_string_view_from_string_substr_to_end(ds_String* string, uint32_t start);

bool            ds_string_view_equal(const ds_StringView* view1, const ds_StringView* view2);
bool            ds_string_view_equal_cstr(const ds_StringView* view, const char* str);
bool            ds_string_view_is_empty(const ds_StringView* view);

int32_t         ds_string_view_find_char(const ds_StringView* view, char c);
int32_t         ds_string_view_find_substr(const ds_StringView* view, const ds_StringView* substr);

bool            ds_string_view_starts_with(const ds_StringView* view, const ds_StringView* prefix);
bool            ds_string_view_ends_with(const ds_StringView* view, const ds_String* suffix);

ds_String*      ds_string_from_view(const ds_StringView* view);

ds_StringView   ds_string_view_trim_whitespace(const ds_StringView* view);
void            ds_string_view_print(const ds_StringView* view, const char* label);

int32_t         ds_string_view_to_int(const ds_StringView* view, bool* success);
double          ds_string_view_to_double(const ds_StringView* view, bool* success);

ds_StringViewArray* ds_string_view_split(const ds_StringView* view, char split);
void            ds_free_string_view_array(ds_StringViewArray* array);

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

static inline char* ds_string_get_data(ds_String* string) {
    return ds_is_heap(string) ? string->heap_data : string->stack_data;
}

// private



#ifdef __cplusplus

#ifdef DS_NAMESPACE
} 
#endif

}
#endif

#endif // DRINGS_H

