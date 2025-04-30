// json.h - A lightweight, single-header JSON library for C.
// Copyright (c) 2025 Kacper Fiedorowicz. All rights reserved.
// This software is licensed under the MIT License.
// See LICENSE for more information.

#ifndef JSON_H
#define JSON_H

#include <stddef.h>

#if defined(JSON_STATIC)
# define JSON_API static
#else
# define JSON_API extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JSON_ARRAY_INITIAL_CAPACITY
/**
 * Defines the initial capacity for JSON arrays.
 *
 * This value determines the number of elements a JSON array
 * can hold before requiring a reallocation. Adjust this value
 * to optimize memory usage and performance for your specific use case.
 */
# define JSON_ARRAY_INITIAL_CAPACITY 1
#endif

#ifndef JSON_ARRAY_CAPACITY_MULTIPLIER
/**
 * Defines the capacity multiplier for JSON arrays.
 *
 * When a JSON array exceeds its current capacity, its capacity is
 * multiplied by this value to allocate additional space. A higher
 * multiplier reduces the frequency of reallocations but increases
 * memory usage.
 */
# define JSON_ARRAY_CAPACITY_MULTIPLIER 2
#endif

#ifndef JSON_OBJECT_INITIAL_CAPACITY
/**
 * Defines the initial capacity for JSON objects.
 *
 * This value determines the number of key-value pairs a JSON object
 * can hold before requiring a reallocation. Adjust this value to
 * optimize memory usage and performance for your specific use case.
 */
# define JSON_OBJECT_INITIAL_CAPACITY 1
#endif

#ifndef JSON_OBJECT_CAPACITY_MULTIPLIER
/**
 * Defines the capacity multiplier for JSON objects.
 *
 * When a JSON object exceeds its current capacity, its capacity is
 * multiplied by this value to allocate additional space. A higher
 * multiplier reduces the frequency of reallocations but increases
 * memory usage.
 */
# define JSON_OBJECT_CAPACITY_MULTIPLIER 2
#endif

/**
 * JSON type definitions.
 *
 * These constants represent the various types of JSON values
 * that can be parsed or constructed using this library. Each
 * type is assigned a unique bit flag for efficient type checking.
 */
#define JSON_TYPE_NULL    1
#define JSON_TYPE_BOOLEAN 2
#define JSON_TYPE_NUMBER  3
#define JSON_TYPE_STRING  4
#define JSON_TYPE_ARRAY   5
#define JSON_TYPE_OBJECT  6

#define json_is_array(VALUE)   ((VALUE) && (VALUE)->type == JSON_TYPE_ARRAY)
#define json_is_boolean(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_BOOLEAN)
#define json_is_null(VALUE)    ((VALUE) && (VALUE)->type == JSON_TYPE_NULL)
#define json_is_number(VALUE)  ((VALUE) && (VALUE)->type == JSON_TYPE_NUMBER)
#define json_is_object(VALUE)  ((VALUE) && (VALUE)->type == JSON_TYPE_OBJECT)
#define json_is_string(VALUE)  ((VALUE) && (VALUE)->type == JSON_TYPE_STRING)

struct json_value
{
    int type;

    union
    {
        double number;
        struct
        {
            size_t capacity;
            size_t length;
            struct json_value **items;
        } array;
        struct
        {
            size_t capacity;
            size_t length;
            char *value;
        } string;
        struct
        {
            size_t capacity;
            size_t n_items;
            struct
            {
                char *key;
                struct json_value *value;
            } **items;
        } object;
    };
};

/**
 * Encodes a JSON value into a JSON string.
 *
 * This function takes a structured `json_value` and converts it into
 * a JSON-encoded string representation. The caller is responsible for
 * freeing the returned string using `free()`.
 *
 * @param value The JSON value to encode. Must not be NULL.
 * @return A pointer to the JSON-encoded string, or NULL if encoding fails.
 */
JSON_API char *json_encode(struct json_value *value);

/**
 * Decodes a JSON string into a JSON value.
 *
 * This function parses a JSON-encoded string and converts it into a
 * structured representation using the `json_value` type. The caller
 * is responsible for freeing the returned `json_value` using `json_free()`.
 *
 * @param json The JSON-encoded string to decode. Must be null-terminated.
 * @return A pointer to the decoded `json_value`, or NULL if decoding fails.
 */
JSON_API struct json_value *json_decode(const char *json);

/**
 * Creates a new JSON object.
 *
 * @return A pointer to the newly created JSON object, or NULL on failure.
 */
JSON_API struct json_value *json_object_new(void);

/**
 * Frees the memory associated with a JSON object.
 *
 * @param object The JSON object to free.
 */
JSON_API void json_object_free(struct json_value *object);

/**
 * Retrieves the value associated with a key in a JSON object.
 *
 * @param object The JSON object to query.
 * @param key The key to look up.
 * @return A pointer to the associated value, or NULL if the key does not exist.
 */
JSON_API struct json_value *json_object_get(struct json_value *object,
                                            const char *key);

/**
 * Sets a key-value pair in a JSON object.
 *
 * @param object The JSON object to modify.
 * @param key The key to set in the object.
 * @param value The value to associate with the key.
 * @return 0 on success, or -1 on failure.
 */
JSON_API int json_object_set(struct json_value *object, const char *key,
                             struct json_value *value);

