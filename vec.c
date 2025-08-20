/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-08-19
 * Description: Vector implementation
 */

#include <malloc.h>
#include <string.h>

#include "./alisp.h"

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
