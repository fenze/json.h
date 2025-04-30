// json.h - A lightweight, single-header JSON library for C.
// Copyright (c) 2025 Kacper Fiedorowicz. All rights reserved.
// This software is licensed under the MIT License.
// See LICENSE for more information.

#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include <stdlib.h>

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

#ifdef JSON_STRDUP
/**
 * Duplicates a string for JSON operations.
 *
 * If `JSON_STRDUP` is defined, it uses the custom string duplication function
 * provided by the user. Otherwise, it defaults to the standard `strdup`.
 *
 * @param str A pointer to the null-terminated string to duplicate.
 * @return A pointer to the duplicated string, or NULL if duplication fails.
 */
# define json_strdup(str) JSON_STRDUP(str)
#else
# define json_strdup(str) strdup(str)
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
# define json_free(ptr) JSON_FREE(ptr)
#else
# define json_free(ptr) free(ptr)
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

struct json_value
{
    /**
     * Enum representing the type of the JSON value.
     *
     * - JSON_TYPE_NULL: Represents a null value.
     * - JSON_TYPE_BOOLEAN: Represents a boolean value (true/false).
     * - JSON_TYPE_NUMBER: Represents a numeric value.
     * - JSON_TYPE_STRING: Represents a string value.
     * - JSON_TYPE_ARRAY: Represents an array of JSON values.
     * - JSON_TYPE_OBJECT: Represents an object with key-value pairs.
     */
    enum
    {
        JSON_TYPE_NULL,
        JSON_TYPE_BOOLEAN,
        JSON_TYPE_NUMBER,
        JSON_TYPE_STRING,
        JSON_TYPE_ARRAY,
        JSON_TYPE_OBJECT
    } type;

    /**
     * Union holding the actual data of the JSON value.
     * The type of data stored depends on the `type` field.
     */
    union
    {
        /**
         * Holds a numeric value when `type` is JSON_TYPE_NUMBER.
         */
        double number;

        /**
         * Holds an array of JSON values when `type` is JSON_TYPE_ARRAY.
         *
         * - `capacity`: The total allocated capacity of the array.
         * - `length`: The current number of elements in the array.
         * - `items`: A pointer to an array of pointers to `json_value`
         * elements.
         */
        struct
        {
            int capacity;
            int length;
            struct json_value **items;
        } array;

        /**
         * Holds a string value when `type` is JSON_TYPE_STRING.
         *
         * - `capacity`: The total allocated capacity of the string.
         * - `length`: The current length of the string (excluding null
         * terminator).
         * - `value`: A pointer to the null-terminated string.
         */
        struct
        {
            int capacity;
            int length;
            char *value;
        } string;