/**
 * Checks if a key exists in a JSON object.
 *
 * @param object The JSON object to query.
 * @param key The key to check for.
 * @return 1 if the key exists, 0 otherwise.
 */
JSON_API int json_object_has(struct json_value *object, const char *key);

/**
 * Counts the number of key-value pairs in a JSON object.
 *
 * @param object The JSON object to query.
 * @return The number of key-value pairs in the object.
 */
JSON_API size_t json_object_count(struct json_value *object);

/**
 * Removes a key-value pair from a JSON object.
 *
 * @param object The JSON object to modify.
 * @param key The key to remove.
 * @return 0 on success, or -1 if the key does not exist.
 */
JSON_API int json_object_remove(struct json_value *object, const char *key);

/**
 * Iterates over the key-value pairs in a JSON object.
 *
 * @param object The JSON object to iterate over.
 * @param iter A pointer to the iteration index (should be initialized to 0).
 * @param key A pointer to store the current key.
 * @param value A pointer to store the current value.
 * @return 1 if there are more items to iterate, 0 otherwise.
 */
JSON_API int json_object_iter(const struct json_value *object, size_t *iter,
                              char **key, struct json_value **value);

/**
 * Sets a value at a specific index in a JSON array.
 *
 * @param array The JSON array to modify.
 * @param index The index to set the value at.
 * @param value The value to set.
 * @return 0 on success, or -1 on failure.
 */
JSON_API int json_array_set(struct json_value *array, size_t index,
                            struct json_value *value);

/**
 * Appends a value to the end of a JSON array.
 *
 * @param array The JSON array to modify.
 * @param value The value to append.
 * @return 0 on success, or -1 on failure.
 */
JSON_API int json_array_push(struct json_value *array,
                             struct json_value *value);

/**
 * Creates a new JSON array.
 *
 * @return A pointer to the newly created JSON array, or NULL on failure.
 */
JSON_API struct json_value *json_array_new(void);

/**
 * Frees the memory associated with a JSON array.
 *
 * @param array The JSON array to free.
 */
JSON_API void json_array_free(struct json_value *array);

/**
 * Initializes a JSON array.
 *
 * @param array The JSON array to initialize.
 */
JSON_API void json_array_init(struct json_value *array);

/**
 * Frees the memory associated with a JSON string.
 *
 * @param string The JSON string to free.
 */
JSON_API void json_string_free(struct json_value *string);

/**
 * Retrieves a value at a specific index in a JSON array.
 *
 * @param array The JSON array to query.
 * @param index The index to retrieve the value from.
 * @return A pointer to the value, or NULL if the index is out of bounds.
 */
JSON_API struct json_value *json_array_get(struct json_value *array,
                                           size_t index);

/**
 * Removes a value at a specific index in a JSON array.
 *
 * @param array The JSON array to modify.
 * @param index The index to remove the value from.
 * @return 0 on success, or -1 on failure.
 */
JSON_API int json_array_remove(struct json_value *array, size_t index);

/**
 * Retrieves the length of a JSON array.
 *
 * @param array The JSON array to query.
 * @return The number of elements in the array.
 */
JSON_API size_t json_array_length(struct json_value *array);

/**
 * Iterates over the elements in a JSON array.
 *
 * @param array The JSON array to iterate over.
 * @param index A pointer to the iteration index (should be initialized to 0).
 * @param value A pointer to store the current value.
 * @return 1 on success, or 0 if the iteration is complete.
 */
JSON_API int json_array_iter(struct json_value *array, size_t *index,
                             struct json_value **value);

/**
 * Creates a new JSON string with the specified value.
 *
 * @param value The string value to initialize the JSON string with.
 * @return A pointer to the newly created JSON string, or NULL on failure.
 */
JSON_API struct json_value *json_string_new(const char *value);

/**
 * Sets the value of a JSON string.
 *
 * @param string The JSON string to modify.
 * @param value The new string value to set.
 */
JSON_API void json_string_set(struct json_value *string, const char *value);

/**
 * Retrieves the string value from a JSON string.
 *
 * @param string The JSON string to query.
 * @return A pointer to the string value.
 */
JSON_API char *json_string_get(struct json_value *string);

/**
 * Creates a new JSON number with the specified value.
 *
 * @param value The numeric value to initialize the JSON number with.
 * @return A pointer to the newly created JSON number, or NULL on failure.
 */
JSON_API struct json_value *json_number_new(double value);

/**
 * Sets the numeric value of a JSON number.
 *
 * @param number The JSON number to modify.
 * @param value The new numeric value to set.
 */
JSON_API void json_number_set(struct json_value *number, double value);

/**
 * Retrieves the numeric value from a JSON number.
 *
 * @param number The JSON number to query.
 * @return The numeric value.
 */
JSON_API double json_number_get(struct json_value *number);

/**
 * Creates a new JSON boolean with the specified value.
 *
 * @param value The boolean value to initialize the JSON boolean with.
 * @return A pointer to the newly created JSON boolean, or NULL on failure.
 */
JSON_API struct json_value *json_boolean_new(int value);

/**
 * Sets the boolean value of a JSON boolean.
 *
 * @param boolean The JSON boolean to modify.
 * @param value The new boolean value to set.
 */
JSON_API void json_boolean_set(struct json_value *boolean, int value);

/**
 * Retrieves the boolean value from a JSON boolean.
 *
 * @param boolean The JSON boolean to query.
 * @return The boolean value.
 */
JSON_API int json_boolean_get(struct json_value *boolean);

