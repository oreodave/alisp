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

#ifndef ALISP_H
#define ALISP_H

#include <stdint.h>

/// The bare fucking minimum
#define MAX(A, B)      ((A) > (B) ? (A) : (B))
#define MIN(A, B)      ((A) < (B) ? (A) : (B))
#define ARRSIZE(A)     (sizeof(A) / sizeof((A)[0]))
#define NTH_BYTE(X, N) (((X) >> (8 * N)) & ((1 << 8) - 1))

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
char *sym_table_find(sym_table_t *table, sv_t sv);
void sym_table_cleanup(sym_table_t *table);

/// Basic defintions for a Lisp
#define NIL 0

typedef struct Obj lisp_t;

typedef struct
{
  lisp_t *car, *cdr;
} cons_t;

typedef struct
{
  // 2 levels of indirection... disgusting
  void *data;
} lvec_t;

/// System context - essentially something to help with system management
typedef struct
{
  lisp_t *memory;
  sym_table_t symtable;
} sys_t;

void sys_init(sys_t *);
void sys_register(sys_t *sys, lisp_t *ptr);
void sys_cleanup(sys_t *);

/// Constructors and destructors
lisp_t *make_int(i64 i);
lisp_t *make_vec(sys_t *sys, u64 capacity);
lisp_t *intern(sys_t *sys, sv_t sv);
lisp_t *cons(sys_t *sys, lisp_t *car, lisp_t *cdr);

i64 as_int(lisp_t *);
char *as_sym(lisp_t *);
cons_t *as_cons(lisp_t *);
void *as_vec(lisp_t *);

#define CAR(L) (as_cons(L)->car)
#define CDR(L) (as_cons(L)->cdr)

/// Pointer tagging scheme for lisps

typedef enum Tag
{
  TAG_NIL  = 0b00000000, // Start of atomic types
  TAG_INT  = 0b00000001, // special so we can encode 63 bit integers
  TAG_SYM  = 0b00000100,
  TAG_CONS = 0b00000010, // Start of container types
  TAG_VEC  = 0b00000110,
  NUM_TAGS = 5,
} tag_t;

enum Shift
{
  SHIFT_INT  = 1,
  SHIFT_SYM  = 8,
  SHIFT_CONS = 8,
  SHIFT_VEC  = 8,
};

enum Mask
{
  MASK_INT  = 0b00000001,
  MASK_SYM  = 0b11111111,
  MASK_CONS = 0b11111111,
  MASK_VEC  = 0b11111111,
};

#define TAG(PTR, TYPE)    ((lisp_t *)(((PTR) << SHIFT_##TYPE) | TAG_##TYPE))
#define IS_TAG(PTR, TYPE) (((u64)(PTR) & MASK_##TYPE) == TAG_##TYPE)
#define UNTAG(PTR, TYPE)  (((u64)PTR) >> SHIFT_##TYPE)

#define INT_MAX ((1L << 62) - 1)
#define INT_MIN (-(1L << 62))

tag_t get_tag(lisp_t *lisp);

lisp_t *tag_int(i64 i);
lisp_t *tag_sym(char *str);
lisp_t *tag_cons(cons_t *cons);
lisp_t *tag_vec(lvec_t *lvec);

#endif
