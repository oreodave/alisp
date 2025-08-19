/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-08-19
 * Description: All the definitions required for the lisp system
 */

#ifndef BASE_H
#define BASE_H

#include <stdint.h>

/// The bare fucking minimum
#define MAX(A, B)  ((A) > (B) ? (A) : (B))
#define MIN(A, B)  ((A) < (B) ? (A) : (B))
#define ARRSIZE(A) (sizeof(A) / sizeof((A)[0]))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/// String Views for my String Needs
typedef struct
{
  u64 size;
  char *data;
} sv_t;

#define SV(DATA, SIZE) ((sv_t){.data = (DATA), .size = (SIZE)})
#define SV_FMT(SV)     (int)(SV).size, (SV).data
#define PR_SV          "%.*s"

sv_t sv_copy(sv_t old);

/// Dynamic arrays

typedef struct
{
  u64 size, capacity;
  u8 bytes[];
} vec_t;

#define VEC_GET(P)  (((vec_t *)(P)) - 1)
#define VEC_SIZE(P) (VEC_GET(P)->size)
#define VEC_CAP(P)  (VEC_GET(P)->capacity)
#define VEC_MULT    2

void vec_make(void **ptr, u64 size);
void vec_free(void **data);
void vec_ensure_remaining(void **ptr, u64 space);
void vec_append_byte(void **ptr, u8 byte);
void vec_append(void **ptr, void *data, u64 size);
void vec_clone(void **dest, void **src);

/// Symbol table
typedef struct
{
  u64 count;     // How many strings?
  u64 capacity;  // How many entry buckets?
  sv_t *entries; // this is actually a vector on the inside lol
} sym_table_t;

#define SYM_TABLE_INIT_SIZE 1024

u64 djb2(sv_t string);
void sym_table_init(sym_table_t *table);
sv_t sym_table_find(sym_table_t *table, sv_t sv);
void sym_table_cleanup(sym_table_t *table);

#endif