/* PRIVATE */

#include <stdio.h>

struct json_parser
{
    const char *input;
    size_t length;
    size_t position;
#if defined(JSON_ERROR)
    struct
    {
        enum
        {
            JSON_ERROR_NONE,
            JSON_ERROR_SYNTAX,
            JSON_ERROR_MEMORY,
            JSON_ERROR_EOF
        } code;
        int line;
        const char *func;
        const char *message;
    } error;
#endif
};

/**
 * Initializes a JSON object.
 *
 * This function is responsible for setting up the internal structure
 * of a JSON object, ensuring it is ready for use. It is invoked by
 * both json_object_new() and json_object_free() to manage the lifecycle
 * of JSON objects.
 *
 * @param object A pointer to the JSON object to be initialized.
 *
 * @note This function is marked as privative and should not be used
 *       directly outside of the JSON library's internal implementation.
 */
JSON_API void json__object_init(struct json_value *object);

/**
 * Decodes a JSON value from a parser.
 *
 * @param parser The JSON parser to decode from.
 * @param value A pointer to store the decoded value.
 * @return 0 on success, or -1 on failure.
 */
JSON_API int json__decode_value(struct json_parser *parser,
                                struct json_value *value);

#include <stdlib.h>
#include <string.h>

#if defined(JSON_ALLOC)
static int random_alloc_fail = 0;
static int random_alloc_fail_odd = 3;

static uintptr_t all_allocations = 0;
static uintptr_t all_memory = 0;
static uintptr_t total_memory = 0;
static uintptr_t total_allocations = 0;
#endif

#if defined(JSON_VERBOSE)
# include <assert.h>
# include <stdarg.h>
# include <stdio.h>

void json_debug(const char *file, const char *func, int line,
                const char *format, ...)
{
    int n;
    va_list args;
    va_start(args, format);

    n = printf("json(\033[1mdebug\033[m): %s:%d in %s ", file, line, func);
    n += vprintf(format, args);

# if defined(JSON_ALLOC)
    while (120 - n > 0) {
        putchar(' ');
        n++;
    }
    printf(" [ mem: %3lu, all: %lu ]", total_memory, total_allocations);
# endif

    putchar('\n');
    va_end(args);
}

# define JSON_DEBUG(...) json_debug(__FILE__, __func__, __LINE__, __VA_ARGS__)

void json_memdump(void)
{
    JSON_DEBUG("Memory Dump: Total Memory Allocated = %lu bytes, Total "
               "Allocations = %lu",
               all_memory, all_allocations);
}

#else
# define JSON_DEBUG(...)
#endif

#if defined(JSON_ALLOC)
static void *json_alloc(size_t size)
{
    if (random_alloc_fail && rand() % random_alloc_fail_odd == 0)
        return NULL;

    void *raw_ptr = malloc(sizeof(uintptr_t) + size);
    if (!raw_ptr)
        return NULL;

    uintptr_t *ptr = (uintptr_t *) raw_ptr;
    *ptr = size;
    total_memory += size;
    all_memory += size;
    total_allocations += 1;
    all_allocations += 1;
    JSON_DEBUG("alloc(%zu) => %p", size, ptr + 1);
    return (void *) (ptr + 1);
}

static void *json_realloc(void *ptr, size_t size)
{
    JSON_DEBUG("realloc(%p, %zu)", ptr, size);

    if (!ptr)
        return json_alloc(size);

    uintptr_t *aligned_ptr = (uintptr_t *) ptr - 1;
    size_t old_size = *aligned_ptr;

    void *new_raw_ptr = malloc(sizeof(uintptr_t) + size);
    if (!new_raw_ptr)
        return NULL;

    uintptr_t *new_ptr = (uintptr_t *) new_raw_ptr;
    *new_ptr = size;

    memcpy((void *) (new_ptr + 1), (void *) (aligned_ptr + 1),
           old_size < size ? old_size : size);

    all_memory += size - old_size;
    total_memory += size - old_size;
    free(aligned_ptr);

    return (void *) (new_ptr + 1);
}

static char *json_strdup(const char *str)
{
    if (!str)
        return NULL;

    size_t len = strlen(str);
    char *new_str = json_alloc(len + 1);
    if (!new_str)
        return NULL;

    JSON_DEBUG("strdup(%p) => %p (%zu bytes)", str, new_str, len + 1);

    memcpy(new_str, str, len + 1);
    return new_str;
}

static void json_free(void *ptr)
{
    if (ptr) {
        JSON_DEBUG("json_free(%p)", ptr);
        uintptr_t *aligned_ptr = ((uintptr_t *) ptr) - 1;
        total_memory -= *aligned_ptr;
        free(aligned_ptr);
        total_allocations--;
        ptr = NULL;
    } else {
        JSON_DEBUG("json_free(NULL)");
    }
}
#endif

#ifdef JSON_ALLOC
# define json_alloc(size) JSON_ALLOC(size)
#else
# define json_alloc(size) malloc(size)
#endif

#ifdef JSON_REALLOC
# define json_realloc(ptr, size) JSON_REALLOC(ptr, size)
#else
# define json_realloc(ptr, size) realloc(ptr, size)
#endif

#ifdef JSON_STRDUP
# define json_strdup(str) JSON_STRDUP(str)
#else
# define json_strdup(str) strdup(str)
#endif

#ifdef JSON_FREE
# define json_free(ptr) JSON_FREE(ptr)
#else
# define json_free(ptr) free(ptr)
#endif

