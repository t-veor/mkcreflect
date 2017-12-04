/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <marcin.kolny@gmail.com> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return. Marcin Kolny
 * ----------------------------------------------------------------------------
 */
#ifndef MKCREFLECT_H_
#define MKCREFLECT_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    MKCREFLECT_TYPES_STRUCT,
    MKCREFLECT_TYPES_STRING,
    MKCREFLECT_TYPES_INTEGER
} MKCREFLECT_Types;

typedef struct
{
    const char* field_type;
    const char* field_name;
    size_t size;
    size_t offset;
    int array_size;
    MKCREFLECT_Types data_type;
} MKCREFLECT_FieldInfo;

typedef struct
{
    const char* name;
    size_t fields_count;
    MKCREFLECT_FieldInfo* fields;
} MKCREFLECT_TypeInfo;

#define MKCREFLECT_EXPAND_(X) X
#define MKCREFLECT_EXPAND_VA_(...) __VA_ARGS__
#define MKCREFLECT_FOREACH_1_(FNC, USER_DATA, ARG) FNC(ARG, USER_DATA)
#define MKCREFLECT_FOREACH_2_(FNC, USER_DATA, ARG, ...) \
    FNC(ARG, USER_DATA) MKCREFLECT_EXPAND_(MKCREFLECT_FOREACH_1_(FNC, USER_DATA, __VA_ARGS__))
#define MKCREFLECT_FOREACH_3_(FNC, USER_DATA, ARG, ...) \
    FNC(ARG, USER_DATA) MKCREFLECT_EXPAND_(MKCREFLECT_FOREACH_2_(FNC, USER_DATA, __VA_ARGS__))
#define MKCREFLECT_FOREACH_4_(FNC, USER_DATA, ARG, ...) \
    FNC(ARG, USER_DATA) MKCREFLECT_EXPAND_(MKCREFLECT_FOREACH_3_(FNC, USER_DATA, __VA_ARGS__))
#define MKCREFLECT_FOREACH_5_(FNC, USER_DATA, ARG, ...) \
    FNC(ARG, USER_DATA) MKCREFLECT_EXPAND_(MKCREFLECT_FOREACH_4_(FNC, USER_DATA, __VA_ARGS__))
#define MKCREFLECT_FOREACH_6_(FNC, USER_DATA, ARG, ...) \
    FNC(ARG, USER_DATA) MKCREFLECT_EXPAND_(MKCREFLECT_FOREACH_5_(FNC, USER_DATA, __VA_ARGS__))
#define MKCREFLECT_FOREACH_7_(FNC, USER_DATA, ARG, ...) \
    FNC(ARG, USER_DATA) MKCREFLECT_EXPAND_(MKCREFLECT_FOREACH_6_(FNC, USER_DATA, __VA_ARGS__))

#define MKCREFLECT_OVERRIDE_4(_1, _2, _3, _4, FNC, ...) FNC
#define MKCREFLECT_OVERRIDE_4_PLACEHOLDER 1, 2, 3, 4
#define MKCREFLECT_OVERRIDE_5(_1, _2, _3, _4, _5, FNC, ...) FNC
#define MKCREFLECT_OVERRIDE_5_PLACEHOLDER MKCREFLECT_OVERRIDE_4_PLACEHOLDER, 5
#define MKCREFLECT_OVERRIDE_7(_1, _2, _3, _4, _5, _6, _7, FNC, ...) FNC
#define MKCREFLECT_OVERRIDE_7_PLACEHOLDER MKCREFLECT_OVERRIDE_4_PLACEHOLDER, 6, 7

#define MKCREFLECT_FOREACH(FNC, USER_DATA, ...) \
    MKCREFLECT_EXPAND_(MKCREFLECT_OVERRIDE_7( __VA_ARGS__,	\
    MKCREFLECT_FOREACH_7_, \
    MKCREFLECT_FOREACH_6_, \
    MKCREFLECT_FOREACH_5_, \
    MKCREFLECT_FOREACH_4_, \
    MKCREFLECT_FOREACH_3_, \
    MKCREFLECT_FOREACH_2_, \
    MKCREFLECT_FOREACH_1_)(FNC, USER_DATA, __VA_ARGS__))

