// json.h - A lightweight, single-header JSON library for C.
// Copyright (c) 2025 Kacper Fiedorowicz. All rights reserved.
// This software is licensed under the MIT License.
// See LICENSE for more information.

#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(JSON_STATIC)
/**
 * Defines the linkage of JSON API functions.
 *
 * If `JSON_STATIC` is defined, the functions are declared as `static`,
 * meaning they are only visible within the translation unit where they
 * are defined. Otherwise, they are declared as `extern`, making them
 * visible across translation units.
 */
# define JSON_API static
#else
# define JSON_API extern
#endif

#ifdef JSON_ALLOC
/**
 * Allocates memory for JSON operations.
 *
 * If `JSON_ALLOC` is defined, it uses the custom allocation function
 * provided by the user. Otherwise, it defaults to the standard `malloc`.
 *
 * @param size The size of memory to allocate in bytes.
 * @return A pointer to the allocated memory, or NULL if allocation fails.
 */
# define json_alloc(size) JSON_ALLOC(size)
#else
# define json_alloc(size) malloc(size)
#endif

#ifdef JSON_REALLOC
/**
 * Reallocates memory for JSON operations.
 *
 * If `JSON_REALLOC` is defined, it uses the custom reallocation function
 * provided by the user. Otherwise, it defaults to the standard `realloc`.
 *
 * @param ptr A pointer to the previously allocated memory.
 * @param size The new size of memory to allocate in bytes.
 * @return A pointer to the reallocated memory, or NULL if reallocation fails.
 */
# define json_realloc(ptr, size) JSON_REALLOC(ptr, size)
#else
# define json_realloc(ptr, size) realloc(ptr, size)
#endif

#ifdef JSON_FREE
/**
 * Frees memory allocated for JSON operations.
 *
 * If `JSON_FREE` is defined, it uses the custom free function provided by
 * the user. Otherwise, it defaults to the standard `free`.
 *
 * @param ptr A pointer to the memory to free.
 */
# define json__free(ptr) JSON_FREE(ptr)
#else
# define json__free(ptr) free(ptr)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JSON_ARRAY_INITIAL_CAPACITY
/**
 * @brief Defines the initial capacity for JSON arrays.
 *
 * This value determines the number of elements a JSON array
 * can hold before requiring a reallocation. Adjust this value
 * to optimize memory usage and performance for your specific use case.
 */
# define JSON_ARRAY_INITIAL_CAPACITY 1
#endif

#ifndef JSON_ARRAY_CAPACITY_MULTIPLIER
/**
 * @brief Defines the capacity multiplier for JSON arrays.
 *
 * When a JSON array exceeds its current capacity, its capacity is
 * multiplied by this value to allocate additional space. A higher
 * multiplier reduces the frequency of reallocations but increases
 * memory usage.
 */
# define JSON_ARRAY_CAPACITY_MULTIPLIER 2
#endif

#ifndef JSON_ARRAY_CAPACITY_THRESHOLD
/**
 * @brief Threshold for triggering reallocation in JSON arrays.
 *
 * Defines how full the array must be (relative to capacity)
 * before reallocating. Typically set to 1 (100% full).
 */
# define JSON_ARRAY_CAPACITY_THRESHOLD 1
#endif

#ifndef JSON_OBJECT_INITIAL_CAPACITY
/**
 * @brief Defines the initial capacity for JSON objects.
 *
 * Determines how many key-value pairs a JSON object can store
 * before requiring reallocation. Increase this value to reduce
 * early reallocations for large objects.
 */
# define JSON_OBJECT_INITIAL_CAPACITY 1
#endif

#ifndef JSON_OBJECT_CAPACITY_MULTIPLIER
/**
 * @brief Growth multiplier for JSON object capacity.
 *
 * When the object exceeds its current capacity, the capacity
 * is multiplied by this factor. A larger multiplier reduces
 * the frequency of reallocations but may waste memory.
 */
# define JSON_OBJECT_CAPACITY_MULTIPLIER 2
#endif

#ifndef JSON_OBJECT_CAPACITY_THRESHOLD
/**
 * @brief Threshold for triggering reallocation in JSON objects.
 *
 * Defines how full the object must be (relative to capacity)
 * before reallocating. Typically set to 1 (100% full).
 */
# define JSON_OBJECT_CAPACITY_THRESHOLD 1
#endif

/**
 * @brief Represents a JSON value.
 *
 * This structure encapsulates a JSON value, which can be one of several types
 * including null, boolean, number, string, array, or object. The type of the
 * value is determined by the `type` field, and the actual data is stored in
 * the corresponding member of the union.
 */
struct json_value
{
    /**
     * @brief Enum representing the type of the JSON value.
     *
     * The type determines which member of the union is valid.
     *
     * - `JSON_TYPE_NULL`: Represents a null value.
     * - `JSON_TYPE_BOOLEAN`: Represents a boolean value (true/false).
     * - `JSON_TYPE_NUMBER`: Represents a numeric value.
     * - `JSON_TYPE_STRING`: Represents a string value.
     * - `JSON_TYPE_ARRAY`: Represents an array of JSON values.
     * - `JSON_TYPE_OBJECT`: Represents an object with key-value pairs.
     */
    enum
    {
        JSON_TYPE_NULL,    /**< Null value. */
        JSON_TYPE_BOOLEAN, /**< Boolean value (true/false). */
        JSON_TYPE_NUMBER,  /**< Numeric value. */
        JSON_TYPE_STRING,  /**< String value. */
        JSON_TYPE_ARRAY,   /**< Array of JSON values. */
        JSON_TYPE_OBJECT   /**< Object with key-value pairs. */
    } type;

    /**
     * @brief Union holding the actual data of the JSON value.
     *
     * The type of data stored depends on the `type` field.
     */
    union
    {
        /**
         * @brief Holds a numeric value when `type` is `JSON_TYPE_NUMBER`.
         */
        double number;

        /**
         * @brief Holds an array of JSON values when `type` is
         * `JSON_TYPE_ARRAY`.
         *
         * - `capacity`: The total allocated capacity of the array.
         * - `length`: The current number of elements in the array.
         * - `items`: A pointer to an array of pointers to `json_value`
         * elements.
         */
        struct
        {
            int capacity;              /**< Total allocated capacity of the array. */
            int length;                /**< Current number of elements in the array. */
            struct json_value **items; /**< Pointer to an array of pointers to
                                          `json_value` elements. */
        } array;

        /**
         * @brief Holds a string value when `type` is `JSON_TYPE_STRING`.
         *
         * - `capacity`: The total allocated capacity of the string.
         * - `length`: The current length of the string (excluding null
         * terminator).
         * - `value`: A pointer to the null-terminated string.
         */
        struct
        {
            int capacity; /**< Total allocated capacity of the string. */
            int length;   /**< Current length of the string (excluding null
                             terminator). */
            char *value;  /**< Pointer to the null-terminated string. */
        } string;