#if defined(JSON_ERROR)
# define JSON_PARSER_ERROR(PARSER, CODE, MESSAGE)                              \
     (PARSER)->error.code = (CODE);                                            \
     (PARSER)->error.line = __LINE__;                                          \
     (PARSER)->error.func = __func__;                                          \
     (PARSER)->error.message = (MESSAGE)
#else
# define JSON_PARSER_ERROR(PARSER, CODE, MESSAGE)
#endif

static void json__parse_whitespace(struct json_parser *parser)
{
    const char *input = parser->input;
    size_t pos = parser->position;
    size_t len = parser->length;

    while (pos < len
           && (input[pos] == ' ' || input[pos] == '\t' || input[pos] == '\n'
               || input[pos] == '\r')) {
        pos++;
    }

    parser->position = pos;
}

static int json_decode_string(struct json_parser *parser,
                              struct json_value *value)
{
    size_t start = parser->position + 1;
    size_t end = start;
    size_t buffer_size = 16;
    size_t buffer_length = 0;
    char *buffer = (char *) json_alloc(buffer_size);

    if (buffer == NULL) {
        JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY,
                          "Memory allocation failed");
        return -1;
    }

    while (end < parser->length && parser->input[end] != '"') {
        if (parser->input[end] == '\\') {
            end++;
            if (end >= parser->length) {
                json_free(buffer);
                JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
                                  "Unterminated escape sequence");
                return -1;
            }

            char escaped_char;
            switch (parser->input[end]) {
            case '"':
                escaped_char = '"';
                break;
            case '\\':
                escaped_char = '\\';
                break;
            case '/':
                escaped_char = '/';
                break;
            case 'b':
                escaped_char = '\b';
                break;
            case 'f':
                escaped_char = '\f';
                break;
            case 'n':
                escaped_char = '\n';
                break;
            case 'r':
                escaped_char = '\r';
                break;
            case 't':
                escaped_char = '\t';
                break;
            default:
                json_free(buffer);
                JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
                                  "Invalid escape character");
                return -1;
            }

            if (buffer_length + 1 >= buffer_size) {
                buffer_size *= 2;
                char *new_buffer = (char *) json_realloc(buffer, buffer_size);
                if (new_buffer == NULL) {
                    json_free(buffer);
                    JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY,
                                      "Memory allocation failed");
                    return -1;
                }
                buffer = new_buffer;
            }

            buffer[buffer_length++] = escaped_char;
        } else {
            if (buffer_length + 1 >= buffer_size) {
                buffer_size *= 2;
                char *new_buffer = (char *) json_realloc(buffer, buffer_size);
                if (new_buffer == NULL) {
                    json_free(buffer);
                    JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY,
                                      "Memory allocation failed");
                    return -1;
                }
                buffer = new_buffer;
            }

            buffer[buffer_length++] = parser->input[end];
        }
        end++;
    }

    if (end == parser->length) {
        json_free(buffer);
        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Unterminated string");
        return -1;
    }

    buffer[buffer_length] = '\0';

    value->type = JSON_TYPE_STRING;
    value->string.value = buffer;
    value->string.length = buffer_length;

    parser->position = end + 1;
    return 0;
}

// Consume digits in a number
static void json__parser_consume_digits(struct json_parser *parser)
{
    while (parser->position < parser->length
           && parser->input[parser->position] >= '0'
           && parser->input[parser->position] <= '9') {
        parser->position++;
    }
}

static int json_decode_number(struct json_parser *parser,
                              struct json_value *value)
{
    size_t start = parser->position;
    char *endptr;

    // Optional minus sign (no plus sign allowed in JSON)
    if (parser->position < parser->length
        && parser->input[parser->position] == '-') {
        parser->position++;
    }

    // Integer part
    if (parser->position < parser->length
        && parser->input[parser->position] == '0') {
        parser->position++;
    } else if (parser->position < parser->length
               && parser->input[parser->position] >= '1'
               && parser->input[parser->position] <= '9') {
        json__parser_consume_digits(parser);
    } else {
        JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER, "Invalid number");
        return -1; // Invalid number
    }

    // Fractional part
    if (parser->position < parser->length
        && parser->input[parser->position] == '.') {
        parser->position++;
        if (parser->position < parser->length
            && parser->input[parser->position] >= '0'
            && parser->input[parser->position] <= '9') {
            json__parser_consume_digits(parser);
        } else {
            JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER,
                              "Invalid number");
            return -1; // Must be at least one digit after '.'
        }
    }

    // Exponent part
    if (parser->position < parser->length
        && (parser->input[parser->position] == 'e'
            || parser->input[parser->position] == 'E')) {
        parser->position++;
        if (parser->position < parser->length
            && (parser->input[parser->position] == '-'
                || parser->input[parser->position] == '+')) {
            parser->position++;
        }
        if (parser->position < parser->length
            && parser->input[parser->position] >= '0'
            && parser->input[parser->position] <= '9') {
            json__parser_consume_digits(parser);
        } else {
            JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER,
                              "Invalid number");
            return -1; // Must be at least one digit after 'e' or 'E'
        }
    }

    // Final validation
    if (start == parser->position) {
        JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER, "Invalid number");
        return -1; // Nothing was parsed
    }

    value->type = JSON_TYPE_NUMBER;
    endptr = NULL;
    value->number = strtod(parser->input + start, &endptr);

    if (endptr != parser->input + parser->position) {
        JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER, "Invalid number");
        return -1; // strtod did not consume exactly what we parsed
    }

    return 0;
}