        /**
         * Holds an object with key-value pairs when `type` is JSON_TYPE_OBJECT.
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
            int capacity;
            int n_items;
            struct
            {
                char *key;
                struct json_value *value;
            } **items;
        } object;
    };
};

/**
 * Checks if a JSON value is an array.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is an array, 0 otherwise.
 */
#define json_is_array(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_ARRAY)

/**
 * Checks if a JSON value is a boolean.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is a boolean, 0 otherwise.
 */
#define json_is_boolean(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_BOOLEAN)

/**
 * Checks if a JSON value is null.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is null, 0 otherwise.
 */
#define json_is_null(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_NULL)

/**
 * Checks if a JSON value is a number.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is a number, 0 otherwise.
 */
#define json_is_number(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_NUMBER)

/**
 * Checks if a JSON value is an object.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is an object, 0 otherwise.
 */
#define json_is_object(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_OBJECT)

/**
 * Checks if a JSON value is a string.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is a string, 0 otherwise.
 */
#define json_is_string(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_STRING)

/**
 * Retrieves the boolean value from a JSON boolean.
 *
 * @param boolean The JSON boolean value to retrieve.
 * @return Non-zero if the boolean is true, 0 if false.
 */
#define json_boolean_get(boolean) ((boolean)->number != 0.0)

/**
 * Sets the boolean value of a JSON boolean.
 *
 * @param boolean The JSON boolean value to modify.
 * @param value The new boolean value to set (non-zero for true, 0 for false).
 */
#define json_boolean_set(boolean, value) ((boolean)->number = value)

/**
 * Retrieves the numeric value from a JSON number.
 *
 * @param number The JSON number value to retrieve.
 * @return The numeric value.
 */
#define json_number_get(number) ((number)->number)

/**
 * Sets the numeric value of a JSON number.
 *
 * @param number The JSON number value to modify.
 * @param value The new numeric value to set.
 */
#define json_number_set(number, value) ((number)->number = value)

/**
 * Retrieves the string value from a JSON string.
 *
 * @param string The JSON string value to retrieve.
 * @return A pointer to the string value.
 */
#define json_string_get(string) ((string)->string.value)

/**
 * Sets the string value of a JSON string.
 *
 * @param string The JSON string value to modify.
 * @param value The new string value to set.
 */
#define json_string_set(string, value) ((string)->string.value = value)

/**
 * Retrieves the number of key-value pairs in a JSON object.
 *
 * @param object A pointer to the JSON object to query.
 * @return The total count of key-value pairs in the object.
 */
#define json_object_count(object) ((object)->object.n_items)

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
JSON_API int json_object_iter(const struct json_value *object, int *iter,
                              char **key, struct json_value **value);

/**
 * Sets a value at a specific index in a JSON array.
 *
 * @param array The JSON array to modify.
 * @param index The index to set the value at.
 * @param value The value to set.
 * @return 0 on success, or -1 on failure.
 */
JSON_API int json_array_set(struct json_value *array, int index,
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
JSON_API struct json_value *json_array_get(struct json_value *array, int index);

/**
 * Removes a value at a specific index in a JSON array.
 *
 * @param array The JSON array to modify.
 * @param index The index to remove the value from.
 * @return 0 on success, or -1 on failure.
 */
JSON_API int json_array_remove(struct json_value *array, int index);

/**
 * Retrieves the length of a JSON array.
 *
 * @param array The JSON array to query.
 * @return The number of elements in the array.
 */
JSON_API int json_array_length(struct json_value *array);

/**
 * Iterates over the elements in a JSON array.
 *
 * @param array The JSON array to iterate over.
 * @param index A pointer to the iteration index (should be initialized to 0).
 * @param value A pointer to store the current value.
 * @return 1 on success, or 0 if the iteration is complete.
 */
JSON_API int json_array_iter(struct json_value *array, int *index,
                             struct json_value **value);

/**
 * Creates a new JSON string with the specified value.
 *
 * @param value The string value to initialize the JSON string with.
 * @return A pointer to the newly created JSON string, or NULL on failure.
 */
JSON_API struct json_value *json_string_new(const char *value);

/**
 * Creates a new JSON number with the specified value.
 *
 * @param value The numeric value to initialize the JSON number with.
 * @return A pointer to the newly created JSON number, or NULL on failure.
 */
JSON_API struct json_value *json_number_new(double value);

/**
 * Creates a new JSON boolean with the specified value.
 *
 * @param value The boolean value to initialize the JSON boolean with.
 * @return A pointer to the newly created JSON boolean, or NULL on failure.
 */
JSON_API struct json_value *json_boolean_new(int value);

/* PRIVATE */

struct json_parser
{
    const char *input;
    int length;
    int position;
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
JSON_API void json_object_init(struct json_value *object);

/**
 * Decodes a JSON value from a parser.
 *
 * @param parser The JSON parser to decode from.
 * @param value A pointer to store the decoded value.
 * @return 0 on success, or -1 on failure.
 */
JSON_API int json__decode_value(struct json_parser *parser,
                                struct json_value *value);

#if defined(JSON_ERROR)
# define JSON_PARSER_ERROR(PARSER, CODE, MESSAGE)                              \
     (PARSER)->error.code = (CODE);                                            \
     (PARSER)->error.line = __LINE__;                                          \
     (PARSER)->error.func = __func__;                                          \
     (PARSER)->error.message = (MESSAGE)
#else
# define JSON_PARSER_ERROR(PARSER, CODE, MESSAGE)
#endif

static int json__strlen(const char *str)
{
    int len = 0;
    while (*str++) {
        len++;
    }
    return len;
}

static int json__streq(const char *s1, const char *s2)
{
    if (json__strlen(s1) != json__strlen(s2)) {
        return 0;
    }

    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }

    if (*s1 == 0 && *s2 == 0) {
        return 1;
    }

    return 0;
}

static void json__parse_whitespace(struct json_parser *parser)
{
    const char *input = parser->input;
    int pos = parser->position;
    int len = parser->length;

    while (pos < len
           && (input[pos] == ' ' || input[pos] == '\t' || input[pos] == '\n'
               || input[pos] == '\r')) {
        pos++;
    }

    parser->position = pos;
}

static int json__decode_string(struct json_parser *parser,
                               struct json_value *value)
{
    int start = parser->position + 1;
    int end = start;
    int buffer_size = 16;
    int buffer_length = 0;
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

static void json__parser_consume_digits(struct json_parser *parser)
{
    while (parser->position < parser->length
           && parser->input[parser->position] >= '0'
           && parser->input[parser->position] <= '9') {
        parser->position++;
    }
}

static int json__decode_number(struct json_parser *parser,
                               struct json_value *value)
{
    int start = parser->position;
    char *endptr;

    if (parser->position < parser->length
        && parser->input[parser->position] == '-') {
        parser->position++;
    }

    if (parser->position < parser->length
        && parser->input[parser->position] == '0') {
        parser->position++;
    } else if (parser->position < parser->length
               && parser->input[parser->position] >= '1'
               && parser->input[parser->position] <= '9') {
        json__parser_consume_digits(parser);
    } else {
        JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER, "Invalid number");
        return -1;
    }

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
            return -1;
        }
    }

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

static int json__decode_array(struct json_parser *parser,
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

static int json__decode_object(struct json_parser *parser,
                               struct json_value *object)
{
    object->type = JSON_TYPE_OBJECT;
    json_object_init(object);

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

        if (json__decode_string(parser, &key) != 0) {
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
        rc = json__decode_string(parser, value);
    } else if (c == '[') {
        rc = json__decode_array(parser, value);
    } else if (c == '{') {
        rc = json__decode_object(parser, value);
    } else if (json__streq(parser->input + parser->position, "true")) {
        value->type = JSON_TYPE_BOOLEAN;
        value->number = 1;
        parser->position += 4;
        return 0;
    } else if (json__streq(parser->input + parser->position, "false")) {
        value->type = JSON_TYPE_BOOLEAN;
        value->number = 0;
        parser->position += 5;
        return 0;
    } else if (json__streq(parser->input + parser->position, "null")) {
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

struct json_value *json_decode(const char *json)
{
    struct json_parser parser;
    struct json_value *value = NULL;

    if (json == NULL) {
        return NULL;
    }

    parser.input = json;
    parser.length = json__strlen(json);
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
            json_free(encoded_array);
            return NULL;
        }

        encoded_item_len = json__strlen(encoded_item);

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

        for (int i = 0; i < encoded_item_len; i++)
            encoded_array[length + i] = encoded_item[i];

        length += encoded_item_len;

        json_free(encoded_item);
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

        int needed = length + key_length + value_length + 4
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
        for (int i = 0; i < key_length; i++)
            encoded_object[length + i] = key[i];

        length += key_length;
        encoded_object[length++] = '"';
        encoded_object[length++] = ':';
        for (int i = 0; i < value_length; i++)
            encoded_object[length + i] = value[i];

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

JSON_API char *json_encode(struct json_value *value)
{
    if (value == NULL) {
        return NULL;
    }

    switch (value->type) {
    case JSON_TYPE_STRING: {
        int buffer_size = value->string.length + 3;
        char *buffer = json_alloc(buffer_size);
        if (buffer == NULL) {
            return NULL;
        }
        buffer[0] = '"';
        for (int i = 0; i < value->string.length; i++)
            buffer[1 + i] = value->string.value[i];
        buffer[value->string.length + 1] = '"';
        buffer[value->string.length + 2] = 0; // Null-terminated string
        return buffer;
    }
    case JSON_TYPE_NUMBER: {
        char buffer[32];
        int length = snprintf(buffer, sizeof(buffer), "%.17g", value->number);

        char *ptr = json_alloc(length + 1);
        if (ptr == NULL)
            return NULL;

        for (int i = 0; i < length; i++)
            ptr[i] = buffer[i];

        ptr[length] = '\0';
        return ptr;
    }
    case JSON_TYPE_BOOLEAN: {
        const char *boolean_str = value->number != 0.0 ? "true" : "false";
        int length = json__strlen(boolean_str);

        char *ptr = json_alloc(length + 1);
        if (ptr == NULL)
            return NULL;

        for (int i = 0; i < length; i++)
            ptr[i] = boolean_str[i];

        ptr[length] = '\0';
        return ptr;
    }
    case JSON_TYPE_NULL: {
        const char *null_str = "null";
        int length = json__strlen(null_str);

        char *ptr = json_alloc(length + 1);
        if (ptr == NULL)
            return NULL;

        for (int i = 0; i < length; i++)
            ptr[i] = null_str[i];

        ptr[length] = '\0';
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

#if defined(JSON_PRINT)
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

static void json_print(struct json_value *value)
{
    json__print_internal(value, 0);
}

static void json_println(struct json_value *value)
{
    json_print(value);
    putchar('\n');
}
#else
# define json_print(value)
# define json_println(value)
#endif

JSON_API void json_object_init(struct json_value *object)
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
    json_object_init(object);

    return object;
}

JSON_API void json_object_free(struct json_value *object)
{
    char *key;
    int iter = 0;
    struct json_value *value;

    while (json_object_iter(object, &iter, &key, &value)) {
        json_object_remove(object, key);
        iter--;
    }

    json_free(object->object.items);
    json_free(object);
}

JSON_API int json_object_set(struct json_value *object, const char *key,
                             struct json_value *value)
{
    if (object->object.n_items == object->object.capacity) {
        int new_capacity =
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

    int idx = object->object.n_items++;
    object->object.items[idx] = json_alloc(sizeof(*object->object.items[idx]));
    if (!object->object.items[idx])
        return -1;

    int key_len = json__strlen(key);
    object->object.items[idx]->key = json_alloc(key_len + 1);
    for (int i = 0; i < key_len; i++) {
        object->object.items[idx]->key[i] =
            (key[i] >= 'a' && key[i] <= 'z') ? key[i] - 32 : key[i];
    }
    object->object.items[idx]->key[key_len] = '\0';
    object->object.items[idx]->value = value;

    return 0;
}

JSON_API int json_object_has(struct json_value *object, const char *key)
{
    for (int i = 0; i < object->object.n_items; i++) {
        if (json__streq(object->object.items[i]->key, key)) {
            return 1;
        }
    }

    return 0;
}

struct json_value *json_object_get(struct json_value *object, const char *key)
{
    for (int i = 0; i < object->object.n_items; i++) {
        if (json__streq(object->object.items[i]->key, key)) {
            return object->object.items[i]->value;
        }
    }

    return NULL;
}

JSON_API int json_object_remove(struct json_value *object, const char *key)
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

JSON_API int json_object_iter(const struct json_value *object, int *iter,
                              char **key, struct json_value **value)
{
    if (*iter >= object->object.n_items)
        return 0;

    *key = object->object.items[*iter]->key;
    *value = object->object.items[*iter]->value;
    (*iter)++;

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
            json_free(value->array.items[i]);
            break;
        }
    }
    json_free(value->array.items);
    json_free(value);
}

JSON_API int json_array_set(struct json_value *array, int index,
                            struct json_value *value)
{
    if (!array) {
        return -1;
    }

    if ((int) index > array->array.length) {
        return -1;
    }

    if ((int) index == array->array.length) {
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

JSON_API int json_array_remove(struct json_value *array, int index)
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

    for (int i = index; i < array->array.length - 1; i++)
        array->array.items[i] = array->array.items[i + 1];
    array->array.length--;

    return 0;
}

struct json_value *json_array_get(struct json_value *array, int index)
{
    if (!array || array->type != JSON_TYPE_ARRAY
        || index >= array->array.length) {
        return NULL;
    }

    return array->array.items[index];
}

JSON_API int json_array_push(struct json_value *array, struct json_value *value)
{
    if (array->array.length == array->array.capacity) {
        int new_capacity =
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

int json_array_iter(struct json_value *array, int *index,
                    struct json_value **value)
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

JSON_API int json_array_length(struct json_value *array)
{
    return array->array.length;
}

struct json_value *json_string_new(const char *string)
{
    struct json_value *value =
        (struct json_value *) json_alloc(sizeof(struct json_value));
    if (!value) {
        return NULL;
    }

    value->type = JSON_TYPE_STRING;
    value->string.length = json__strlen(string);
    value->string.value = json_alloc(value->string.length + 1);
    if (!value->string.value) {
        json_free(value);
        return NULL;
    }

    for (int i = 0; i < value->string.length; i++) {
        value->string.value[i] =
            (string[i] >= 'a' && string[i] <= 'z') ? string[i] - 32 : string[i];
    }
    value->string.value[value->string.length] = '\0';

    return value;
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

void json_string_free(struct json_value *string)
{
    json_free(string->string.value);
    json_free(string);
}

#ifdef __cplusplus
}
#endif

#endif /* JSON_H */