#define MKCREFLECT_DECLARE_SIMPLE_FIELD_(IGNORE, TYPE, FIELD_NAME) \
    TYPE FIELD_NAME;
#define MKCREFLECT_DECLARE_ARRAY_FIELD_(IGNORE, TYPE, FIELD_NAME, ARRAY_SIZE) \
    TYPE FIELD_NAME[ARRAY_SIZE];

#define MKCREFLECT_DECLARE_FIELD_(...) MKCREFLECT_EXPAND_(MKCREFLECT_OVERRIDE_4( \
    __VA_ARGS__, \
    MKCREFLECT_DECLARE_ARRAY_FIELD_, \
    MKCREFLECT_DECLARE_SIMPLE_FIELD_, \
    MKCREFLECT_OVERRIDE_4_PLACEHOLDER)(__VA_ARGS__))

#define MKCREFLECT_DECLARE_FIELD(X, USER_DATA) MKCREFLECT_DECLARE_FIELD_ X

#define MKCREFLECT_SUM(...) +1

#define MKCREFLECT_ARRAY_FIELD_INFO_(TYPE_NAME, DATA_TYPE, C_TYPE, FIELD_NAME, ARRAY_SIZE) \
    #C_TYPE, #FIELD_NAME, sizeof(C_TYPE) * ARRAY_SIZE, offsetof(TYPE_NAME, FIELD_NAME), \
    ARRAY_SIZE, MKCREFLECT_TYPES_##DATA_TYPE

#define MKCREFLECT_SIMPLE_FIELD_INFO_(TYPE_NAME, DATA_TYPE, C_TYPE, FIELD_NAME) \
    #C_TYPE, #FIELD_NAME, sizeof(C_TYPE), offsetof(TYPE_NAME, FIELD_NAME), \
    -1, MKCREFLECT_TYPES_##DATA_TYPE

#define MKCREFLECT_FIELD_INFO_(...) \
{ \
    MKCREFLECT_EXPAND_(MKCREFLECT_OVERRIDE_5( \
    __VA_ARGS__, \
    MKCREFLECT_ARRAY_FIELD_INFO_, \
    MKCREFLECT_SIMPLE_FIELD_INFO_, \
    MKCREFLECT_OVERRIDE_5_PLACEHOLDER)(__VA_ARGS__)) \
},

#define MKCREFLECT_FIELD_INFO(X, USER_DATA) \
    MKCREFLECT_FIELD_INFO_(USER_DATA, MKCREFLECT_EXPAND_VA_ X)

#define MKCREFLECT_DEFINE_STRUCT(GET_TYPE_FNC_NAME, TYPE_NAME, ...) \
    typedef struct \
    { \
        MKCREFLECT_FOREACH(MKCREFLECT_DECLARE_FIELD, 0, __VA_ARGS__) \
    } TYPE_NAME; \
    static MKCREFLECT_TypeInfo* GET_TYPE_FNC_NAME(void) \
    { \
        static MKCREFLECT_FieldInfo fields_info[MKCREFLECT_FOREACH(MKCREFLECT_SUM, 0, __VA_ARGS__)] = \
        { \
            MKCREFLECT_FOREACH(MKCREFLECT_FIELD_INFO, TYPE_NAME, __VA_ARGS__) \
        }; \
        static MKCREFLECT_TypeInfo type_info = \
        { \
            #TYPE_NAME, \
            MKCREFLECT_FOREACH(MKCREFLECT_SUM, 0, __VA_ARGS__), \
            fields_info \
        }; \
        return &type_info; \
    }

#ifdef __cplusplus
}
#endif

#endif /* MKCREFLECT_H_ */