static int json_decode_array(struct json_parser *parser,
                             struct json_value *array)
{
    if (parser->length - parser->position < 1) {
        JSON_PARSER_ERROR(parser, JSON_ERROR_EOF, "Unexpected end of input");
        return -1;
    }

    if (parser->input[parser->position] != '[') {
        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected '['");
        return -1;
    }

    array->type = JSON_TYPE_ARRAY;
    json_array_init(array);

    if (parser->position + 1 < parser->length
        && parser->input[parser->position + 1] == ']') {
        parser->position += 2;
        return 0;
    }

    parser->position += 1; // Skip '['
    while (parser->position < parser->length
           && parser->input[parser->position] != ']') {
        struct json_value *item;
        json__parse_whitespace(parser);

        item = (struct json_value *) json_alloc(sizeof(struct json_value));
        if (item == NULL) {
            JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY,
                              "Memory allocation failed");
            json_array_free(array);
            return -1;
        }

        if (json__decode_value(parser, item) != 0) {
            json_free(item);
            json_array_free(array);
            return -1;
        }

        json_array_push(array, item);
        json__parse_whitespace(parser);

        if (parser->position < parser->length
            && parser->input[parser->position] == ',') {
            parser->position++;
        }
    }

    if (parser->position >= parser->length
        || parser->input[parser->position] != ']') {
        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
                          "Expected closing ']' for array");
        json_array_free(array);
        return -1;
    }

    parser->position++; // Skip ']'
    return 0;
}

static int json_decode_object(struct json_parser *parser,
                              struct json_value *object)
{
    object->type = JSON_TYPE_OBJECT;
    json__object_init(object);

    parser->position++;
    json__parse_whitespace(parser);

    while (parser->position < parser->length
           && parser->input[parser->position] != '}') {
        struct json_value key, *value;

        json__parse_whitespace(parser);

        if (parser->input[parser->position] != '"') {
            JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected string key");
            return -1;
        }

        if (json_decode_string(parser, &key) != 0) {
            JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
                              "Failed to parse string key");
            return -1;
        }

        json__parse_whitespace(parser);
        if (parser->position >= parser->length
            || parser->input[parser->position] != ':') {
            json_free(key.string.value);
            JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
                              "Expected ':' after string key");
            return -1;
        }
        parser->position++; // Skip the colon

        value = (struct json_value *) json_alloc(sizeof(struct json_value));
        if (value == NULL) {
            json_free(key.string.value);
            JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY,
                              "Failed to allocate memory for JSON item");
            return -1;
        }

        json__parse_whitespace(parser);
        if (json__decode_value(parser, value) != 0) {
            json_free(value);
            json_free(key.string.value);
            JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
                              "Failed to parse JSON value");
            return -1;
        }

        if (json_object_set(object, key.string.value, value) != 0) {
            json_free(value);
            json_free(key.string.value);
            JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY,
                              "Failed to set key-value pair in object");
            return -1;
        }

        json_free(key.string.value); // Free the key string as it's now
                                     // duplicated in the object

        json__parse_whitespace(parser);
        if (parser->position < parser->length
            && parser->input[parser->position] == ',') {
            parser->position++;
        }
    }

    if (parser->position >= parser->length
        || parser->input[parser->position] != '}') {
        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
                          "Expected '}' after JSON object");
        return -1;
    }

    parser->position++; // Skip the closing brace

    return 0;
}

int json__decode_value(struct json_parser *parser, struct json_value *value)
{
    int rc;
    char c = parser->input[parser->position];

    if (c == '"') {
        rc = json_decode_string(parser, value);
    } else if (c == '[') {
        rc = json_decode_array(parser, value);
    } else if (c == '{') {
        rc = json_decode_object(parser, value);
    } else if (!strcmp(parser->input + parser->position, "true")) {
        value->type = JSON_TYPE_BOOLEAN;
        value->number = 1;
        parser->position += 4;
        return 0;
    } else if (!strcmp(parser->input + parser->position, "false")) {
        value->type = JSON_TYPE_BOOLEAN;
        value->number = 0;
        parser->position += 5;
        return 0;
    } else if (!strcmp(parser->input + parser->position, "null")) {
        value->type = JSON_TYPE_NULL;
        parser->position += 4;
        return 0;
    } else {
        rc = json_decode_number(parser, value);
    }

#if defined(JSON_ERROR)
    if (rc != 0 && parser->error.code == JSON_ERROR_NONE) {
        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected value");
    }
#endif

    return rc;
}

struct json_value *json_decode(const char *json)
{
    struct json_parser parser;
    struct json_value *value = NULL;

    if (json == NULL) {
        return NULL;
    }

    parser.input = json;
    parser.length = strlen(json);
    parser.position = 0;
#if defined(JSON_ERROR)
    parser.error.code = JSON_ERROR_NONE;
    parser.error.message = NULL;
#endif

    value = (struct json_value *) json_alloc(sizeof(struct json_value));
    if (value == NULL) {
        return NULL;
    }