        /**
         * @brief Holds an object with key-value pairs when `type` is
         * `JSON_TYPE_OBJECT`.
         *
         * - `capacity`: The total allocated capacity for key-value pairs.
         * - `n_items`: The current number of key-value pairs in the object.
         * - `items`: A pointer to an array of pointers to key-value pair
         * structures. Each key-value pair consists of:
         *     - `key`: A pointer to a null-terminated string representing the
         * key.
         *     - `value`: A pointer to a `json_value` representing the
         * associated value.
         */
        struct
        {
            int capacity; /**< Total allocated capacity for key-value pairs. */
            int n_items;  /**< Current number of key-value pairs in the object.
                           */
            struct
            {
                char *key;                /**< Pointer to a null-terminated string representing
                                             the key. */
                struct json_value *value; /**< Pointer to a `json_value` representing
                                             the associated value. */
            } **items;                    /**< Pointer to an array of pointers to key-value pair
                                             structures. */
        } object;
    };
};

// TODO: make these macros evaluate `VALUE` once

/**
 * @brief Checks if a JSON value is an array.
 *
 * This macro determines whether a given JSON value is of type array.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is an array, 0 otherwise.
 */
#define json_is_array(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_ARRAY)

/**
 * @brief Checks if a JSON value is a boolean.
 *
 * This macro determines whether a given JSON value is of type boolean.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is a boolean, 0 otherwise.
 */
#define json_is_boolean(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_BOOLEAN)

/**
 * @brief Checks if a JSON value is null.
 *
 * This macro determines whether a given JSON value is of type null.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is null, 0 otherwise.
 */
#define json_is_null(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_NULL)

/**
 * @brief Checks if a JSON value is a number.
 *
 * This macro determines whether a given JSON value is of type number.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is a number, 0 otherwise.
 */
#define json_is_number(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_NUMBER)

/**
 * @brief Checks if a JSON value is an object.
 *
 * This macro determines whether a given JSON value is of type object.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is an object, 0 otherwise.
 */
#define json_is_object(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_OBJECT)

/**
 * @brief Checks if a JSON value is a string.
 *
 * This macro determines whether a given JSON value is of type string.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is a string, 0 otherwise.
 */
#define json_is_string(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_STRING)

/**
 * @brief Retrieves the boolean value from a JSON boolean.
 *
 * This macro extracts the boolean value from a JSON boolean type.
 *
 * @param VALUE The JSON boolean value to retrieve.
 * @return Non-zero if the boolean is true, 0 if false.
 */
#define json_boolean_get(VALUE) ((VALUE)->number != 0.0)

/**
 * @brief Sets the boolean value of a JSON boolean.
 *
 * This macro modifies the boolean value of a JSON boolean type.
 *
 * @param VALUE The JSON boolean value to modify.
 * @param STATE The new boolean value to set (non-zero for true, 0 for false).
 */
#define json_boolean_set(VALUE, STATE) ((VALUE)->number = STATE)

/**
 * @brief Retrieves the numeric value from a JSON number.
 *
 * This macro extracts the numeric value from a JSON number type.
 *
 * @param VALUE The JSON number value to retrieve.
 * @return The numeric value.
 */
#define json_number_get(VALUE) ((VALUE)->number)

/**
 * @brief Sets the numeric value of a JSON number.
 *
 * This macro modifies the numeric value of a JSON number type.
 *
 * @param JSON The JSON number value to modify.
 * @param VALUE The new numeric value to set.
 */
#define json_number_set(JSON, VALUE) ((JSON)->number = VALUE)

/**
 * @brief Retrieves the string value from a JSON string.
 *
 * This macro extracts the string value from a JSON string type.
 *
 * @param JSON The JSON string value to retrieve.
 * @return A pointer to the string value.
 */
#define json_string_get(JSON) ((JSON)->string.value)

/**
 * @brief Sets the string value of a JSON string.
 *
 * This macro modifies the string value of a JSON string type.
 *
 * @param JSON The JSON string value to modify.
 * @param VALUE The new string value to set.
 */
#define json_string_set(JSON, VALUE) ((JSON)->string.value = VALUE)

/**
 * @brief Retrieves an element from a JSON array.
 *
 * This function retrieves an element from a JSON array at the specified index.
 *
 * @param array The JSON array to retrieve from.
 * @param index The index of the element to retrieve.
 * @return A pointer to the element at the specified index, or NULL if out of
 * bounds.
 */
JSON_API struct json_value *json_array_get(struct json_value *array, int index);

inline struct json_value *json_array_get(struct json_value *array, int index)
{
    return index < array->array.length ? array->array.items[index] : NULL;
}

/**
 * @brief Sets an element in a JSON array.
 *
 * This macro sets an element in a JSON array at the specified index.
 *
 * @param array The JSON array to modify.
 * @param index The index of the element to set.
 * @param value The new value to set.
 */
#define json_array_set(ARRAY, INDEX, VALUE) ((ARRAY)->array.items[(INDEX)] = (VALUE))

/**
 * @brief Retrieves the number of key-value pairs in a JSON object.
 *
 * This macro returns the total count of key-value pairs in a JSON object.
 *
 * @param OBJECT A pointer to the JSON object to query.
 * @return The total count of key-value pairs in the object.
 */
#define json_object_count(OBJECT) ((OBJECT)->object.n_items)

/**
 * @brief Retrieves the number of elements in a JSON array.
 *
 * This macro returns the total count of elements in a JSON array.
 *
 * @param ARRAY A pointer to the JSON array to query.
 * @return The total count of elements in the array.
 */
#define json_array_count(ARRAY) ((ARRAY)->array.length)

/**
 * @brief Encodes a JSON value into a JSON string.
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
 * @brief Decodes a JSON string into a JSON value.
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
 * @brief Decodes a JSON string into a JSON value.
 *
 * Decodes up to `length` bytes, see `json_decode` for more.
 *
 * @param json The JSON-encoded string to decode.
 * @param length The length of the string to decode.
 * @return A pointer to the decoded `json_value`, or NULL if decoding fails.
 */
JSON_API struct json_value *json_decode_with_length(const char *json, int length);

/**
 * @brief Creates a new JSON object.
 *
 * This function allocates and initializes a new JSON object.
 *
 * @return A pointer to the newly created JSON object, or NULL on failure.
 */
JSON_API struct json_value *json_object_new(void);

/**
 * @brief Frees the memory associated with a JSON object.
 *
 * This function releases all memory used by the JSON object, including
 * its key-value pairs and their associated values.
 *
 * @param object The JSON object to free.
 */
JSON_API void json_object_free(struct json_value *object);

/**
 * @brief Retrieves the value associated with a key in a JSON object.
 *
 * This function searches for a key in the JSON object and returns the
 * associated value if the key exists.
 *
 * @param object The JSON object to query.
 * @param key The key to look up.
 * @return A pointer to the associated value, or NULL if the key does not exist.
 */
JSON_API struct json_value *json_object_get(struct json_value *object, const char *key);

/**
 * @brief Sets a key-value pair in a JSON object.
 *
 * This function adds or updates a key-value pair in the JSON object.
 * If the key already exists, its value is updated.
 *
 * @param object The JSON object to modify.
 * @param key The key to set in the object.
 * @param value The value to associate with the key.
 * @return 0 on success, or -1 on failure.
 */
