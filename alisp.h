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

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/// The bare fucking minimum
#define MAX(A, B)      ((A) > (B) ? (A) : (B))
#define MIN(A, B)      ((A) < (B) ? (A) : (B))
#define ARRSIZE(A)     (sizeof(A) / sizeof((A)[0]))
#define NTH_BYTE(X, N) (((X) >> (8 * N)) & ((1 << 8) - 1))
#define FAIL(MSG)      assert(false && "FAIL: " #MSG)
#define TODO(MSG)      assert(false && "TODO: " #MSG)

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
#define PRD_SV         "%d@%p"

sv_t sv_copy(sv_t);

/// Dynamic arrays

#define VEC_INLINE_CAPACITY 32
#define VEC_MULT            2

typedef struct Vector
{
  u64 size, capacity;
  u8 is_inlined;
  union
  {
    void *ptr;
    alignas(max_align_t) u8 inlined[VEC_INLINE_CAPACITY];
  };
} vec_t;

static_assert(sizeof(vec_t) == 64, "vec_t has to be 64 bytes");

#define VEC_GET(V, T) ((T *)vec_data(V))

void vec_init(vec_t *, u64);
void vec_free(vec_t *);
void *vec_data(vec_t *);
void vec_ensure_free(vec_t *, u64);
void vec_append(vec_t *, const void *const, u64);
void vec_clone(vec_t *, vec_t *);

/// Symbol table
typedef struct
{
  u64 count;    // How many strings?
  u64 capacity; // How many entry buckets?
  vec_t entries;
} sym_table_t;

#define SYM_TABLE_INIT_SIZE (1 << 10)

u64 djb2(sv_t string);
void sym_table_init(sym_table_t *);
char *sym_table_find(sym_table_t *, sv_t);
void sym_table_cleanup(sym_table_t *);

/// Streams
typedef enum
{
  STREAM_TYPE_STRING,
  STREAM_TYPE_FILE,
} stream_type_t;

typedef enum
{
  STREAM_ERR_INVALID_PTR      = -4,
  STREAM_ERR_FILE_NONEXISTENT = -3,
  STREAM_ERR_FILE_READ        = -2,
  STREAM_ERR_PIPE_NONEXISTENT = -1,
  STREAM_ERR_OK               = 0,
} stream_err_t;

typedef struct
{
  vec_t cache;
  FILE *file;
} stream_pipe_t;

typedef struct
{
  stream_type_t type;
  char *name;
  u64 position;
  union
  {
    sv_t string;
    stream_pipe_t pipe;
  };
} stream_t;

#define STREAM_DEFAULT_CHUNK 64

stream_err_t stream_init_string(stream_t *, char *, sv_t);
stream_err_t stream_init_file(stream_t *, char *, FILE *);
void stream_stop(stream_t *);

// End of Content (i.e. we've consumed all cached content/file)
bool stream_eoc(stream_t *);
// size of immediately accessible content
u64 stream_size(stream_t *);

// Return current character, push position by 1
char stream_next(stream_t *);
// Peek current character, do not push position
char stream_peek(stream_t *);
// Seek forward or backward in the stream, return success
bool stream_seek(stream_t *, i64);
bool stream_seek_forward(stream_t *, u64);
bool stream_seek_backward(stream_t *, u64);

// Return a relative substring (using sv_t) of a given size
sv_t stream_substr(stream_t *, u64);
// Return an absolutely located substring (using sv_t) at given index and of
// given size.
sv_t stream_substr_abs(stream_t *, u64, u64);

/// Basic defintions for a Lisp
#define NIL 0

typedef struct Obj lisp_t;

typedef struct
{
  lisp_t *car, *cdr;
} cons_t;

/// System context - essentially something to help with system management
typedef struct
{
  lisp_t *memory;
  sym_table_t symtable;
} sys_t;

void sys_init(sys_t *);
void sys_register(sys_t *, lisp_t *);
void sys_cleanup(sys_t *);

/// Constructors and destructors
lisp_t *make_int(i64);
lisp_t *make_vec(sys_t *, u64);
lisp_t *intern(sys_t *, sv_t);
lisp_t *cons(sys_t *, lisp_t *, lisp_t *);

i64 as_int(lisp_t *);
char *as_sym(lisp_t *);
cons_t *as_cons(lisp_t *);
vec_t *as_vec(lisp_t *);

#define CAR(L) (as_cons(L)->car)
#define CDR(L) (as_cons(L)->cdr)

lisp_t *car(lisp_t *);
lisp_t *cdr(lisp_t *);

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

tag_t get_tag(lisp_t *);

lisp_t *tag_int(i64);
lisp_t *tag_sym(char *);
lisp_t *tag_cons(cons_t *);
lisp_t *tag_vec(vec_t *);

/// Reader
typedef enum
{
  READ_OK = 0,
} read_err_t;

// Attempt to read an expression from the stream, storing it in a pointer,
// returning any errors if failed
read_err_t read(sys_t *, stream_t *, lisp_t **);

// Attempt to read all expressions from a stream till end of content, storing
// them in the given vector.  Return any error at any point during the read.
read_err_t read_all(sys_t *, stream_t *, vec_t *);

#endif