    if (json__decode_value(&parser, value) != 0) {
#if defined(JSON_ERROR) && !defined(JSON_ERROR_HANDLER)
        if (parser.error.code != JSON_ERROR_NONE) {
            fprintf(stderr, "JSON(\033[1merror\033[m): %s:%d %s\n",
                    parser.error.func, parser.error.line, parser.error.message);
        }
#elif defined(JSON_ERROR) && defined(JSON_ERROR_HANDLER)
        if (parser.error.code != JSON_ERROR_NONE) {
            JSON_ERROR_HANDLER(CODE, MESSAGE)
        }
#endif
        json_free(value);
        return NULL;
    }

    return value;
}

static char *json_encode_array(struct json_value *value)
{
    size_t length;
    size_t buffer_size;
    char *encoded_array;
    if (value == NULL) {
        return NULL;
    }

    buffer_size = 512;
    encoded_array = (char *) json_alloc(buffer_size);
    if (encoded_array == NULL) {
        return NULL;
    }

    length = 1;
    encoded_array[0] = '[';

    for (size_t i = 0; i < value->array.length; i++) {
        size_t needed;
        size_t encoded_item_len;
        char *encoded_item = json_encode(value->array.items[i]);
        if (encoded_item == NULL) {
            json_free(encoded_array);
            return NULL;
        }

        encoded_item_len = strlen(encoded_item);

        needed = length + encoded_item_len + (i > 0 ? 1 : 0) + 1;
        if (needed > buffer_size) {
            char *new_encoded_array = (char *) json_realloc(
                encoded_array, (buffer_size = needed * 2));
            if (new_encoded_array == NULL) {
                json_free(encoded_array);
                json_free(encoded_item);
                return NULL;
            }
            encoded_array = new_encoded_array;
        }

        if (i > 0) {
            encoded_array[length++] = ',';
        }

        memcpy(encoded_array + length, encoded_item, encoded_item_len);
        length += encoded_item_len;

        json_free(encoded_item);
    }

    encoded_array[length++] = ']';
    encoded_array[length] = '\0';

    return encoded_array;
}

static char *json_encode_object(struct json_value *object)
{
    size_t length;
    size_t buffer_size = 256;
    char *encoded_object = (char *) json_alloc(buffer_size);
    if (encoded_object == NULL) {
        return NULL;
    }

    encoded_object[0] = '{';
    length = 1;

    for (size_t i = 0; i < object->object.n_items; i++) {
        char *new_buffer;
        char *key = object->object.items[i]->key;
        char *value = json_encode(object->object.items[i]->value);

        size_t key_length = strlen(key);
        size_t value_length = strlen(value);

        size_t needed = length + key_length + value_length + 4
                      + (i < object->object.n_items - 1 ? 1 : 0);

        if (needed > buffer_size) {
            buffer_size = needed * 2;
            new_buffer = (char *) json_realloc(encoded_object, buffer_size);
            if (new_buffer == NULL) {
                json_free(encoded_object);
                json_free(value);
                return NULL;
            }
            encoded_object = new_buffer;
        }

        encoded_object[length++] = '"';
        memcpy(encoded_object + length, key, key_length);
        length += key_length;
        encoded_object[length++] = '"';
        encoded_object[length++] = ':';
        memcpy(encoded_object + length, value, value_length);
        length += value_length;

        json_free(value);

        if (i < object->object.n_items - 1) {
            encoded_object[length++] = ',';
        }
    }

    encoded_object[length++] = '}';
    encoded_object[length] = '\0';

    return encoded_object;
}

char *json_encode(struct json_value *value)
{
    if (value == NULL) {
        return NULL;
    }

    switch (value->type) {
    case JSON_TYPE_STRING: {
        size_t buffer_size = value->string.length + 3;
        char *buffer = json_alloc(buffer_size);
        if (buffer == NULL) {
            return NULL;
        }
        buffer[0] = '"';
        memcpy(buffer + 1, value->string.value, value->string.length);
        buffer[value->string.length + 1] = '"';
        buffer[value->string.length + 2] = 0; // Null-terminated string
        return buffer;
    }
    case JSON_TYPE_NUMBER: {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.17g", value->number);
        return json_strdup(buffer);
    }
    case JSON_TYPE_BOOLEAN:
        return json_strdup(value->number != 0.0 ? "true" : "false");
    case JSON_TYPE_NULL:
        return json_strdup("null");
    case JSON_TYPE_ARRAY:
        return json_encode_array(value);
    case JSON_TYPE_OBJECT:
        return json_encode_object(value);
    default:
        return NULL;
    }
}

#if defined(JSON_PRINT)

static void json_print_indent(int indent)
{
    for (int i = 0; i < indent; i++) {
        putchar(' ');
    }
}

static void json_print_internal(struct json_value *value, int indent)
{
    if (value == NULL) {
        printf("null");
        return;
    }

    switch (value->type) {
    case JSON_TYPE_STRING:
        printf("\"%s\"", value->string.value);
        break;
    case JSON_TYPE_NUMBER:
        printf("%g", value->number);
        break;
    case JSON_TYPE_BOOLEAN:
        printf("%s", value->number == 0.0 ? "false" : "true");
        break;
    case JSON_TYPE_NULL:
        printf("null");
        break;
    case JSON_TYPE_ARRAY:
        printf("[\n");
        for (size_t i = 0; i < value->array.length; i++) {
            json_print_indent(indent + 2);
            json_print_internal(value->array.items[i], indent + 2);
            if (i < value->array.length - 1) {
                printf(",\n");
            } else {
                printf("\n");
            }
        }
        json_print_indent(indent);
        printf("]");
        break;
    case JSON_TYPE_OBJECT:
        printf("{\n");
        for (size_t i = 0; i < value->object.n_items; i++) {
            json_print_indent(indent + 2);
            printf("\"%s\": ", value->object.items[i]->key);
            json_print_internal(value->object.items[i]->value, indent + 2);
            if (i < value->object.n_items - 1) {
                printf(",\n");
            } else {
                printf("\n");
            }
        }
        json_print_indent(indent);
        printf("}");
        break;
    default:
        break;
    }
}