JSON_API int json_object_set(struct json_value *object, const char *key, struct json_value *value);

/**
 * @brief Checks if a key exists in a JSON object.
 *
 * This function determines whether a specific key exists in the JSON object.
 *
 * @param object The JSON object to query.
 * @param key The key to check for.
 * @return 1 if the key exists, 0 otherwise.
 */
JSON_API int json_object_has(struct json_value *object, const char *key);

/**
 * @brief Removes a key-value pair from a JSON object.
 *
 * This function deletes a key-value pair from the JSON object. If the key
 * does not exist, the function does nothing.
 *
 * @param object The JSON object to modify.
 * @param key The key to remove.
 */
JSON_API void json_object_remove(struct json_value *object, const char *key);

/**
 * @brief Iterates over the key-value pairs in a JSON object.
 *
 * This function provides a way to iterate through all key-value pairs
 * in a JSON object. The iteration index should be initialized to 0
 * before the first call.
 *
 * @param object The JSON object to iterate over.
 * @param iter A pointer to the iteration index (should be initialized to 0).
 * @param key A pointer to store the current key.
 * @param value A pointer to store the current value.
 * @return 1 if there are more items to iterate, 0 otherwise.
 */
JSON_API int json_object_iter(const struct json_value *object, int *iter, char **key, struct json_value **value);

/**
 * @brief Appends a value to the end of a JSON array.
 *
 * This function adds a new value to the end of the JSON array.
 *
 * @param array The JSON array to modify.
 * @param value The value to append.
 * @return 0 on success, or -1 on failure.
 */
JSON_API int json_array_push(struct json_value *array, struct json_value *value);

/**
 * @brief Creates a new JSON array.
 *
 * This function allocates and initializes a new JSON array.
 *
 * @return A pointer to the newly created JSON array, or NULL on failure.
 */
JSON_API struct json_value *json_array_new(void);

/**
 * @brief Frees the memory associated with a JSON array.
 *
 * This function releases all memory used by the JSON array, including
 * its elements.
 *
 * @param array The JSON array to free.
 */
JSON_API void json_array_free(struct json_value *array);

/**
 * @brief Initializes a JSON array.
 *
 * This function sets up the internal structure of a JSON array,
 * preparing it for use.
 *
 * @param array The JSON array to initialize.
 */
JSON_API void json_array_init(struct json_value *array);

/**
 * @brief Frees the memory associated with a JSON string.
 *
 * This function releases all memory used by the JSON string.
 *
 * @param string The JSON string to free.
 */
JSON_API void json_string_free(struct json_value *string);

/**
 * @brief Removes a value at a specific index in a JSON array.
 *
 * This function deletes the value at the specified index in the JSON array.
 * If the index is out of bounds, the function fails.
 *
 * @param array The JSON array to modify.
 * @param index The index to remove the value from.
 */
JSON_API void json_array_remove(struct json_value *array, int index);

/**
 * @brief Retrieves the length of a JSON array.
 *
 * This function returns the number of elements in the JSON array.
 *
 * @param array The JSON array to query.
 * @return The number of elements in the array.
 */
JSON_API int json_array_length(struct json_value *array);

/**
 * @brief Iterates over the elements in a JSON array.
 *
 * This function provides a way to iterate through all elements in a JSON array.
 * The iteration index should be initialized to 0 before the first call.
 *
 * @param array The JSON array to iterate over.
 * @param index A pointer to the iteration index (should be initialized to 0).
 * @param value A pointer to store the current value.
 * @return 1 on success, or 0 if the iteration is complete.
 */
JSON_API int json_array_iter(struct json_value *array, int *index, struct json_value **value);

/**
 * @brief Creates a new JSON string with the specified value.
 *
 * This function allocates and initializes a new JSON string with the
 * provided string value.
 *
 * @param value The string value to initialize the JSON string with.
 * @return A pointer to the newly created JSON string, or NULL on failure.
 */
JSON_API struct json_value *json_string_new(const char *value);

/**
 * @brief Creates a new JSON number with the specified value.
 *
 * This function allocates and initializes a new JSON number with the
 * provided numeric value.
 *
 * @param value The numeric value to initialize the JSON number with.
 * @return A pointer to the newly created JSON number, or NULL on failure.
 */
JSON_API struct json_value *json_number_new(double value);

/**
 * @brief Creates a new JSON boolean with the specified value.
 *
 * This function allocates and initializes a new JSON boolean with the
 * provided boolean value.
 *
 * @param value The boolean value to initialize the JSON boolean with.
 * @return A pointer to the newly created JSON boolean, or NULL on failure.
 */
JSON_API struct json_value *json_boolean_new(int value);

/**
 * @brief Deallocates memory associated with a JSON value and its descendants.
 *
 * This function performs a comprehensive cleanup of the specified JSON value,
 * including recursively freeing all child elements, whether they are objects,
 * arrays, or strings. It ensures that all allocated memory is properly released.
 *
 * @param value A pointer to the JSON value to be deallocated.
 */
JSON_API void json_free(struct json_value *value);

/**
 * @brief Copy the whole value and all its children into a new json_value.
 *
 * In theory, no memory should be shared with the previous value.
 *
 * @param value The JSON value to deep-copy.
 */
JSON_API struct json_value *json_deep_copy(struct json_value *value);

/**
 * @brief Represents a JSON parser for decoding JSON strings.
 *
 * This structure is used internally by the JSON library to parse
 * JSON-encoded strings into structured `json_value` objects.
 *
 * @note This structure is private and should not be accessed directly
 *       outside of the JSON library's internal implementation.
 */
struct json_parser
{
    /**
     * @brief The input JSON string to be parsed.
     *
     * This is a pointer to the null-terminated JSON string that the parser
     * operates on.
     */
    const char *input;

    /**
     * @brief The length of the input JSON string.
     *
     * This value represents the total number of characters in the input
     * JSON string, excluding the null terminator.
     */
    int length;

    /**
     * @brief The current position in the input JSON string.
     *
     * This value indicates the index of the character currently being
     * processed by the parser.
     */
    int position;

#if defined(JSON_ERROR)
    /**
     * @brief Represents error information for the JSON parser.
     *
     * This structure is used to store details about any errors encountered
     * during the parsing process.
     */
    struct
    {
        /**
         * @brief Enum representing the type of error encountered.
         *
         * - `JSON_ERROR_NONE`: No error occurred.
         * - `JSON_ERROR_SYNTAX`: A syntax error was encountered.
         * - `JSON_ERROR_MEMORY`: A memory allocation error occurred.
         * - `JSON_ERROR_EOF`: Unexpected end of input was encountered.
         */
        enum
        {
            JSON_ERROR_NONE,           /**< No error occurred. */
            JSON_ERROR_SYNTAX,         /**< A syntax error was encountered. */
            JSON_ERROR_MEMORY,         /**< A memory allocation error occurred. */
            JSON_ERROR_EOF,            /**< Unexpected end of input was encountered. */
            JSON_ERROR_INVALID_NUMBER, /**< Failed to parse number */
        } code;

