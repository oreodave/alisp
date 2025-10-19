/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the Unlicense
 * for details.

 * You may distribute and modify this code under the terms of the
 * Unlicense, which you should have received a copy of along with this
 * program.  If not, please go to <https://unlicense.org/>.

 * Created: 2025-08-20
 * Description: Stable Vector implementation
 */

#include <malloc.h>
#include <string.h>

#include "./alisp.h"

void vec_init(vec_t *vec, u64 size)
{
  memset(vec, 0, sizeof(*vec));
  if (!vec)
    return;
  else if (size <= VEC_INLINE_CAPACITY)
  {
    vec->is_inlined = 1;
    vec->capacity   = VEC_INLINE_CAPACITY;
    vec->ptr        = NULL;
  }
  else
  {
    vec->is_inlined = 0;
    vec->capacity   = size;
    vec->ptr        = calloc(1, vec->capacity);
  }
}

void vec_free(vec_t *vec)
{
  if (!vec)
    return;
  if (!vec->is_inlined && vec->ptr)
    free(vec->ptr);
  memset(vec, 0, sizeof(*vec));
}

void *vec_data(vec_t *vec)
{
  return vec->is_inlined ? vec->inlined : vec->ptr;
}

void vec_ensure_free(vec_t *vec, u64 size)
{
  if (!vec)
    return;
  if (vec->capacity - vec->size < size)
  {
    vec->capacity = MAX(vec->capacity * VEC_MULT, vec->size + size);
    if (vec->is_inlined)
    {
      // If we're inlined, we need to allocate on the heap now.  So let's copy
      // vec->inlined over to vec->ptr, then turn off inlining.

      // We need to do a two-way swap since vec->ptr and vec->inlined are taking
      // up the same space.
      u8 buffer[VEC_INLINE_CAPACITY];
      memcpy(buffer, vec->inlined, vec->size);
      vec->ptr = calloc(1, vec->capacity);
      memcpy(vec->ptr, buffer, vec->size);
      vec->is_inlined = 0;
    }
    else
      vec->ptr = realloc(vec->ptr, vec->capacity);
  }
}

void vec_append(vec_t *vec, const void *const ptr, u64 size)
{
  if (!vec)
    return;
  vec_ensure_free(vec, size);
  memcpy(vec_data(vec) + vec->size, ptr, size);
  vec->size += size;
}

void vec_clone(vec_t *dest, vec_t *src)
{
  if (!src || !dest)
    return;
  vec_init(dest, src->capacity);
  memcpy(vec_data(dest), vec_data(src), src->size);
}