static void json_print(struct json_value *value)
{
    json_print_internal(value, 0);
}

void json_println(struct json_value *value)
{
    json_print(value);
    putchar('\n');
}
#endif

void json__object_init(struct json_value *object)
{
    object->object.n_items = 0;
    object->object.capacity = JSON_OBJECT_INITIAL_CAPACITY;
    object->object.items = json_alloc(sizeof(struct json_object_item *)
                                      * JSON_OBJECT_INITIAL_CAPACITY);
}

struct json_value *json_object_new(void)
{
    struct json_value *object =
        (struct json_value *) json_alloc(sizeof(struct json_value));
    if (!object) {
        return NULL;
    }

    object->type = JSON_TYPE_OBJECT;
    json__object_init(object);

    return object;
}

void json_object_free(struct json_value *object)
{
    char *key;
    size_t iter = 0;
    struct json_value *value;

    while (json_object_iter(object, &iter, &key, &value)) {
        json_object_remove(object, key);
        iter--;
    }

    json_free(object->object.items);
    json_free(object);
}

int json_object_set(struct json_value *object, const char *key,
                    struct json_value *value)
{
    if (object->object.n_items == object->object.capacity) {
        size_t new_capacity =
            object->object.capacity
                ? object->object.capacity * JSON_OBJECT_CAPACITY_MULTIPLIER
                : JSON_OBJECT_INITIAL_CAPACITY;
        void *new_items = json_realloc(
            object->object.items, new_capacity * sizeof(*object->object.items));
        if (new_items == NULL) {
            return -1;
        }
        object->object.items = new_items;
        object->object.capacity = new_capacity;
    }

    size_t idx = object->object.n_items++;
    object->object.items[idx] = json_alloc(sizeof(*object->object.items[idx]));
    if (!object->object.items[idx])
        return -1;

    object->object.items[idx]->key = json_strdup(key);
    object->object.items[idx]->value = value;

    return 0;
}

int json_object_has(struct json_value *object, const char *key)
{
    for (size_t i = 0; i < object->object.n_items; i++) {
        if (strcmp(object->object.items[i]->key, key) == 0) {
            return 1;
        }
    }

    return 0;
}

struct json_value *json_object_get(struct json_value *object, const char *key)
{
    for (size_t i = 0; i < object->object.n_items; i++) {
        if (strcmp(object->object.items[i]->key, key) == 0) {
            return object->object.items[i]->value;
        }
    }

    return NULL;
}

int json_object_remove(struct json_value *object, const char *key)
{
    for (size_t i = 0; i < object->object.n_items; i++) {
        if (strcmp(object->object.items[i]->key, key) == 0) {
            switch (object->object.items[i]->value->type) {
            case JSON_TYPE_ARRAY:
                json_array_free(object->object.items[i]->value);
                break;
            case JSON_TYPE_OBJECT:
                json_object_free(object->object.items[i]->value);
                break;
            case JSON_TYPE_STRING:
                json_string_free(object->object.items[i]->value);
                break;
            default:
                json_free(object->object.items[i]->value);
                break;
            }

            json_free(object->object.items[i]->key);
            json_free(object->object.items[i]);

            if (i < object->object.n_items - 1) {
                object->object.items[i] =
                    object->object.items[object->object.n_items - 1];
            }

            object->object.n_items--;
            return 0;
        }
    }

    return 0;
}

int json_object_iter(const struct json_value *object, size_t *iter, char **key,
                     struct json_value **value)
{
    if (*iter >= object->object.n_items)
        return 0;

    *key = object->object.items[*iter]->key;
    *value = object->object.items[*iter]->value;
    (*iter)++;

    return 1;
}

void json_object_clear(struct json_value *object)
{
    char *key;
    size_t iter = 0;
    struct json_value *value;

    while (json_object_iter(object, &iter, &key, &value)) {
        json_object_remove(object, key);
        iter--;
    }
}

size_t json_object_count(struct json_value *object)
{
    return object->object.n_items;
}

void json_array_init(struct json_value *array)
{
    if (!array) {
        return;
    }

    array->array.length = 0;
    array->array.capacity = JSON_ARRAY_INITIAL_CAPACITY;
    array->array.items = (struct json_value **) json_alloc(
        sizeof(struct json_value *) * JSON_ARRAY_INITIAL_CAPACITY);
}

struct json_value *json_array_new(void)
{
    struct json_value *value =
        (struct json_value *) json_alloc(sizeof(struct json_value));
    if (!value) {
        return NULL;
    }

    value->type = JSON_TYPE_ARRAY;
    json_array_init(value);

    return value;
}

void json_array_free(struct json_value *value)
{
    for (size_t i = 0; i < value->array.length; i++) {
        switch (value->array.items[i]->type) {
        case JSON_TYPE_OBJECT:
            json_object_free(value->array.items[i]);
            break;
        case JSON_TYPE_ARRAY:
            json_array_free(value->array.items[i]);
            break;
        case JSON_TYPE_STRING:
            json_string_free(value->array.items[i]);
            break;
        default:
            json_free(value->array.items[i]);
            break;
        }
    }
    json_free(value->array.items);
    json_free(value);
}