        /**
         * @brief The line number where the error occurred.
         *
         * This value is used for debugging purposes to identify the location
         * of the error in the source code.
         */
        int line;

        /**
         * @brief The name of the function where the error occurred.
         *
         * This is a pointer to a string containing the name of the function
         * in which the error was detected.
         */
        const char *func;

        /**
         * @brief A descriptive message about the error.
         *
         * This is a pointer to a string containing a human-readable
         * description of the error.
         */
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
JSON_API void json_object_init(struct json_value *object);

#if defined(JSON_ERROR)
/**
 * @brief Sets an error in the JSON parser.
 *
 * This macro assigns error details to the JSON parser's error structure.
 * It captures the error code, the line number, the function name, and
 * a descriptive error message.
 *
 * @param PARSER The JSON parser where the error occurred.
 * @param CODE The error code to set.
 * @param MESSAGE A descriptive message about the error.
 */
# define JSON_PARSER_ERROR(PARSER, CODE, MESSAGE)                                                                      \
     (PARSER)->error.code = (CODE);                                                                                    \
     (PARSER)->error.line = __LINE__;                                                                                  \
     (PARSER)->error.func = __func__;                                                                                  \
     (PARSER)->error.message = (MESSAGE)
#else
/**
 * @brief No-op macro for JSON parser errors.
 *
 * When JSON_ERROR is not defined, this macro does nothing.
 *
 * @param PARSER The JSON parser (unused).
 * @param CODE The error code (unused).
 * @param MESSAGE The error message (unused).
 */
# define JSON_PARSER_ERROR(PARSER, CODE, MESSAGE)
#endif

static int json__decode_value(struct json_parser *parser, struct json_value *value);

static int json__strlen(const char *str)
{
    int len = 0;
    while (*str++)
        len++;
    return len;
}

static int json__streq(const char *s1, const char *s2)
{
    if (json__strlen(s1) != json__strlen(s2))
        return 0;

    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }

    if (*s1 == 0 && *s2 == 0)
        return 1;

    return 0;
}

static int json__streqn(const char *s1, const char *s2, int limit)
{
    while (*s1 && *s2 && *s1 == *s2 && limit-- > 1) {
        s1++;
        s2++;
    }

    return *s1 == *s2 ? 1 : 0;
}

static void json__parse_whitespace(struct json_parser *parser)
{
    const char *ptr = parser->input;
    int pos = parser->position;
    int len = parser->length;

    while (pos < len && (ptr[pos] == ' ' || ptr[pos] == '\t' || ptr[pos] == '\n' || ptr[pos] == '\r'))
        pos++;

    parser->position = pos;
}

static int json__decode_string(struct json_parser *parser, struct json_value *value)
{
    int start = parser->position + 1;
    int end = start;
    int buffer_size = 16;
    int buffer_length = 0;
    char *buffer = (char *) json_alloc(buffer_size);

    if (buffer == NULL) {
        JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY, "Memory allocation failed");
        return -1;
    }

    while (end < parser->length && parser->input[end] != '"') {
        if (parser->input[end] == '\\') {
            end++;
            if (end >= parser->length) {
                json__free(buffer);
                JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Unterminated escape sequence");
                return -1;
            }

            if (parser->input[end] == 'u') {
                // Handle \uXXXX Unicode escape sequences
                end++;
                if (end + 3 >= parser->length) {
                    json__free(buffer);
                    JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Incomplete Unicode escape sequence");
                    return -1;
                }

                // Parse 4 hex digits
                unsigned int code_point = 0;
                for (int i = 0; i < 4; i++) {
                    char c = parser->input[end + i];
                    code_point <<= 4;

                    if (c >= '0' && c <= '9') {
                        code_point |= (c - '0');
                    } else if (c >= 'a' && c <= 'f') {
                        code_point |= (c - 'a' + 10);
                    } else if (c >= 'A' && c <= 'F') {
                        code_point |= (c - 'A' + 10);
                    } else {
                        json__free(buffer);
                        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Invalid hex digit in Unicode escape");
                        return -1;
                    }
                }

                // Check for surrogate pair
                if (code_point >= 0xD800 && code_point <= 0xDBFF) {
                    // High surrogate - need to get the low surrogate
                    end += 4; // Move past the first \uXXXX

                    // Check for the sequence \uYYYY where YYYY is the low surrogate
                    if (end + 1 >= parser->length || parser->input[end] != '\\' || parser->input[end + 1] != 'u') {
                        json__free(buffer);
                        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected low surrogate after high surrogate");
                        return -1;
                    }

                    end += 2; // Move past \u
                    if (end + 3 >= parser->length) {
                        json__free(buffer);
                        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
                                          "Incomplete Unicode escape sequence for low surrogate");
                        return -1;
                    }

                    // Parse 4 hex digits for low surrogate
                    unsigned int low_surrogate = 0;
                    for (int i = 0; i < 4; i++) {
                        char c = parser->input[end + i];
                        low_surrogate <<= 4;

                        if (c >= '0' && c <= '9') {
                            low_surrogate |= (c - '0');
                        } else if (c >= 'a' && c <= 'f') {
                            low_surrogate |= (c - 'a' + 10);
                        } else if (c >= 'A' && c <= 'F') {
                            low_surrogate |= (c - 'A' + 10);
                        } else {
                            json__free(buffer);
                            JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Invalid hex digit in low surrogate");
                            return -1;
                        }
                    }

                    // Validate low surrogate range
                    if (low_surrogate < 0xDC00 || low_surrogate > 0xDFFF) {
                        json__free(buffer);
                        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Invalid low surrogate value");
                        return -1;
                    }

                    // Combine surrogate pair to get actual code point
                    code_point = 0x10000 + ((code_point - 0xD800) << 10) + (low_surrogate - 0xDC00);
                    end += 4; // Move past the second \uXXXX
                } else {
                    end += 4; // Move past \uXXXX for non-surrogate case
                }

                // Encode code point as UTF-8
                if (code_point < 0x80) {
                    // 1-byte character
                    if (buffer_length + 1 >= buffer_size) {
                        buffer_size *= 2;
                        char *new_buffer = (char *) json_realloc(buffer, buffer_size);
                        if (new_buffer == NULL) {
                            json__free(buffer);
                            JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY, "Memory allocation failed");
                            return -1;
                        }
                        buffer = new_buffer;
                    }
                    buffer[buffer_length++] = (char) code_point;
                } else if (code_point < 0x800) {
                    // 2-byte character
                    if (buffer_length + 2 >= buffer_size) {
                        buffer_size *= 2;
                        char *new_buffer = (char *) json_realloc(buffer, buffer_size);
                        if (new_buffer == NULL) {
                            json__free(buffer);
                            JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY, "Memory allocation failed");
                            return -1;
                        }
                        buffer = new_buffer;
                    }
                    buffer[buffer_length++] = (char) (0xC0 | (code_point >> 6));
                    buffer[buffer_length++] = (char) (0x80 | (code_point & 0x3F));
                } else if (code_point < 0x10000) {
                    // 3-byte character
                    if (buffer_length + 3 >= buffer_size) {
                        buffer_size *= 2;
                        char *new_buffer = (char *) json_realloc(buffer, buffer_size);
                        if (new_buffer == NULL) {
                            json__free(buffer);
                            JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY, "Memory allocation failed");
                            return -1;
                        }
                        buffer = new_buffer;
                    }
                    buffer[buffer_length++] = (char) (0xE0 | (code_point >> 12));
                    buffer[buffer_length++] = (char) (0x80 | ((code_point >> 6) & 0x3F));
                    buffer[buffer_length++] = (char) (0x80 | (code_point & 0x3F));
                } else {
                    // 4-byte character
                    if (buffer_length + 4 >= buffer_size) {
                        buffer_size *= 2;
                        char *new_buffer = (char *) json_realloc(buffer, buffer_size);
                        if (new_buffer == NULL) {
                            json__free(buffer);
                            JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY, "Memory allocation failed");
                            return -1;
                        }
                        buffer = new_buffer;
                    }
                    buffer[buffer_length++] = (char) (0xF0 | (code_point >> 18));
                    buffer[buffer_length++] = (char) (0x80 | ((code_point >> 12) & 0x3F));
                    buffer[buffer_length++] = (char) (0x80 | ((code_point >> 6) & 0x3F));
                    buffer[buffer_length++] = (char) (0x80 | (code_point & 0x3F));
                }

                continue; // Skip the increment at the end of the loop
            } else {
                // Handle standard escape sequences
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
                    json__free(buffer);
                    JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Invalid escape character");
                    return -1;
                }

                if (buffer_length + 1 >= buffer_size) {
                    buffer_size *= 2;
                    char *new_buffer = (char *) json_realloc(buffer, buffer_size);
                    if (new_buffer == NULL) {
                        json__free(buffer);
                        JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY, "Memory allocation failed");
                        return -1;
                    }
                    buffer = new_buffer;
                }

                buffer[buffer_length++] = escaped_char;
            }
        } else {
            if (buffer_length + 1 >= buffer_size) {
                buffer_size *= 2;
                char *new_buffer = (char *) json_realloc(buffer, buffer_size);
                if (new_buffer == NULL) {
                    json__free(buffer);
                    JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY, "Memory allocation failed");
                    return -1;
                }
                buffer = new_buffer;
            }

            buffer[buffer_length++] = parser->input[end];
        }
        end++;
    }

    if (end == parser->length) {
        json__free(buffer);
        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Unterminated string");
        return -1;
    }

    buffer[buffer_length] = 0;

    value->type = JSON_TYPE_STRING;
    value->string.value = buffer;
    value->string.length = buffer_length;

    parser->position = end + 1;
    return 0;
}

