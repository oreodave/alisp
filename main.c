/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the Unlicense
 * for details.

 * You may distribute and modify this code under the terms of the
 * Unlicense, which you should have received a copy of along with this
 * program.  If not, please go to <https://unlicense.org/>.

 * Created: 2025-08-19
 * Description: Entrypoint
 */

#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef struct
{
  u64 size, capacity;
  u8 bytes[];
} vec_t;

#define VEC_GET(P)  (((vec_t *)(P)) - 1)
#define VEC_SIZE(P) (VEC_GET(P)->size)
#define VEC_CAP(P)  (VEC_GET(P)->capacity)
#define VEC_MULT    2

void vec_make(void **ptr, u64 size)
{
  if (!ptr)
    return;
  vec_t *vector    = calloc(1, sizeof(*vector) + size);
  vector->size     = 0;
  vector->capacity = size;
  *ptr             = (vector + 1);
}

void vec_free(void **data)
{
  if (!data || !*data)
    return;
  free(VEC_GET(*data));
  *data = NULL;
}

void vec_ensure_remaining(void **ptr, u64 space)
{
  if (!ptr || !*ptr)
    return;
  vec_t *vec = VEC_GET(*ptr);
  if (vec->capacity - vec->size < space)
  {
    void *new_vec = NULL;
    vec_make(&new_vec, MAX(vec->capacity * VEC_MULT, vec->size + space));
    VEC_SIZE(new_vec) = vec->size;
    memcpy(new_vec, *ptr, vec->size);
    vec_free(ptr);
    *ptr = new_vec;
  }
}

void vec_append_byte(void **ptr, u8 byte)
{
  vec_ensure_remaining(ptr, 1);
  vec_t *vec              = VEC_GET(*ptr);
  vec->bytes[vec->size++] = byte;
}

void vec_append(void **ptr, void *data, u64 size)
{
  vec_ensure_remaining(ptr, size);
  vec_t *vec = VEC_GET(*ptr);
  memcpy(*ptr + vec->size, data, size);
  vec->size += size;
}

void vec_clone(void **dest, void **src)
{
  if (!dest || !src || !*src)
    return;
  vec_make(dest, VEC_SIZE(*src));
  memcpy(*dest, *src, VEC_SIZE(*src));
  VEC_SIZE(*dest) = VEC_SIZE(*src);
}

typedef struct
{
  u64 size;
  char *data;
} sv_t;

#define SV(DATA, SIZE) ((sv_t){.data = (DATA), .size = (SIZE)})
#define SV_FMT(SV)     (int)(SV).size, (SV).data
#define PR_SV          "%.*s"

sv_t sv_copy(sv_t old)
{
  char *newstr = calloc(1, old.size * sizeof(*newstr));
  memcpy(newstr, old.data, old.size);
  return SV(newstr, old.size);
}

typedef struct
{
  u64 count;     // How many strings?
  u64 capacity;  // How many entry buckets?
  sv_t *entries; // this is actually a vector on the inside lol
} sym_table_t;

u64 djb2(sv_t string)
{
  u64 hash = 5381;
  for (u64 i = 0; i < string.size; ++i)
    hash = string.data[i] + (hash + (hash << 5));
  return hash;
}

#define SYM_TABLE_INIT_SIZE 1024

void sym_table_init(sym_table_t *table)
{
  table->capacity = MAX(table->capacity, SYM_TABLE_INIT_SIZE);
  table->count    = 0;
  vec_make((void **)&table->entries, table->capacity * sizeof(*table->entries));
}

sv_t sym_table_find(sym_table_t *table, sv_t sv)
{
  // TODO: Deal with resizing this when table->count > table->size / 2
  u64 index = djb2(sv) & (table->capacity - 1);

  for (sv_t comp  = table->entries[index]; comp.data; index += 1,
            index = index & (table->capacity - 1), comp = table->entries[index])
    // Is it present in the table?
    if (sv.size == comp.size && strncmp(sv.data, comp.data, sv.size) == 0)
      return comp;

  // Otherwise we need to duplicate and make it permanently interned
  sv_t newsv            = sv_copy(sv);
  table->entries[index] = newsv;
  ++table->count;

  return newsv;
}

void sym_table_cleanup(sym_table_t *table)
{
  for (u64 i = 0; i < table->capacity; ++i)
    if (table->entries[i].data)
      free(table->entries[i].data);
  vec_free((void **)&table->entries);
  memset(table, 0, sizeof(*table));
}

int main(void)
{
  sym_table_t table = {0};
  sym_table_init(&table);
  // Let's hash the words of lorem ipsum
  const char *words[] = {
      "aliquam",      "erat",      "volutpat",  "nunc",      "eleifend",
      "leo",          "vitae",     "magna",     "in",        "id",
      "erat",         "non",       "orci",      "commodo",   "lobortis",
      "proin",        "neque",     "massa",     "cursus",    "ut",
      "gravida",      "ut",        "lobortis",  "eget",      "lacus",
      "sed",          "diam",      "praesent",  "fermentum", "tempor",
      "tellus",       "nullam",    "tempus",    "mauris",    "ac",
      "felis",        "vel",       "velit",     "tristique", "imperdiet",
      "donec",        "at",        "pede",      "etiam",     "vel",
      "neque",        "nec",       "dui",       "dignissim", "bibendum",
      "vivamus",      "id",        "enim",      "phasellus", "neque",
      "orci",         "porta",     "a",         "aliquet",   "quis",
      "semper",       "a",         "massa",     "phasellus", "purus",
      "pellentesque", "tristique", "imperdiet", "tortor",    "nam",
      "euismod",      "tellus",    "id",        "erat",
  };

  for (u64 i = 0; i < sizeof(words) / sizeof(words[0]); ++i)
  {
    sv_t sv = sym_table_find(&table, SV(words[i], strlen(words[i])));
    printf("%s => %p\n", words[i], sv.data);
  }
  return 0;
}