JSON_API int json_array_set(struct json_value *array, size_t index,
                            struct json_value *value)
{
    if (!array) {
        return -1;
    }

    if ((size_t) index > array->array.length) {
        return -1;
    }

    if ((size_t) index == array->array.length) {
        struct json_value **new_items = (struct json_value **) json_realloc(
            array->array.items,
            ++array->array.length * sizeof(struct json_value *));
        if (!new_items) {
            array->array.length--;
            return -1;
        }
        array->array.items = new_items;
    }

    if (!value) {
        value = (struct json_value *) json_alloc(sizeof(struct json_value));
        if (!value) {
            array->array.length--;
            return -1;
        }
        value->type = JSON_TYPE_NULL;
    }

    array->array.items[index] = value;
    return 0;
}

int json_array_remove(struct json_value *array, size_t index)
{
    if (!array || array->type != JSON_TYPE_ARRAY
        || index >= array->array.length) {
        return -1;
    }

    switch (array->array.items[index]->type) {
    case JSON_TYPE_OBJECT:
        json_object_free(array->array.items[index]);
        break;
    case JSON_TYPE_ARRAY:
        json_array_free(array->array.items[index]);
        break;
    case JSON_TYPE_STRING:
        json_string_free(array->array.items[index]);
        break;
    default:
        break;
    }

    memmove(&array->array.items[index], &array->array.items[index + 1],
            (array->array.length - index - 1) * sizeof(struct json_value *));
    array->array.length--;

    return 0;
}

struct json_value *json_array_get(struct json_value *array, size_t index)
{
    if (!array || array->type != JSON_TYPE_ARRAY
        || index >= array->array.length) {
        return NULL;
    }

    return array->array.items[index];
}

int json_array_push(struct json_value *array, struct json_value *value)
{
    if (!array || array->type != JSON_TYPE_ARRAY) {
        return -1;
    }

    if (!value) {
        return -1;
    }

    if (array->array.length == array->array.capacity) {
        size_t new_capacity =
            array->array.capacity
                ? array->array.capacity * JSON_ARRAY_CAPACITY_MULTIPLIER
                : JSON_ARRAY_INITIAL_CAPACITY;
        struct json_value **new_items = (struct json_value **) json_realloc(
            array->array.items, new_capacity * sizeof(struct json_value *));
        if (!new_items) {
            return -1;
        }

        array->array.items = new_items;
        array->array.capacity = new_capacity;
    }

    array->array.items[array->array.length++] = value;
    return 0;
}

int json_array_iter(struct json_value *array, size_t *index,
                    struct json_value **value)
{
    if (*index >= array->array.length) {
        return 0;
    }

    *value = array->array.items[*index];
    (*index)++;

    return 1;
}

void json_array_clear(struct json_value *array)
{
    size_t iter = 0;
    struct json_value *value;

    while (json_array_iter(array, &iter, &value)) {
        json_array_remove(array, iter);
        iter--;
    }
}

size_t json_array_length(struct json_value *array)
{
    return array->array.length;
}

struct json_value *json_string_new(const char *value)
{
    struct json_value *string =
        (struct json_value *) json_alloc(sizeof(struct json_value));
    if (!string) {
        return NULL;
    }

    string->type = JSON_TYPE_STRING;
    string->string.length = strlen(value);
    string->string.value = json_strdup(value);
    if (!string->string.value) {
        json_free(string);
        return NULL;
    }

    strcpy(string->string.value, value);

    return string;
}

struct json_value *json_number_new(double value)
{
    struct json_value *number =
        (struct json_value *) json_alloc(sizeof(struct json_value));
    if (!number) {
        return NULL;
    }

    number->type = JSON_TYPE_NUMBER;
    number->number = value;

    return number;
}

struct json_value *json_boolean_new(int value)
{
    struct json_value *boolean =
        (struct json_value *) json_alloc(sizeof(struct json_value));
    if (!boolean) {
        return NULL;
    }

    boolean->type = JSON_TYPE_BOOLEAN;
    boolean->number = value;

    return boolean;
}

inline int json_boolean_get(struct json_value *boolean)
{
    return boolean->number != 0.0;
}

inline void json_boolean_set(struct json_value *boolean, int value)
{
    boolean->number = value;
}

inline double json_number_get(struct json_value *number)
{
    return number->number;
}

inline void json_number_set(struct json_value *number, double value)
{
    number->number = value;
}

inline char *json_string_get(struct json_value *string)
{
    return string->string.value;
}

void json_string_set(struct json_value *string, const char *value)
{
    size_t new_length = strlen(value);
    if (string->string.value && string->string.capacity >= new_length + 1) {
        string->string.length = new_length;
        strcpy(string->string.value, value);
    } else {
        free(string->string.value);
        string->string.length = new_length;
        string->string.value = (char *) json_alloc(new_length + 1);
        if (!string->string.value) {
            return;
        }
        string->string.capacity = new_length + 1;
        strcpy(string->string.value, value);
    }
}

void json_string_free(struct json_value *string)
{
    json_free(string->string.value);
    json_free(string);
}

#ifdef __cplusplus
}
#endif

#endif /* JSON_H */