static void json__parser_consume_digits(struct json_parser *parser)
{
    while (parser->position < parser->length && parser->input[parser->position] >= '0'
           && parser->input[parser->position] <= '9') {
        parser->position++;
    }
}

static int json__decode_number(struct json_parser *parser, struct json_value *value)
{
    int start = parser->position;
    char *endptr;

    if (parser->position < parser->length && parser->input[parser->position] == '-')
        parser->position++;

    if (parser->position < parser->length && parser->input[parser->position] == '0') {
        parser->position++;
    } else if (parser->position < parser->length && parser->input[parser->position] >= '1'
               && parser->input[parser->position] <= '9') {
        json__parser_consume_digits(parser);
    } else {
        JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER, "Invalid number");
        return -1;
    }

    if (parser->position < parser->length && parser->input[parser->position] == '.') {
        parser->position++;
        if (parser->position < parser->length && parser->input[parser->position] >= '0'
            && parser->input[parser->position] <= '9') {
            json__parser_consume_digits(parser);
        } else {
            JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER, "Invalid number");
            return -1;
        }
    }

    if (parser->position < parser->length
        && (parser->input[parser->position] == 'e' || parser->input[parser->position] == 'E')) {
        parser->position++;
        if (parser->position < parser->length
            && (parser->input[parser->position] == '-' || parser->input[parser->position] == '+')) {
            parser->position++;
        }
        if (parser->position < parser->length && parser->input[parser->position] >= '0'
            && parser->input[parser->position] <= '9') {
            json__parser_consume_digits(parser);
        } else {
            JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER, "Invalid number");
            return -1;
        }
    }

    if (start == parser->position) {
        JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER, "Invalid number");
        return -1;
    }

    value->type = JSON_TYPE_NUMBER;
    endptr = NULL;
    value->number = strtod(parser->input + start, &endptr);

    if (endptr != parser->input + parser->position) {
        JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER, "Invalid number");
        return -1;
    }

    return 0;
}

static int json__decode_array(struct json_parser *parser, struct json_value *array)
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

    if (parser->position + 1 < parser->length && parser->input[parser->position + 1] == ']') {
        parser->position += 2;
        return 0;
    }

    parser->position += 1; // Skip '['
    while (parser->position < parser->length && parser->input[parser->position] != ']') {
        json__parse_whitespace(parser);

        struct json_value *item;
        if ((item = json_alloc(sizeof(struct json_value))) == NULL) {
            JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY, "Memory allocation failed");
            json_array_free(array);
            return -1;
        }

        if (json__decode_value(parser, item) != 0) {
            json__free(item);
            json_array_free(array);
            return -1;
        }

        json_array_push(array, item);
        json__parse_whitespace(parser);

        if (parser->position < parser->length && parser->input[parser->position] == ',') // Skip ','
            parser->position++;
    }

    if (parser->position >= parser->length || parser->input[parser->position] != ']') {
        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected closing ']' for array");
        json_array_free(array);
        return -1;
    }

    if (parser->position < parser->length) // Skip ']'
        parser->position++;
    return 0;
}

static int json__decode_object(struct json_parser *parser, struct json_value *object)
{
    object->type = JSON_TYPE_OBJECT;
    json_object_init(object);

    parser->position++;
    json__parse_whitespace(parser);

    while (parser->position < parser->length && parser->input[parser->position] != '}') {
        struct json_value key, *value;
        json__parse_whitespace(parser);

        if (parser->input[parser->position] != '"') {
            JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected string key");
            return -1;
        }

        if (json__decode_string(parser, &key) != 0) {
            JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Failed to parse string key");
            return -1;
        }

        json__parse_whitespace(parser);
        if (parser->position >= parser->length || parser->input[parser->position] != ':') {
            json__free(key.string.value);
            JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected ':' after string key");
            return -1;
        }
        parser->position++; // Skip the colon

        value = (struct json_value *) json_alloc(sizeof(struct json_value));
        if (value == NULL) {
            json__free(key.string.value);
            JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY, "Failed to allocate memory for JSON item");
            return -1;
        }

        json__parse_whitespace(parser);
        if (json__decode_value(parser, value) != 0) {
            json__free(value);
            json__free(key.string.value);
            JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Failed to parse JSON value");
            return -1;
        }

        if (json_object_set(object, key.string.value, value) != 0) {
            json__free(value);
            json__free(key.string.value);
            JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY, "Failed to set key-value pair in object");
            return -1;
        }

        json__free(key.string.value);
        json__parse_whitespace(parser);
        if (parser->position < parser->length && parser->input[parser->position] == ',')
            parser->position++; // Skip comma or closing brace
    }

    if (parser->position >= parser->length || parser->input[parser->position] != '}') {
        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected '}' after JSON object");
        return -1;
    }

    parser->position++; // Skip the closing brace

    return 0;
}

