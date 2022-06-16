////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_DEFINE_H
#define TRINITY_DEFINE_H

#include "CompilerDefs.h"

#include <cstddef>
#include <cinttypes>
#include <climits>

#include <ace/ACE_export.h>


#define TRINITY_LITTLEENDIAN 0
#define TRINITY_BIGENDIAN    1

#if !defined(TRINITY_ENDIAN)
#  if defined (BOOST_BIG_ENDIAN)
#    define TRINITY_ENDIAN TRINITY_BIGENDIAN
#  else
#    define TRINITY_ENDIAN TRINITY_LITTLEENDIAN
#  endif
#endif

#if TRINITY_PLATFORM == TRINITY_PLATFORM_WINDOWS
#  define TRINITY_PATH_MAX 260
#  define _USE_MATH_DEFINES
#  ifndef DECLSPEC_NORETURN
#    define DECLSPEC_NORETURN __declspec(noreturn)
#  endif //DECLSPEC_NORETURN
#  ifndef DECLSPEC_DEPRECATED
#    define DECLSPEC_DEPRECATED __declspec(deprecated)
#  endif //DECLSPEC_DEPRECATED
#else // TRINITY_PLATFORM != TRINITY_PLATFORM_WINDOWS
#  define TRINITY_PATH_MAX PATH_MAX
#  define DECLSPEC_NORETURN
#  define DECLSPEC_DEPRECATED
#endif // TRINITY_PLATFORM

#if !defined(COREDEBUG)
#  define TRINITY_INLINE inline
#else //COREDEBUG
#  if !defined(TRINITY_DEBUG)
#    define TRINITY_DEBUG
#  endif //TRINITY_DEBUG
#  define TRINITY_INLINE
#endif //!COREDEBUG

#if TRINITY_COMPILER == TRINITY_COMPILER_GNU
#  define ATTR_NORETURN __attribute__((__noreturn__))
#  define ATTR_PRINTF(F, V) __attribute__ ((__format__ (__printf__, F, V)))
#  define ATTR_DEPRECATED __attribute__((__deprecated__))
#else //TRINITY_COMPILER != TRINITY_COMPILER_GNU
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F, V)
#  define ATTR_DEPRECATED
#endif //TRINITY_COMPILER == TRINITY_COMPILER_GNU

#define UI64FMTD "%" PRIu64
#define UI64LIT(N) UINT64_C(N)

#define SI64FMTD "%" PRId64
#define SI64LIT(N) INT64_C(N)

#define SZFMTD "%" PRIuPTR

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

enum DBCFormer : char
{
    FT_NA = 'x',                                              //not used or unknown, 4 byte size
    FT_NA_BYTE = 'X',                                         //not used or unknown, byte
    FT_STRING = 's',                                          //LocalizedString*
    FT_NSTRING = 'S',                                         // null-terminated string
    FT_FLOAT = 'f',                                           //float
    FT_QWORD = 'q',
    FT_INT = 'i',                                             //uint32
    FT_BYTE = 'b',                                            //uint8
    FT_SHORT = 'w',                                           //uint16 aka word
    FT_SORT = 'd',                                            //sorted by this field, field is not included
    FT_INDEX = 'n',                                           //the same, but parsed to data
    FT_WORD_INDEX = 'z',                                      // 16 bit index
    FT_BYTE_INDEX = 'y',                                      // 8 bit index
    FT_LOGIC = 'l',                                           //Logical (boolean)
    FT_INT_RELATIONSHIP = 'I',
    FT_WORD_RELATIONSHIP = 'W',
    FT_BYTE_RELATIONSHIP = 'B',

    FT_ARR_2 = '2',
    FT_ARR_3 = '3',
    FT_ARR_4 = '4',
    FT_ARR_5 = '5',
    FT_ARR_6 = '6',
    FT_ARR_7 = '7',
    FT_ARR_8 = '8',


    FT_SQL_PRESENT = 'p',                                     //Used in sql format to mark column present in sql db2
    FT_SQL_ABSENT = 'a',                                      //Used in sql format to mark column absent in sql db2
    FT_SQL_SUP = 'o',                                         // Supp sql row (not in db2)
    FT_END = '\0'
};

bool IsRelationshipField(char p_Field);

#define FT_ARR_10 '1', '0'
#define FT_ARR_14 '1', '4'
#define FT_ARR_16 '1', '6'
#define FT_ARR_17 '1', '7'
#define FT_ARR_18 '1', '8'
#define FT_ARR_21 '2', '1'
#define FT_ARR_24 '2', '4'

#endif //TRINITY_DEFINE_H
