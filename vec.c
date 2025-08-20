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

void vec_free(vec_t *vec)
{
  if (!vec)
    return;
  if (vec->data)
    free(vec->data);
  memset(vec, 0, sizeof(*vec));
}

void vec_ensure_free(vec_t *vec, u64 size)
{
  if (!vec)
    return;
  if (vec->capacity - vec->size < size)
  {
    vec->capacity = MAX(vec->capacity * VEC_MULT, vec->size + size);
    vec->data     = realloc(vec->data, vec->capacity);
  }
}

void vec_append(vec_t *vec, void *ptr, u64 size)
{
  if (!vec)
    return;
  vec_ensure_free(vec, size);
  memcpy(vec->data + vec->size, ptr, size);
  vec->size += size;
}

void vec_clone(vec_t *dest, vec_t *src)
{
  if (!src || !dest)
    return;
  dest       = src;
  dest->data = calloc(1, dest->capacity);
  memcpy(dest->data, src->data, src->size);
}