static int json__decode_value(struct json_parser *parser, struct json_value *value)
{
    int rc;
    char c = parser->input[parser->position];

    if (c == '"') {
        rc = json__decode_string(parser, value);
    } else if (c == '[') {
        rc = json__decode_array(parser, value);
    } else if (c == '{') {
        rc = json__decode_object(parser, value);
    } else if (json__streqn(parser->input + parser->position, "true", 4)) {
        value->type = JSON_TYPE_BOOLEAN;
        value->number = 1;
        parser->position += 4;
        return 0;
    } else if (json__streqn(parser->input + parser->position, "false", 4)) {
        value->type = JSON_TYPE_BOOLEAN;
        value->number = 0;
        parser->position += 5;
        return 0;
    } else if (json__streqn(parser->input + parser->position, "null", 4)) {
        value->type = JSON_TYPE_NULL;
        parser->position += 4;
        return 0;
    } else {
        rc = json__decode_number(parser, value);
    }

#if defined(JSON_ERROR)
    if (rc != 0 && parser->error.code == JSON_ERROR_NONE) {
        JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected value");
    }
#endif

    return rc;
}

struct json_value *json_decode_with_length(const char *json, int length)
{
    struct json_parser parser;
    struct json_value *value = NULL;

    parser.input = json;
    parser.length = length;
    parser.position = 0;

#if defined(JSON_ERROR)
    parser.error.code = JSON_ERROR_NONE;
    parser.error.message = NULL;
#endif

    if ((value = json_alloc(sizeof(struct json_value))) == NULL)
        return NULL;

    if (json__decode_value(&parser, value) != 0) {
#if defined(JSON_ERROR) && !defined(JSON_ERROR_HANDLER)
        if (parser.error.code != JSON_ERROR_NONE) {
            fprintf(stderr, "JSON(\033[1merror\033[m): %s:%d %s at\n", parser.error.func, parser.error.line,
                    parser.error.message);
        }
#elif defined(JSON_ERROR) && defined(JSON_ERROR_HANDLER)
        if (parser.error.code != JSON_ERROR_NONE) {
            JSON_ERROR_HANDLER(parser.error.code, parser.error.message);
        }
#endif
        json__free(value);
        return NULL;
    }

    return value;
}

struct json_value *json_decode(const char *json)
{
    return json_decode_with_length(json, json__strlen(json));
}

static char *json__encode_array(struct json_value *value)
{
    int length;
    int buffer_size;
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

    for (int i = 0; i < value->array.length; i++) {
        int needed;
        int encoded_item_len;
        char *encoded_item = json_encode(value->array.items[i]);
        if (encoded_item == NULL) {
            json__free(encoded_array);
            return NULL;
        }

        encoded_item_len = json__strlen(encoded_item);

        needed = length + encoded_item_len + (i > 0 ? 1 : 0) + 1;
        if (needed > buffer_size) {
            char *new_encoded_array = (char *) json_realloc(encoded_array, (buffer_size = needed * 2));
            if (new_encoded_array == NULL) {
                json__free(encoded_array);
                json__free(encoded_item);
                return NULL;
            }
            encoded_array = new_encoded_array;
        }

        if (i > 0) {
            encoded_array[length++] = ',';
        }

        for (int i = 0; i < encoded_item_len; i++)
            encoded_array[length + i] = encoded_item[i];

        length += encoded_item_len;

        json__free(encoded_item);
    }

    encoded_array[length++] = ']';
    encoded_array[length] = '\0';

    return encoded_array;
}

static char *json__encode_object(struct json_value *object)
{
    int length;
    int buffer_size = 256;
    char *encoded_object = (char *) json_alloc(buffer_size);
    if (encoded_object == NULL) {
        return NULL;
    }

    encoded_object[0] = '{';
    length = 1;

    for (int i = 0; i < object->object.n_items; i++) {
        char *new_buffer;
        char *key = object->object.items[i]->key;
        char *value = json_encode(object->object.items[i]->value);

        int key_length = json__strlen(key);
        int value_length = json__strlen(value);

        int needed = length + key_length + value_length + 4 + (i < object->object.n_items - 1 ? 1 : 0);

        if (needed > buffer_size) {
            buffer_size = needed * 2;
            new_buffer = (char *) json_realloc(encoded_object, buffer_size);
            if (new_buffer == NULL) {
                json__free(encoded_object);
                json__free(value);
                return NULL;
            }
            encoded_object = new_buffer;
        }

        encoded_object[length++] = '"';
        for (int i = 0; i < key_length; i++)
            encoded_object[length + i] = key[i];

        length += key_length;
        encoded_object[length++] = '"';
        encoded_object[length++] = ':';
        for (int i = 0; i < value_length; i++)
            encoded_object[length + i] = value[i];

        length += value_length;

        json__free(value);

        if (i < object->object.n_items - 1) {
            encoded_object[length++] = ',';
        }
    }

    encoded_object[length++] = '}';
    encoded_object[length] = '\0';

    return encoded_object;
}

static char *json__encode_string(struct json_value *value)
{
    const char *str = value->string.value;
    int length = value->string.length;
    int buffer_size = length * 6 + 3; // Maximum possible size (all chars escaped + quotes)
    char *buffer = (char *) json_alloc(buffer_size);
    int pos = 0;

    if (!buffer)
        return NULL;

    buffer[pos++] = '"';

    for (int i = 0; i < length; i++) {
        unsigned char c = (unsigned char) str[i];

        if (c == '"' || c == '\\' || c == '/') {
            buffer[pos++] = '\\';
            buffer[pos++] = c;
        } else if (c == '\b') {
            buffer[pos++] = '\\';
            buffer[pos++] = 'b';
        } else if (c == '\f') {
            buffer[pos++] = '\\';
            buffer[pos++] = 'f';
        } else if (c == '\n') {
            buffer[pos++] = '\\';
            buffer[pos++] = 'n';
        } else if (c == '\r') {
            buffer[pos++] = '\\';
            buffer[pos++] = 'r';
        } else if (c == '\t') {
            buffer[pos++] = '\\';
            buffer[pos++] = 't';
        } else if (c < 32) {
            // Control characters must be escaped as \uXXXX
            buffer[pos++] = '\\';
            buffer[pos++] = 'u';
            buffer[pos++] = '0';
            buffer[pos++] = '0';
            buffer[pos++] = "0123456789ABCDEF"[(c >> 4) & 0xF];
            buffer[pos++] = "0123456789ABCDEF"[c & 0xF];
        } else if (c < 128) {
            // ASCII characters
            buffer[pos++] = c;
        } else {
            // UTF-8 encoded characters
            // Check if this is a multi-byte sequence
            const unsigned char *bytes = (const unsigned char *) &str[i];
            if ((c & 0xE0) == 0xC0) {
                // Two-byte sequence
                if (i + 1 >= length || (bytes[i + 1] & 0xC0) != 0x80) {
                    // Invalid UTF-8 sequence, escape as replacement character
                    buffer[pos++] = '\\';
                    buffer[pos++] = 'u';
                    buffer[pos++] = 'F';
                    buffer[pos++] = 'F';
                    buffer[pos++] = 'F';
                    buffer[pos++] = 'D';
                } else {
                    // Valid two-byte sequence, keep as is
                    buffer[pos++] = c;
                    buffer[pos++] = str[++i];
                }
            } else if ((c & 0xF0) == 0xE0) {
                // Three-byte sequence
                if (i + 2 >= length || (bytes[i + 1] & 0xC0) != 0x80 || (bytes[i + 2] & 0xC0) != 0x80) {
                    // Invalid UTF-8 sequence
                    buffer[pos++] = '\\';
                    buffer[pos++] = 'u';
                    buffer[pos++] = 'F';
                    buffer[pos++] = 'F';
                    buffer[pos++] = 'F';
                    buffer[pos++] = 'D';
                } else {
                    // Valid three-byte sequence, keep as is
                    buffer[pos++] = c;
                    buffer[pos++] = str[++i];
                    buffer[pos++] = str[++i];
                }
            } else if ((c & 0xF8) == 0xF0) {
                // Four-byte sequence
                if (i + 3 >= length || (bytes[i + 1] & 0xC0) != 0x80 || (bytes[i + 2] & 0xC0) != 0x80
                    || (bytes[i + 3] & 0xC0) != 0x80) {
                    // Invalid UTF-8 sequence
                    buffer[pos++] = '\\';
                    buffer[pos++] = 'u';
                    buffer[pos++] = 'F';
                    buffer[pos++] = 'F';
                    buffer[pos++] = 'F';
                    buffer[pos++] = 'D';
                } else {
                    // Valid four-byte sequence, keep as is
                    buffer[pos++] = c;
                    buffer[pos++] = str[++i];
                    buffer[pos++] = str[++i];
                    buffer[pos++] = str[++i];
                }
            } else {
                // Invalid UTF-8 start byte
                buffer[pos++] = '\\';
                buffer[pos++] = 'u';
                buffer[pos++] = 'F';
                buffer[pos++] = 'F';
                buffer[pos++] = 'F';
                buffer[pos++] = 'D';
            }
        }
    }

    buffer[pos++] = '"';
    buffer[pos] = '\0';

    // Reallocate to actual size
    char *result = (char *) json_realloc(buffer, pos + 1);
    return result ? result : buffer;
}

JSON_API char *json_encode(struct json_value *value)
{
    switch (value->type) {
    case JSON_TYPE_STRING:
        return json__encode_string(value);
    case JSON_TYPE_NUMBER: {
        char *ptr;
        char buffer[32];
        int length = snprintf(buffer, sizeof(buffer), "%.17g", value->number);

        if ((ptr = json_alloc(length + 1)) == NULL)
            return NULL;

        for (int i = 0; i < length; i++)
            ptr[i] = buffer[i];

        ptr[length] = 0;
        return ptr;
    }
    case JSON_TYPE_BOOLEAN: {
        char *ptr;
        const char *boolean_str = value->number != 0.0 ? "true" : "false";
        int length = json__strlen(boolean_str);

        if ((ptr = json_alloc(length + 1)) == NULL)
            return NULL;

        for (int i = 0; i < length; i++)
            ptr[i] = boolean_str[i];

        ptr[length] = 0;
        return ptr;
    }
    case JSON_TYPE_NULL: {
        char *ptr;
        const char *null_str = "null";
        int length = json__strlen(null_str);

        if ((ptr = json_alloc(length + 1)) == NULL)
            return NULL;

        for (int i = 0; i < length; i++)
            ptr[i] = null_str[i];

        ptr[length] = 0;
        return ptr;
    }
    case JSON_TYPE_ARRAY:
        return json__encode_array(value);
    case JSON_TYPE_OBJECT:
        return json__encode_object(value);
    default:
        return NULL;
    }
}

JSON_API void json_object_init(struct json_value *object)
{
    object->object.n_items = 0;
    object->object.capacity = 0;
    object->object.items = NULL;
}

JSON_API struct json_value *json_object_new(void)
{
    struct json_value *object = (struct json_value *) json_alloc(sizeof(struct json_value));
    if (!object) {
        return NULL;
    }

    object->type = JSON_TYPE_OBJECT;
    json_object_init(object);

    return object;
}

JSON_API void json_object_free(struct json_value *object)
{
    for (int i = 0; i < object->object.n_items; i++) {
        json_free(object->object.items[i]->value);
        json__free(object->object.items[i]->key);
        json__free(object->object.items[i]);
    }

    json__free(object->object.items);
    json__free(object);
}

JSON_API int json_object_set(struct json_value *object, const char *key, struct json_value *value)
{
    if (object->object.n_items >= object->object.capacity * JSON_OBJECT_CAPACITY_THRESHOLD) {
        void *items;
        int capacity;

        if (object->object.capacity)
            capacity = object->object.capacity * JSON_OBJECT_CAPACITY_MULTIPLIER;
        else
            capacity = JSON_OBJECT_INITIAL_CAPACITY;

        if ((items = json_realloc(object->object.items, capacity * sizeof(*object->object.items))) == NULL)
            return -1;

        object->object.items = items;
        object->object.capacity = capacity;
    }

    for (int i = 0; i < object->object.n_items; i++) {
        if (json__streq(object->object.items[i]->key, key)) {
            json_free(object->object.items[i]->value);
            object->object.items[i]->value = value;
            return 0;
        }
    }

    int idx = object->object.n_items++;
    object->object.items[idx] = json_alloc(sizeof(struct json_value));
    if (!object->object.items[idx])
        return -1;

    int key_len = json__strlen(key);
    object->object.items[idx]->key = json_alloc(key_len + 1);
    strncpy(object->object.items[idx]->key, key, key_len);
    object->object.items[idx]->key[key_len] = '\0';
    object->object.items[idx]->value = value;

    return 0;
}

JSON_API int json_object_has(struct json_value *object, const char *key)
{
    for (int i = 0; i < object->object.n_items; i++)
        if (json__streq(object->object.items[i]->key, key))
            return 1;

    return 0;
}

JSON_API struct json_value *json_object_get(struct json_value *object, const char *key)
{
    for (int i = 0; i < object->object.n_items; i++)
        if (json__streq(object->object.items[i]->key, key))
            return object->object.items[i]->value;

    return NULL;
}

JSON_API void json_object_remove(struct json_value *object, const char *key)
{
    for (int i = 0; i < object->object.n_items; i++) {
        if (json__streq(object->object.items[i]->key, key)) {
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
                json__free(object->object.items[i]->value);
                break;
            }

            json__free(object->object.items[i]->key);
            json__free(object->object.items[i]);

            if (i < object->object.n_items - 1) {
                object->object.items[i] = object->object.items[object->object.n_items - 1];
            }

            object->object.n_items--;
        }
    }
}

JSON_API int json_object_iter(const struct json_value *object, int *iter, char **key, struct json_value **value)
{
    if (*iter >= object->object.n_items)
        return 0;

    *key = object->object.items[*iter]->key;
    *value = object->object.items[(*iter)++]->value;

    return 1;
}

JSON_API void json_object_clear(struct json_value *object)
{
    char *key;
    int iter = 0;
    struct json_value *value;

    while (json_object_iter(object, &iter, &key, &value)) {
        json_object_remove(object, key);
        iter--;
    }
}

JSON_API void json_array_init(struct json_value *array)
{
    array->array.length = 0;
    array->array.capacity = 0;
    array->array.items = NULL;
}

JSON_API struct json_value *json_array_new(void)
{
    struct json_value *value;
    if ((value = json_alloc(sizeof(struct json_value))) == NULL)
        return NULL;

    value->type = JSON_TYPE_ARRAY;
    json_array_init(value);

    return value;
}

JSON_API void json_array_free(struct json_value *value)
{
    for (int i = 0; i < value->array.length; i++) {
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
            json__free(value->array.items[i]);
            break;
        }
    }
    json__free(value->array.items);
    json__free(value);
}

JSON_API void json_free(struct json_value *value)
{
    switch (value->type) {
    case JSON_TYPE_OBJECT:
        json_object_free(value);
        break;
    case JSON_TYPE_ARRAY:
        json_array_free(value);
        break;
    case JSON_TYPE_STRING:
        json_string_free(value);
        break;
    default:
        json__free(value);
    }
}

JSON_API struct json_value *json_deep_copy(struct json_value *value)
{
    struct json_value *new_value;
    struct json_value *element;
    char *key;
    int iter;

    iter = 0;

    switch (value->type) {
    case JSON_TYPE_OBJECT:
        new_value = json_object_new();
        while (json_object_iter(value, &iter, &key, &element))
            json_object_set(new_value, key, json_deep_copy(element));
        return new_value;

    case JSON_TYPE_ARRAY:
        new_value = json_array_new();
        while (json_array_iter(value, &iter, &element))
            json_array_push(new_value, json_deep_copy(element));
        return new_value;

    case JSON_TYPE_STRING:
        return json_string_new(value->string.value);

    default:
        new_value = json_alloc(sizeof(struct json_value));
        memcpy(new_value, value, sizeof(*value));
        return new_value;
    }

    return NULL;
}

JSON_API void json_array_remove(struct json_value *array, int index)
{
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

    for (int i = index; i < array->array.length - 1; i++)
        array->array.items[i] = array->array.items[i + 1];
    array->array.length--;
}

JSON_API inline int json_array_length(struct json_value *array)
{
    return array->array.length;
}

JSON_API
int json_array_push(struct json_value *array, struct json_value *value)
{
    int index = array->array.length;
    int capacity = array->array.capacity;
    int capacity_threshold = capacity * JSON_ARRAY_CAPACITY_THRESHOLD;

    if (index >= capacity_threshold) {
        int capacity;
        if (array->array.capacity > 0)
            capacity = array->array.capacity * JSON_ARRAY_CAPACITY_MULTIPLIER;
        else
            capacity = JSON_ARRAY_INITIAL_CAPACITY;

        struct json_value **items;
        int size = capacity * sizeof(struct json_value *);
        if ((items = json_realloc(array->array.items, size)) == NULL)
            return -1;

        array->array.items = items;
        array->array.capacity = capacity;
    }

    array->array.items[array->array.length++] = value;
    return 0;
}

JSON_API int json_array_iter(struct json_value *array, int *index, struct json_value **value)
{
    if (*index >= array->array.length)
        return 0;

    *value = array->array.items[(*index)++];
    return 1;
}

JSON_API void json_array_clear(struct json_value *array)
{
    int iter = 0;
    struct json_value *value;

    while (json_array_iter(array, &iter, &value))
        json_array_remove(array, iter--);
}

JSON_API struct json_value *json_string_new(const char *string)
{
    struct json_value *value;
    if ((value = json_alloc(sizeof(struct json_value))) == NULL)
        return NULL;

    value->type = JSON_TYPE_STRING;
    value->string.length = json__strlen(string);
    if ((value->string.value = json_alloc(value->string.length + 1)) == NULL) {
        json__free(value);
        return NULL;
    }

    for (int i = 0; i < value->string.length; i++)
        value->string.value[i] = string[i];

    value->string.value[value->string.length] = 0;
    return value;
}

JSON_API struct json_value *json_number_new(double value)
{
    struct json_value *number;
    if ((number = json_alloc(sizeof(struct json_value))) == NULL)
        return NULL;

    number->type = JSON_TYPE_NUMBER;
    number->number = value;

    return number;
}

JSON_API struct json_value *json_boolean_new(int value)
{
    struct json_value *boolean;
    if ((boolean = json_alloc(sizeof(struct json_value))) == NULL)
        return NULL;

    boolean->type = JSON_TYPE_BOOLEAN;
    boolean->number = value;

    return boolean;
}

JSON_API void json_string_free(struct json_value *string)
{
    json__free(string->string.value);
    json__free(string);
}

static inline void json__print_indent(int indent)
{
    for (int i = 0; i < indent; i++) {
        putchar(' ');
    }
}

static void json__print_internal(struct json_value *value, int indent)
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
        printf("%.17g", value->number);
        break;
    case JSON_TYPE_BOOLEAN:
        printf("%s", value->number == 0.0 ? "false" : "true");
        break;
    case JSON_TYPE_NULL:
        printf("null");
        break;
    case JSON_TYPE_ARRAY:
        printf("[\n");
        for (int i = 0; i < value->array.length; i++) {
            json__print_indent(indent + 2);
            json__print_internal(value->array.items[i], indent + 2);
            if (i < value->array.length - 1) {
                printf(",\n");
            } else {
                printf("\n");
            }
        }
        json__print_indent(indent);
        printf("]");
        break;
    case JSON_TYPE_OBJECT:
        printf("{\n");
        for (int i = 0; i < value->object.n_items; i++) {
            json__print_indent(indent + 2);
            printf("\"%s\": ", value->object.items[i]->key);
            json__print_internal(value->object.items[i]->value, indent + 2);
            if (i < value->object.n_items - 1) {
                printf(",\n");
            } else {
                printf("\n");
            }
        }
        json__print_indent(indent);
        printf("}");
        break;
    default:
        break;
    }
}

JSON_API void json_print(struct json_value *value)
{
    json__print_internal(value, 0);
}

JSON_API void json_println(struct json_value *value)
{
    json_print(value);
    putchar('\n');
}

#ifdef __cplusplus
}
#endif

#endif /* JSON_H */
